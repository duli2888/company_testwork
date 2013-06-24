#include <stdio.h>
#include "sockets.h"

#include "../../include/shared.h"
#define SERVER_PORT 20000

#define DEST_IP_STR			"172.21.123.28"

#define SELECT 0
#define SOCKET 0
#define SENDTO 0
#define RECVFROM 0
#define CONNECT 0
#define GETSOCKOPT 0
#define SHUTDOWN 0
#define SEND 0
#define GETSOCKNAME 0
#define GETPEERNAME 0
#define IOCTL 0
#define CLOSE 0
#define READ 0
#define WRITE 0

extern l4_uint16_t nsend;

extern int set_icmp_pack(l4_uint16_t pack_no);

int tconnect(void);
int trecvfrom(void);
int tsocket(void);
int tsendto(void);
int tselect(void);
int tgetsockopt(void);
int tshutdown(void);
int tsend(void);
int tgetsockname(void);
int tgetpeername(void);
int tioctlsocket(void);
int tclose(void);
int tread(void);
int twrite(void);

int ftest(void)
{
#if CONNECT
	tconnect();
#endif

#if RECVFROM
	trecvfrom();
#endif

#if SOCKET
	tsocket();
#endif

#if SENDTO
	tsendto();
#endif

#if SELECT
	tselect();
#endif

#if GETSOCKOPT
	tgetsockopt();
#endif

#if SHUTDOWN
	tshutdown();
#endif 

#if SEND
	tsend();
#endif

#if GETSOCKNAME
	tgetsockname();
#endif

#if GETPEERNAME
	tgetpeername();
#endif

#if IOCTL 
	tioctlsocket();
#endif

#if CLOSE
	tclose();
#endif

#if READ
	tread();
#endif

#if WRITE
	twrite();
#endif
	return 0;
}

int tconnect(void)
{
	int fd = 22;
   	socklen_t socklen = 33;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));                                                                                                                                   
	addr.sin_len = 555;
	addr.sin_family = 111;
	inet_aton(DEST_IP_STR, &addr.sin_addr);

	printf("fd = %d, socklen = %d, addr.sin_len = %d, addr.sin_family = %d, addr.sin_addr = %d\n", fd, socklen, addr.sin_len, addr.sin_family, addr.sin_addr);
	if(connect(fd, (struct sockaddr*)&addr, socklen) < 0) {
		printf("can not connect to %s, exit!\n", DEST_IP_STR);
		exit(1);
	}

	return 0;
}

int tselect(void)
{
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	struct timeval timeout={3,0};

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(1, &readfds);
	FD_SET(4, &readfds);
	FD_SET(2, &writefds);
	FD_SET(5, &writefds);
	FD_SET(3, &exceptfds);
	FD_SET(6, &exceptfds);

#if 0
	printf("nfds = %d timeout.tv_sec = %lu timeout.tv_usec = %lu\n", 6, timeout.tv_sec, timeout.tv_usec);                                                    

	printf("readfds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", readfds.fds_bits[0], readfds.fds_bits[1], 
			readfds.fds_bits[2], readfds.fds_bits[3], readfds.fds_bits[4], readfds.fds_bits[5], readfds.fds_bits[7], readfds.fds_bits[8]);
	printf("writefds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", writefds.fds_bits[0], writefds.fds_bits[1], 
			writefds.fds_bits[2], writefds.fds_bits[3], writefds.fds_bits[4], writefds.fds_bits[5], writefds.fds_bits[7], writefds.fds_bits[8]);
	printf("exceptfds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", exceptfds.fds_bits[0], exceptfds.fds_bits[1], 
			exceptfds.fds_bits[2], exceptfds.fds_bits[3], exceptfds.fds_bits[4], exceptfds.fds_bits[5], exceptfds.fds_bits[7], exceptfds.fds_bits[8]);
#endif

	selectsocket(6, &readfds, &writefds, &exceptfds, &timeout);

	return 0;
}

int tsendto(void)
{
	char sendpacket[100] = "Hello, Novel SuperTv Company!!!";
	struct sockaddr_in dest_addr;
//	ssize_t reval;
	bzero(&dest_addr, sizeof(dest_addr));                                                                                                                                   
	dest_addr.sin_family = AF_INET;
	inet_aton(DEST_IP_STR, &dest_addr.sin_addr);


	int sockfd = 1;
	int packetsize;
	packetsize = 100;
	printf("[before sendto]sockfd = %d len = %d flags = %d addrlen = %d\n", sockfd, packetsize, 0, sizeof(dest_addr));
	printf("[before sendto]sin_len = %d sin_family = %d, sin_addr = %d\n", dest_addr.sin_len, dest_addr.sin_family, dest_addr.sin_addr);

	if(sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) { 
		printf("sendto error\n");
		return -1;
	}

	return 0;
}

