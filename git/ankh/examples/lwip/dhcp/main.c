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
/* 
 * Need to include this file before others.
 * Sets our byteorder.
 */
#include "arch/cc.h"

#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include <l4/ankh/shared_ip_addr.h>
#include <l4/sys/l4int.h>
#define GETOPT_LIST_END { 0, 0, 0, 0 }

enum options
{
	BUF_SIZE,
	SHM_NAME,
};

/* Network interface variables */
ip_addr_t ipaddr, netmask, gw;
struct netif netif;

struct netif my_netif;
extern err_t ankhif_init(struct netif*);

static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

#define MAXLINE 4096
static void mysocket_server(void)
{

	int    listenfd, connfd;
	struct sockaddr_in     servaddr;
	char    buff[4096];
	int     n;  

	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3000);                                                                                                                  

	if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   
	printf("bind success\n");
	if( listen(listenfd, 10) == -1){
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   

	printf("listen success\n");
	printf("======waiting for client's request======\n");
	while(1){
		if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
			continue;
		}   
		printf("accept socket success\n");
		n = recv(connfd, buff, MAXLINE, 0); 
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
		close(connfd);
	}   

	close(listenfd);
}

int main(int argc, char **argv)
{
#if 0
#if 1
	v_global_ip_addr = GLOBAL_IP_ADDR;
//	IP4_ADDR(&ipaddr, 10,255,1,249);
	ipaddr.addr = v_global_ip_addr; 
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 10,255,1,1);
#else
	IP4_ADDR(&gw, 192,168,0,1);
	IP4_ADDR(&ipaddr, 192,168,0,249);
	IP4_ADDR(&netmask, 255,255,255,0);

#endif
#else
	IP4_ADDR(&gw, 0,0,0,0);
	IP4_ADDR(&ipaddr, 0,0,0,0);
	IP4_ADDR(&netmask, 0,0,0,0);

#endif

	if (l4ankh_init()){
		return 1;
	}

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;

	static struct option long_opts[] = {
		{"bufsize", 1, 0, BUF_SIZE },
		{"shm", 1, 0, SHM_NAME },
		GETOPT_LIST_END
	};
#define TEST_DHCP 0
#if TEST_DHCP
	l4_uint16_t  i,j;
	l4_int8_t  result;
	l4_uint16_t cnt;

	for(i=0; i<5; i++) {
		result = dhcp_start(&my_netif);
		IP4_ADDR(&ipaddr,0,0,0,0);
		for(cnt=0; (cnt<10) && (ipaddr.addr == 0); cnt++) {
			ipaddr.addr = my_netif.ip_addr.addr;
			//for(j=0;j<100000;j++);
			l4_sleep(1000);
		}
		dhcp_stop(&my_netif);
		if(ipaddr.addr != 0) {
			break;
		}
	}

	 ipaddr.addr  = my_netif.ip_addr.addr;
	 gw.addr = my_netif.gw.addr;
	 netmask.addr = my_netif.netmask.addr;

#endif 

#if 1  // 2012-07-12 sunray

	 printf("lwip: init cfg.bufsize & cfg.shm_name\n");
	 cfg.bufsize = 2048;
	 strcpy(cfg.shm_name,"shm_lwip");

#else
	 while (1) {
		 int optind = 0;
		 int opt = getopt_long(argc, argv, "b:s:", long_opts, &optind);
		printf("getopt: %d\n", opt);

		if (opt == -1)
			break;

		switch(opt) {
			case BUF_SIZE:
				printf("buf size: %d\n", atoi(optarg));
				cfg.bufsize = atoi(optarg);
				break;
			case SHM_NAME:
				printf("shm name: %s\n", optarg);
				snprintf(cfg.shm_name, CFG_SHM_NAME_SIZE, "%s", optarg);
				break;
			default:
				break;
		}
	}
#endif
	// Start the TCP/IP thread & init stuff
	
	tcpip_init(NULL, NULL);
	struct netif *n = netif_add(&my_netif,
	                            &ipaddr, &netmask, &gw,
	                            &cfg, // configuration state
	                            ankhif_init, ethernet_input);

	netif_set_default(&my_netif);
	netif_set_up(&my_netif);  

	assert(n == &my_netif);

	printf("dhcp_start()\n");

	dhcp_start(&my_netif);
	l4_sleep(5000);


	if(my_netif.ip_addr.addr == 0){
		printf("dhcp don't get the IP\n");
		return 0;
	}


	//netif_set_addr(&my_netif,&ipaddr , &netmask, &gw);
	printf("dhcp started\n");
	//netif_set_up(&my_netif);
	while (!netif_is_up(&my_netif))
		l4_sleep(1000);
	printf("[----------Network interface is up.------------]\n");
	printf("IP: "); print_ip(&my_netif.ip_addr); printf("\n");
	printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	shared_ip_addr(my_netif.ip_addr.addr);
	dhcp_stop(&my_netif);
	

	mysocket_server();
//	server();
	
	return 0;
}
