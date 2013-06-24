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

#include "../../server/netlib/shared_ip_addr.h"

#define GETOPT_LIST_END { 0, 0, 0, 0 }

enum options
{
	BUF_SIZE,
	SHM_NAME,
};

/* Network interface variables */
//ip_addr_t ip_g_addr, g_netmask, g_gw;
ip_addr_t ipaddr, netmask, gw;
struct netif netif;
/* Set network address variables */

#if 0 // ipaddr, netmask, gw set; add by [DuLi:2012-11-12]

ip_g_addr.addr = 2222;
//((u32_t)((10) & 0xff) << 24) | ((u32_t)((255) & 0xff) << 16) | ((u32_t)((1) & 0xff) << 8) | (u32_t)((249) & 0xff);
gw.addr = ((u32_t)((10) & 0xff) << 24) | ((u32_t)((255) & 0xff) << 16) | ((u32_t)((1) & 0xff) << 8) | (u32_t)((1) & 0xff);
netmask.addr = ((u32_t)((255) & 0xff) << 24) | ((u32_t)((255) & 0xff) << 16) | ((u32_t)((255) & 0xff) << 8) | (u32_t)((0) & 0xff);

#endif 

#if 0
IP4_ADDR(gw, 10,255,1,1);
IP4_ADDR(ipaddr, 10,255,1,249);
IP4_ADDR(netmask, 255,255,255,0);
#endif

struct netif my_netif;
extern "C" {extern err_t ankhif_init(struct netif*);}

static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

static void do_lookup(char *hostname)
{
	printf("[----------------do_lookup---------------------]");
	ip_addr_t i;
	struct hostent *h = gethostbyname(hostname);

	printf("gethostbyname(%s): %s\n", hostname, h ? "SUCCESS" : "FAILED");
	if (h) {
		i.addr = *(u32_t*)h->h_addr_list[0];
		printf("   h_name: '%s'\n", h->h_name);
		printf("   --> "); print_ip(&i); printf("\n");
	}
}

static void dns_test(void)
{
	do_lookup("www.baidu.com");
	do_lookup("www.sina.com");
	do_lookup("www.tudos.org");
	do_lookup("www.ifsr.de");
	do_lookup("www.dynamo-dresden.de");
	do_lookup("www.spiegel.de");
	do_lookup("www.smoking-gnu.de");
	do_lookup("www.fail.fx");
}
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

static void server(void)
{
	int err, fd;
	struct sockaddr_in in, clnt;
	in.sin_len = 0;
	in.sin_family = AF_INET;
	in.sin_port = htons(6666);
	in.sin_addr.s_addr = my_netif.ip_addr.addr;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("socket created: %d\n", sock);

	err = bind(sock, (struct sockaddr*)&in, sizeof(in));
	printf("bound to addr: %d\n", err);

	err = listen(sock, 10);
	printf("listen(): %d\n", err);

	for (;;) {
		char buf[1024];
		socklen_t clnt_size = sizeof(clnt);
		fd = accept(sock, (struct sockaddr*)&clnt, &clnt_size);

		printf("Got connection from  ");
		print_ip((ip_addr_t*)&clnt.sin_addr); printf("\n");

		err = read(fd, buf, sizeof(buf));
		printf("Read from fd %d (err %d)\n", fd, err);
		printf("%s", buf);
	}
}


int main(int argc, char **argv)
{
#if 1
#if 1
//	IP4_ADDR(&ipaddr, 10,255,1,249);
	ipaddr.addr = GLOBAL_IP_ADDR; 
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

	printf("netif_add: %p (%p)\n", n, &my_netif);
	assert(n == &my_netif);

//
//printf("dhcp_start()\n");
//
//dhcp_start(&my_netif);
//
//printf("dhcp started\n");

		l4_sleep(1000);
#if 1
	while (!netif_is_up(&my_netif))
		l4_sleep(1000);
#endif

	printf("[----------Network interface is up.------------]\n");
	printf("IP: "); print_ip(&my_netif.ip_addr); printf("\n");
	printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	l4_sleep(1000);
	dns_test();

	mysocket_server();
//	server();
	
	return 0;
}
