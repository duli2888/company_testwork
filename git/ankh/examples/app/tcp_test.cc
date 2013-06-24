#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread-l4.h>
#include <unistd.h>

#include <l4/util/util.h>
//#include <l4/ankh/client-c.h>
//#include <l4/ankh/session.h>
#include "sockets.h"

#define MAXLINE 4096
void mysocket_server(void)
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

void mysocket_client(void)
{
	int    sockfd;
	char   sendline[4096];
	struct sockaddr_in    servaddr;

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		return;
	}   

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(3000);
	if( inet_aton("172.21.123.28", &servaddr.sin_addr) <= 0){ 
		printf("inet_pton error for %s\n", "172.21.123.28");
		return;
	}   

	if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){ 
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		return;
	}   

	printf("send msg to server: \n");
	//fgets(sendline, 4096, stdin);
	strcpy(sendline, "hello, I come from MicroKernel clinet");
	if( send(sockfd, sendline, strlen(sendline), 0) < 0) {
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		return;
	}
	close(sockfd);
}

