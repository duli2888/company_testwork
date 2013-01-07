#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 4096
#define SSHPORT	22

int main(int argc, char** argv)
{
	int    sockfd, n;
	char    recvline[4096], sendline[4096];
	struct sockaddr_in    servaddr;

	if( argc != 2){ 
		printf("usage: ./client <ipaddress>\n");
		exit(0);
	}   

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		exit(0);
	}   

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SSHPORT);
	if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){ 
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
	}   

	if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){ 
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   

	printf("send msg to server: \n");
	fgets(sendline, 4096, stdin);

	if( send(sockfd, sendline, strlen(sendline), 0) < 0)
	{
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	close(sockfd);
	exit(0);
}

void receive_back_info()
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
	servaddr.sin_port = htons(SSHPORT);

	if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   

	if( listen(listenfd, 10) == -1){
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}   

	printf("======waiting for server's back information======\n");
	while(1){
		if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
			continue;
		}   
		n = recv(connfd, buff, MAXLINE, 0); 
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
		close(connfd);
	}   

	close(listenfd);
}
