#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <l4/ankh/client-c.h>
#include <l4/util/util.h>
#include <l4/sys/l4int.h>
#include "sockets.h"

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  3
#define ICMP_ECHO       8
#define ICMP_ECHOREPLY  0
#define GETOPT_LIST_END { 0, 0, 0, 0 }

//#define DEST_IP_STR	"172.21.123.248"	// 测试同一网段的PING
//#define DEST_IP_STR	"192.168.6.3"		// 测试不同网段的PING
//#define DEST_IP_STR	"172.21.123.27"		// 测试自己IP的PING
#define DEST_IP_STR	"172.21.123.28"		// 测试自己的台式机PING
//#define DEST_IP_STR	"127.0.0.1"			// 测试127.xx.xx.xx网段的PING
//#define DEST_IP_STR	"127.12.34.56"			// 测试127.xx.xx.xx网段的PING
//#define DEST_IP_STR	"127.255.255.28"			// 测试127.xx.xx.xx网段的PING
//#define DEST_IP_STR	"0.0.0.0"			// 测试127.xx.xx.xx网段的PING


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
struct timeval tvsend;

int statistics(void);
unsigned short cal_chksum(unsigned short *addr, int len);
int set_icmp_pack(l4_uint16_t pack_no);
int send_packet(void);
int recv_packet(void);
int unpack_icmp(char *buf, int len);
void tv_sub(struct timeval *out,struct timeval *in);
int ankh_ping(void);

int statistics(void)
{       
	printf("%d packets transmitted, %d received , %%%d lost\n", nsend, nreceived, (nsend - nreceived) / nsend * 100);
	close(sockfd);
	return 0;
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

	icmp = (struct icmp*)sendpacket;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = pack_no;
	icmp->icmp_id = pid;
	packsize = 8 + datalen;

	icmp->icmp_cksum = cal_chksum((unsigned short *)icmp, packsize);	/*校验算法*/

	return packsize;
}

/* 发送三个ICMP报文 */
int send_packet()
{
	int packetsize;
	nsend++;
	packetsize = set_icmp_pack(nsend);		/* 设置ICMP报头 */
	gettimeofday(&tvsend, NULL);			/* 记录发送时间 */

	if(sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) { 
		printf("sendto error\n");
		return -1;
	}
	return 0;
}

/* 接收所有ICMP报文 */
int recv_packet()
{       
	int n;
	u32_t fromlen;
	fromlen = sizeof(from);
	while ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen)) >= 0) {
		gettimeofday(&tvrecv, NULL);	/* 记录接收时间 */
		int ret = unpack_icmp(recvpacket, n);
		if ( ret == -2) {
			continue;   // This packet is not for me
		} else if ( ret == -1) {
			printf("unpack_icmp() error\n");
			return -1;
		} else {
			nreceived++; // 正常包
			return 0;
		}
	}
	printf("recvfrom error\n");
	return -1;
}

/* 剥去ICMP报头 */
int unpack_icmp(char *buf, int len)
{       
	int iphdrlen;
	struct ip *ip;
	struct icmp *icmp;
	unsigned int um, ms;

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
		tv_sub(&tvrecv, &tvsend);									/* 本设备和对方设备发送的时间差 */
		ms = tvrecv.tv_usec / 1000 + tvrecv.tv_sec * 1000;			/* 毫秒的整数部分 */
		um = tvrecv.tv_usec % 1000;									/* 毫秒的小数部分 */

		/* 显示相关信息 */
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%d.%03d ms\n",
				len,
				inet_ntoa(from.sin_addr),
				icmp->icmp_seq,
				ip->ip_ttl,
				ms, um);
	} else
		return -2; // This packet is not for me
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

	inet_aton(DEST_IP_STR, &dest_addr.sin_addr);

	pid = 2828;
	printf("---------------------[ping begin]--------------------\n");
	int i = 0;	
	for (i = 0; i < 5; ++i){
		if (send_packet() == -1 ) {										/* 发送所有ICMP报文 */
			printf("send_packet() error\n");
			continue;
		}
		if (recv_packet() == -1 ) {										/* 接收所有ICMP报文 */
			printf("recv_packet() error\n");
			continue;
		}
		sleep(1);
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


