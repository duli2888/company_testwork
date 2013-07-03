#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <unistd.h>
//#include <netinet/in.h>
//#include <netinet/ip.h>
//#include <netinet/ip_icmp.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <l4/ankh/lwip-ankh.h>
#include <l4/ankh/client-c.h>
#include <l4/util/util.h>
#include "arch/cc.h"

#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "shared_ip_addr.h"

#include <l4/sys/l4int.h>

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  3
#define ICMP_ECHO       8
#define ICMP_ECHOREPLY  0
#define GETOPT_LIST_END { 0, 0, 0, 0 }

ip_addr_t ipaddr, netmask, gw;
struct netif my_netif;
extern err_t ankhif_init(struct netif*);
static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

struct ip
{
	unsigned int ip_hl:4;       /* header length */ /* 大端小端 */
	unsigned int ip_v:4;        /* version */
	u_int8_t ip_tos;            /* type of service */
	u_short ip_len;         /* total length */
	u_short ip_id;          /* identification */
	u_short ip_off;         /* fragment offset field */
	u_int8_t ip_ttl;            /* time to live */
	u_int8_t ip_p;          /* protocol */
	u_short ip_sum;         /* checksum */
	struct in_addr ip_src, ip_dst;  /* source and dest address */
};  


struct icmp
{
	l4_uint8_t icmp_type;
	l4_uint8_t icmp_code;
	l4_uint16_t icmp_cksum;
	l4_uint16_t icmp_id;
	l4_uint16_t icmp_seq;
	u_int8_t    icmp_data[1];
};

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];

int sockfd, datalen = 56;
l4_uint16_t nsend = 0, nreceived = 0; // nsend为发送数据包计数器,nreceived为接收数据包计数器
struct sockaddr_in dest_addr;
pid_t pid;
struct sockaddr_in from;
struct timeval tvrecv;

void statistics(void);
unsigned short cal_chksum(unsigned short *addr, int len);
int set_icmp_pack(l4_uint16_t pack_no);
int send_packet(void);
int recv_packet(void);
int unpack_icmp(char *buf, int len);
void tv_sub(struct timeval *out,struct timeval *in);
int ankh_ping(void);

void statistics(void)
{       
	printf("\n--------------------PING statistics-------------------\n");
	printf("%d packets transmitted, %d received , %%%d lost\n", nsend, nreceived, (nsend - nreceived) / nsend * 100);
	close(sockfd);
	exit(1);
}

/* 校验和算法 */
unsigned short cal_chksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	/* 把ICMP报头二进制数据以2字节为单位累加起来 */
	while(nleft>1) {       
		sum += *w++;
		nleft -= 2;
	}
	/* 若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高字节，这个2字节数据的低字节为0，继续累加 */
	if(nleft == 1) {       
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer =~ sum;
	return answer;
}

/* 设置ICMP报头 */
int set_icmp_pack(l4_uint16_t pack_no)
{    
 	int packsize;
	struct icmp *icmp;
	struct timeval *tval;

	icmp = (struct icmp*)sendpacket;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = pack_no;
	icmp->icmp_id = pid;
	packsize = 8 + datalen;

	tval = (struct timeval *)icmp->icmp_data;
	gettimeofday(tval, NULL);											/*记录发送时间*/
	icmp->icmp_cksum = cal_chksum((unsigned short *)icmp, packsize);	/*校验算法*/

	return packsize;
}

/* 发送三个ICMP报文 */
int send_packet()
{
	int packetsize;
	nsend ++;
	packetsize = set_icmp_pack(nsend);		/* 设置ICMP报头 */
	if(sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) { 
		printf("sendto error\n");
		return -1;
	}
	sleep(1);						/*每隔一秒发送一个ICMP报文*/
	return 0;
}

