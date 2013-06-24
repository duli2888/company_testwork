#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread-l4.h>
#include <l4/util/util.h>
#include <l4/ankh/client-c.h>
#include <l4/ankh/lwip-ankh.h>
#include <l4/ankh/session.h>

#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>
#include <l4/ankh/shared.h>

#include "arch/cc.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

//#include <l4/ankh/shared_ip_addr.h>
#include <l4/sys/l4int.h>

extern "C" {
	#include "lwip/dns.h"
}
/*************全局变量*************************/
char g_recv_buf[TR_BUF_SIZE];
L4::Cap<void> server_as_client;
/**********************************************/

int func_send_call(L4::Cap<void> const &server)
{
	L4::Ipc::Iostream s(l4_utcb());
	l4_addr_t addr = 0;
	int err;

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return 1;
	}   

	s << l4_umword_t(Opcode::Op_buf_send) << (l4_addr_t)addr;
	s << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0); 

	int r = l4_error(s.call(server.cap(), Protocol::Pro_Send_Buf));
	if (r) return r;    // failure

	bzero(g_recv_buf, TR_BUF_SIZE);
	memcpy(g_recv_buf, (const char *)addr, TR_BUF_SIZE);
#if 0
	printf("\n--------------------------[server receive buffer]---------------------------------------------\n");
	int i;
	char *tp = (char *)addr;
	for (i = 1; i <= 80; i++) {
		printf("%.2x ", *(char *)tp);
		tp++;
		if (i % 8 == 0) printf("  ");
		if (i % 16 == 0 ) printf("\n");

	}
	printf("\n--------------------------[server receive buffer]---------------------------------------------\n");
#endif
	return 0;           // ok
}


void *pthread_get_data(void*)
{
	server_as_client = L4Re::Env::env()->get_cap<void>("Stream_Map_Send");
	if (!server_as_client.is_valid()) {
		printf("Could not get capability!\n");
		return NULL;
	}

	if (func_send_call(server_as_client)) {
		printf("Error talking to server\n");
		return NULL;
	}
	return NULL;
}

int get_data()
{
	pthread_t tid;
	if (pthread_create(&tid, NULL, pthread_get_data, NULL) != 0) {
		printf("create thread pthread_get_data error\n");
		return -1;
	}

	if (pthread_join(tid, NULL) != 0) {
		printf("pthread_join error\n");
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
ip_addr_t ipaddr, netmask, gw;
static struct netif my_netif;
static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

extern "C" {
	extern err_t ankhif_init(struct netif*);
	L4_CV void shared_ip_addr(unsigned int ip_addr) L4_NOTHROW;
}

int ankh_init(void)
{
	IP4_ADDR(&ipaddr, 172,21,123,27);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 172,21,123,1);		// 网关是否设置为172.21.123.1

	if (l4ankh_init()) return 1;

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;
	//printf("lwip: init cfg.bufsize & cfg.shm_name\n");
	cfg.bufsize = 2048;
	strcpy(cfg.shm_name,"shm_lwip");

	tcpip_init(NULL, NULL);
	struct netif *n = netif_add(&my_netif,
			&ipaddr, &netmask, &gw,
			&cfg,						// configuration state
			ankhif_init, ethernet_input);

	assert(n == &my_netif);

	netif_set_default(&my_netif);
	netif_set_up(&my_netif);

	while (!netif_is_up(&my_netif))
		l4_sleep(100);
	shared_ip_addr(my_netif.ip_addr.addr);
	printf("[NIC UP]: IP: "); print_ip(&my_netif.ip_addr); printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	return 0;
}

int ankh_static_init(ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2);
int ankh_dhcp_init(void);

int if_config(int mode, ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2)
{
	if (mode == STATIC_IP) { // static IP
		ankh_static_init(ipaddr, netmask, gw, dns1, dns2);
	}

	if (mode == DYNAMIC_IP) { // dynamic IP
		if (ankh_dhcp_init() < 0) {
			printf("[DHCP]Get Ipaddr error\n");
			return -1;
		}
	}

	return 0;
}

int ankh_static_init(ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2)
{
	ip_addr_t sipaddr, snetmask, sgw;
	if (l4ankh_init()) return 1;

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;
	cfg.bufsize = 2048;
	strcpy(cfg.shm_name,"shm_lwip");

	sipaddr.addr = ipaddr.addr;
	snetmask.addr =  netmask.addr;
	sgw.addr = gw.addr;

	tcpip_init(NULL, NULL);
	struct netif *n = netif_add(&my_netif,
			&sipaddr, &snetmask, &sgw,
			&cfg,						// configuration state
			ankhif_init, ethernet_input);


	netif_set_default(&my_netif);
	netif_set_up(&my_netif);

	assert(n == &my_netif);

	dns_setserver((u8_t)0, &dns1);
	dns_setserver((u8_t)1, &dns2);

	while (!netif_is_up(&my_netif))
		l4_sleep(100);

	shared_ip_addr(my_netif.ip_addr.addr);
	printf("[NIC UP]: IP: "); print_ip(&my_netif.ip_addr); printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	return 0;
}

int ankh_dhcp_init(void)
{
	ip_addr_t sipaddr, snetmask, sgw;

	if (l4ankh_init()) return 1;

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;
	cfg.bufsize = 2048;
	strcpy(cfg.shm_name,"shm_lwip");

	
	tcpip_init(NULL, NULL);
	struct netif *n = netif_add(&my_netif,
			&sipaddr, &snetmask, &sgw,
			&cfg,						// configuration state
			ankhif_init, ethernet_input);

	assert(n == &my_netif);

	//////////////////////////////////////////////////
	IP4_ADDR(&sgw, 0,0,0,0);
	IP4_ADDR(&sipaddr, 0,0,0,0);
	IP4_ADDR(&snetmask, 0,0,0,0);

	dhcp_start(&my_netif);
	l4_sleep(20000);

	if(my_netif.ip_addr.addr == 0){
		printf("dhcp don't get the IP\n");
		return -1;
	}

	// sipaddr.addr = my_netif.ip_addr.addr;
	// snetmask.addr = my_netif.netmask.addr;
	// sgw.addr = my_netif.gw.addr;
	printf("dhcp started\n");
	////////////////////////////////////////////////////

	// netif_set_addr(&my_netif, &sipaddr, &snetmask, &sgw);

	netif_set_default(&my_netif);
	netif_set_up(&my_netif);

	while (!netif_is_up(&my_netif))
		l4_sleep(100);

	shared_ip_addr(my_netif.ip_addr.addr);

	printf("[NIC UP]: IP: "); print_ip(&my_netif.ip_addr); printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	return 0;
}
