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

struct netif my_netif;
extern err_t ankhif_init(struct netif*);

static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

static void do_lookup(char *hostname)
{
	printf("[----------------do_lookup---------------------]\n");
	ip_addr_t i;

	struct hostent *h = gethostbyname(hostname);

	printf("gethostbyname(%s): %s\n", hostname, h ? "SUCCESS" : "FAILED");
	if (h) {
		i.addr = *(u32_t*)h->h_addr_list[0];
		printf("h_name: '%s' -->  %d.%d.%d.%d \n", h->h_name, ip4_addr1(&i), ip4_addr2(&i), ip4_addr3(&i), ip4_addr4(&i));
	}
}


static void dns_test(void)
{
	do_lookup("www.baidu.com");
	do_lookup("www.sina.com");
	do_lookup("www.novel-supertv.com");
	do_lookup("www.novel-supertv.com");
	do_lookup("www.163.com");
	do_lookup("www.sohu.com");
	do_lookup("www.google.com");
	do_lookup("www.android.com");
	do_lookup("www.douban.com");
	do_lookup("www.qq.com");
	do_lookup("www.jd.com");
}

int main(int argc, char **argv)
{
	IP4_ADDR(&ipaddr, 172,21,123,27);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 172,21,123,1);

	if (l4ankh_init()) {
		return 1;
	}

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;

	static struct option long_opts[] = {
		{"bufsize", 1, 0, BUF_SIZE },
		{"shm", 1, 0, SHM_NAME },
		GETOPT_LIST_END
	};

    printf("lwip: init cfg.bufsize & cfg.shm_name\n");
    cfg.bufsize = 2048;
    strcpy(cfg.shm_name,"shm_lwip");

	// Start the TCP/IP thread & init stuff
	tcpip_init(NULL, NULL);
	struct netif *n = netif_add(&my_netif,
	                            &ipaddr, &netmask, &gw,
	                            &cfg,							// configuration state
	                            ankhif_init, ethernet_input);

	netif_set_default(&my_netif);
	netif_set_up(&my_netif);

	printf("netif_add: %p (%p)\n", n, &my_netif);
	assert(n == &my_netif);

	while (!netif_is_up(&my_netif))
		l4_sleep(1000);

	printf("[----------Network interface is up.------------]\n");
	printf("IP: "); print_ip(&my_netif.ip_addr); printf("\n");
	printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	shared_ip_addr(my_netif.ip_addr.addr);
	dns_test();

	printf("------------------------------[DNS Test Over]------------------------------------\n");

	return 0;
}