/* 接收所有ICMP报文 */
int recv_packet()
{       
	int n;
	u32_t fromlen;
	fromlen = sizeof(from);

	if((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
		//if((n = lwip_recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
		//if(errno == EINTR)	
		//	return -1;
			printf("recvfrom error\n");
			return -1;
	}
	gettimeofday(&tvrecv, NULL);	/* 记录接收时间 */
	if(unpack_icmp(recvpacket, n) == -1) {
		printf("unpack_icmp() error\n");
		return -1;
	}
	nreceived++;
	return 0;
}

/* 剥去ICMP报头 */
int unpack_icmp(char *buf, int len)
{       
	int iphdrlen;
	struct ip *ip;
	struct icmp *icmp;
	struct timeval *tvsend;
	double rtt;

	ip = (struct ip *)buf;
	iphdrlen = ip->ip_hl << 2;									/* 求ip报头长度, 即ip报头的长度标志乘4 */
	icmp = (struct icmp *)(buf + iphdrlen);						/* 越过ip报头,指向ICMP报头 */
	len -= iphdrlen;											/* ICMP报头及ICMP数据报的总长度 */

	if(len < 8) {												/* 小于ICMP报头长度则不合理 */
		printf("ICMP packets\'s length is less than 8\n");
		return -1;
	}

	/* 确保所接收的是我所发的的ICMP的回应 */
	if((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid)) {       
		tvsend = (struct timeval *)icmp->icmp_data;
		tv_sub(&tvrecv,tvsend);									/* 接收和发送的时间差 */
		rtt = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000;		/* 以毫秒为单位计算rtt */

		/* 显示相关信息 */
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",
				len,
				inet_ntoa(from.sin_addr),
				icmp->icmp_seq,
				ip->ip_ttl,
				rtt);
	} else
		return -1;
	return 0;
}

int ankh_ping(void)
{       
	int size = 50 * 1024;


	/* 生成使用ICMP的原始套接字,这种套接字只有root才能生成 */
	if((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {
		perror("socket error");
		exit(1);
	}

	/* 扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的
	 * 的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
	 */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;

	inet_aton("172.21.123.248", &dest_addr.sin_addr);
	//inet_aton("172.21.123.248", &dest_addr.sin_addr);

	/* 获取main的进程id, 用于设置ICMP的标志符 */
	//pid = getpid();
	pid = 2828;
//	printf("PING %s(%s): %d bytes data in ICMP packets.\n", argv[1], inet_ntoa(dest_addr.sin_addr), datalen);
	printf("---------------------[ping begin]--------------------\n");
	int i = 0;	
	for (i = 0; i < 5; i++){
			if (sendpacket == NULL) {
				printf("sendpacket malloc error\n");
				continue;
			}
		if (send_packet() == -1 ) {										/* 发送所有ICMP报文 */
			printf("send_packet() error\n");
			continue;
		}
		if (recv_packet() == -1 ) {										/* 接收所有ICMP报文 */
			printf("recv_packet() error\n");
			continue;
		}
	}
	statistics();								/* 进行统计 */

	return 0;
}

/* 两个timeval结构相减 */
void tv_sub(struct timeval *out, struct timeval *in)
{
	if((out->tv_usec -= in->tv_usec) < 0) {
		-- out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
int main(void)
{
	IP4_ADDR(&ipaddr, 172,21,123,27);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 172,21,123,1);  // 网关是否设置为172.21.123.1

	if (l4ankh_init()){
		return 1;
	}

	l4_cap_idx_t c = pthread_getl4cap(pthread_self());
	cfg.send_thread = c;

    printf("lwip: init cfg.bufsize & cfg.shm_name\n");
    cfg.bufsize = 2048;
    strcpy(cfg.shm_name,"shm_lwip");

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

	while (!netif_is_up(&my_netif))
		l4_sleep(1000);

	printf("[----------Network interface is up.------------]\n");
	printf("IP: "); print_ip(&my_netif.ip_addr); printf("\n");
	printf("GW: "); print_ip(&my_netif.gw); printf("\n");

	shared_ip_addr(my_netif.ip_addr.addr);
	ankh_ping();
	printf("ping application is exit success\n");

	return 0;
}
