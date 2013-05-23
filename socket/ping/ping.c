#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  3
#define ICMP_ECHOREPLY  0

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
int sockfd, datalen = 56;
int nsend = 0, nreceived = 0; // nsend为发送数据包计数器,nreceived为接收数据包计数器
struct sockaddr_in dest_addr;
pid_t pid;
struct sockaddr_in from;
//struct timeval tvrecv;
struct timespec tvrecv;

void statistics(void);
unsigned short cal_chksum(unsigned short *addr, int len);
int set_icmp_pack(int pack_no);
int send_packet(void);
int recv_packet(void);
int unpack_icmp(char *buf, int len);
//void tv_sub(struct timeval *out,struct timeval *in);
struct timespec tv_sub(struct timespec start, struct timespec end);

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
int set_icmp_pack(int pack_no)
{    
 	int i, packsize;
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
    packetsize = set_icmp_pack(nsend);      /* 设置ICMP报头 */

    struct icmp *icmp;
    icmp = (struct icmp*)sendpacket;
    //printf("send_packet[icmp_type = %x, icmp_code = %x, icmp_id = %d, icmp_seq = %u, icmp_cksum = %d]\n",
     //                   icmp->icmp_type, icmp->icmp_code, icmp->icmp_id, icmp->icmp_seq, icmp->icmp_cksum);

    if(sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto error");
        return -1;
    }
    nsend++;

}

/* 接收所有ICMP报文 */
int recv_packet()
{       
    int n, fromlen;
    extern int errno;

    fromlen = sizeof(from);

    if((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
        //if(errno == EINTR)
        //  continue;
        perror("recvfrom error");
        return;
    }
//    gettimeofday(&tvrecv, NULL);    /* 记录接收时间 */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tvrecv);
    if(unpack_icmp(recvpacket, n) == -1)
        printf("unpack_icmp error\n");
    nreceived++;
}

/* 剥去ICMP报头 */
int unpack_icmp(char *buf, int len)
{       
	int i,iphdrlen;
	struct ip *ip;
	struct icmp *icmp;
//	struct timeval *tvsend;
	struct timespec tvsend;
	struct timespec time_diff;
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
//		tvsend = (struct timeval *)icmp->icmp_data;
		tvsend = *(struct timespec *)icmp->icmp_data;
		time_diff = tv_sub(tvrecv,tvsend);									/* 接收和发送的时间差 */
		rtt = time_diff.tv_sec * 1000 + time_diff.tv_nsec / 1000;		/* 以毫秒为单位计算rtt */

		/* 显示相关信息 */
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",
				len,
				inet_ntoa(from.sin_addr),
				icmp->icmp_seq,
				ip->ip_ttl,
				rtt);
	} else {
		printf("[Wrong :icmp_type = %d icmp_id = %d]\n", icmp->icmp_type, icmp->icmp_id);
		return -1;
	}
	printf("[Right:icmp_type = %d icmp_id = %d]\n", icmp->icmp_type, icmp->icmp_id);
	return 0;
}

int ping_function()
{
	int size = 50 * 1024;

	struct protoent *protocol;
	if((protocol = getprotobyname("icmp")) == NULL){
		perror("getprotobyname");
		exit(1);
	}

	/* 生成使用ICMP的原始套接字,这种套接字只有root才能生成 */
	if((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) {
	//if((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {
		perror("socket error");
		exit(1);
	}   

    /* 扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的
     * 的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
     */
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;

    inet_aton("172.21.123.248", &(dest_addr.sin_addr));

    /* 获取main的进程id, 用于设置ICMP的标志符 */
    //pid = getpid();
    pid = 2388;         // pid是否支持获取pid, 是否直接有函数调用.
    printf("PING (%s): %d bytes data in ICMP packets.\n", inet_ntoa(dest_addr.sin_addr), datalen);
    int i = 0;
    for(i = 0; i < 5; i++) {
        if(send_packet() == -1) {                                       /* 发送所有ICMP报文 */
            printf("send_packet error\n");
            continue;
        }   
		sleep(1);
        if(recv_packet() == -1) {                                       /* 接收ICMP报文 */
            printf("recv_packet error\n");
        }   
    }   
//  statistics(SIGALRM);                                                /* 进行统计 */
    statistics();                                                   /* 进行统计 */
    printf("-------------------- ankh_ping[Over] \n");
    return 0;
}

int main(int argc,char *argv[])
{       
#if 0
	struct hostent *host;
	struct protoent *protocol;
	unsigned long inaddr = 0l;
	int waittime = MAX_WAIT_TIME;
	int size = 50 * 1024;

	if(argc < 2) {
		printf("usage:%s hostname/IP address\n", argv[0]);
		exit(1);
	}

	if((protocol = getprotobyname("icmp")) == NULL){
		perror("getprotobyname");
		exit(1);
	}

	printf("--------------protocol->p_proto = %d----------\n", protocol->p_proto);
	/* 生成使用ICMP的原始套接字,这种套接字只有root才能生成 */
	//if((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) {
	if((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {
		perror("socket error");
		exit(1);
	}

	/* 回收root权限,设置当前用户权限 */
	setuid(getuid());

	/* 扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的
	 * 的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
	 */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
#endif

#if 0
	/* 判断是主机名还是ip地址 */
	if(inaddr = inet_addr(argv[1]) == INADDR_NONE) {
		if((host = gethostbyname(argv[1])) == NULL) {	/* 是主机名 */
			perror("gethostbyname error");
			exit(1);
		}
		memcpy( (char *)&dest_addr.sin_addr,host->h_addr,host->h_length);
	} else												/* 是ip地址 */
		memcpy( (char *)&dest_addr,(char *)&inaddr,host->h_length);

	/* 获取main的进程id, 用于设置ICMP的标志符 */
#endif

//	inet_aton("172.21.123.248", &(dest_addr.sin_addr));

	//pid = getpid();
//	pid = 2088;
//	printf("PING %s(%s): %d bytes data in ICMP packets.\n", argv[1], inet_ntoa(dest_addr.sin_addr), datalen);
	ping_function();

	return 0;
}

/* 两个timeval结构相减 */
#if 1
struct timespec tv_sub(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}
#endif
#if 0
void tv_sub(struct timeval *out, struct timeval *in)
{
	if((out->tv_usec -= in->tv_usec) < 0) {
		-- out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
#endif
