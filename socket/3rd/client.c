#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 4096

int main(int argc, char** argv)
{
	int    sockfd;
	char    recvline[4096], sendline[4096];
	struct sockaddr_in    servaddr;
	int sn;

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(3000);
	if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s\n",argv[1]);
		return -1;
	}
	if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		return -1;
	}

	strcpy(sendline, "I am a client,Hello,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,Hello, I am a client,");
		printf("send msg to server: \n");
		//fgets(sendline, 4096, stdin);
		sn = send(sockfd, sendline, strlen(sendline), 0);
		printf("SEND Date number = %d\n", sn);
		if(sn < 0) {
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		sleep(3);
	close(sockfd);
	return 0;
}