int tsocket(void)
{
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {
		perror("socket error");
		exit(1);
	}
	printf("socket(%d, %d, %d)----- sockfd = %d\n", AF_INET, SOCK_RAW, 1, sockfd);

	return 0;
}

int trecvfrom(void)
{
	struct sockaddr_in src_addr;
	socklen_t addrlen = 55;
	char buf[TR_BUF_SIZE];
	bzero(&src_addr, sizeof(src_addr));                                                                                                                                   
	src_addr.sin_len = 555;
	src_addr.sin_family = 111;
	inet_aton(DEST_IP_STR, &src_addr.sin_addr);

	printf("before recvfrom addrlen = %d\n", addrlen);
	ssize_t reval = recvfrom(11, buf, 12, 13, (struct sockaddr *)&src_addr, &addrlen);
	//printf("reval = %d\n", reval);
	printf("----------------------after recvfrom reval = %d addrlen = %d\n", reval, addrlen);
	printf("[2]sin_len = %d sin_family = %d sin_addr = %d\n", src_addr.sin_len, src_addr.sin_family, src_addr.sin_addr); 
	printf("client receive data frome server : %s \n", buf);

	return 0;
}

int tgetsockopt(void)
{
	printf("getsockopt()------------------test\n");
	int sockfd = 22, level = 33, optname = 44, reval = 0;
	socklen_t optlen = 99;
	char buf[100];
	reval = getsockopt(sockfd, level, optname, (void *)buf, &optlen);
	printf("sockfd = %d, level = %d, optname = %d, optlen = %d\n buf = [%s]\n", sockfd, level, optname, optlen, buf);
	if (reval == -1) {
		printf("getsockopt call error\n");
		return -1;
	}
	if (reval == 0) printf("getsockopt call successfully\n");
	return 0;
}

int tshutdown(void)
{
	printf("shutdown()------------------test\n");
	int sockfd = 33, how = 44, reval = 0;
	reval = shutdown(sockfd, how);
	printf("sockfd = %d, how = %d, reval = %d\n", sockfd, how, reval);

	return 0;
}

int tsend(void)
{
	printf("send()------------------test\n");
	char sendpacket[100] = "Hello, Server, This is send() function test!!!";
	ssize_t reval;
	int sockfd = 1;
	int packetsize, flags  = 11;
	packetsize = 100;
	printf("sockfd = %d len = %d flags = %d sendpacket = [%s]\n", sockfd, packetsize, flags, sendpacket);

	reval = send(sockfd, sendpacket, packetsize, flags);
	printf("reval = %d\n", reval);
	if(reval < 0) { 
		printf("sendto error\n");
		return -1;
	}

	return 0;
}

int tgetsockname(void)
{
	printf("getsockname()------------------test\n");
	int sockfd = 1;
	struct sockaddr addr;
	socklen_t addrlen;
	int reval = getsockname(sockfd, &addr, &addrlen);
	printf("sockfd = %d addr.sa_len = %d addr.sa_family = %d addr.sa_data = %d addr.sa_data = %d addr.sa_data = %d addrlen = %d reval = %d\n", 
			sockfd, addr.sa_len, addr.sa_family, addr.sa_data[2], addr.sa_data[1], addr.sa_data[13], addrlen, reval);

	return 0;
}

int tgetpeername(void)
{
	printf("getpeername()------------------test\n");
	int reval;
	int sockfd = 1;
	struct sockaddr addr;
	socklen_t addrlen;
	reval = getpeername(sockfd, &addr, &addrlen);
	printf("sockfd = %d addr.sa_len = %d addr.sa_family = %d addr.sa_data = %d addr.sa_data = %d addr.sa_data = %d addrlen = %d reval = %d\n", 
			sockfd, addr.sa_len, addr.sa_family, addr.sa_data[2], addr.sa_data[1], addr.sa_data[13], addrlen, reval);

	return 0;
}

int tioctlsocket(void)
{
	printf("ioctlsocket()------------------test\n");
	int reval;
	int s = 1;
	long cmd = 555;
	char argp[10] = "hello";
	printf("[before]s = %d, cmd = %ld argp = %s\n", s, cmd, argp);
	reval = ioctlsocket(s, cmd, argp);
	printf("[after]s = %d, cmd = %ld argp = [%s] reval = %d\n", s, cmd, argp, reval);

	return 0;
}

int tclose(void)
{
	printf("closesocket()------------------test\n");
	int s = 33, reval = 0;
	reval = closesocket(s);
	printf("s = %d reval = %d\n", s, reval);

	return 0;
}

int tread(void)
{
	printf("read()------------------test\n");
	//reval = read();
	//printf("s = %d reval = %d\n", s, reval);

	return 0;
}

int twrite(void)
{
	printf("write()------------------test\n");
	//reval = read();
	//printf("s = %d reval = %d\n", s, reval);

	return 0;
}
