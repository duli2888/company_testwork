/*
 * Author:DuLi
 *
 */

#include<stdlib.h>    
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

char page[]=
"HTTP/1.0 200 OK\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"<head>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
"<title>安全操作系统</title>\n"
"</head>\n"
" <body bgcolor=\"#7EC0EE\">\n"
" <h1>Hello World</h1>\n"
" <p>This reserved page will renovate by user...</p>\n"
" </body>\n"
"</html>\n";

char sbuf[]=
"HTTP/1.0 200 OK\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"<head>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
"<title>安全操作系统</title>\n"
"</head>\n"
" <body bgcolor=\"#7EC0EE\">\n"
" <h3>欢迎来到安全操作系统---微内核操作系统</h3>\n"
" <h2>Micro Kernel</h2>\n"
" <h3>   --Introduce</h3>\n"
" <pre> <font size=\"4\">\n"
" 安全操作系统是由北京永新视博数字电视技术有限公司自主研发的安全操作系统，此系统采用了独树一帜的微内核，具有安全性高，系统小，等特点。\n"
"	Early operating system kernels were rather small, partly because computer memory was limited. As the capability of computers grew,\n"
"the number of devices the kernel had to control also grew. Through the early history of Unix, kernels were generally small, even though\n"
"those kernels contained device drivers and file system managers. When address spaces increased from 16 to 32 bits, kernel design was\n"
"no longer cramped by the hardware architecture, and kernels began to grow.The Berkeley Software Distribution (BSD) of Unix began the era of\n"
"big kernels. In addition to operating a basic system consisting of the CPU, disks and printers, BSD started adding additional file systems,\n"
"a complete TCP/IP networking system, and a number of \"virtual\" devices that allowed the existing programs to work invisibly over the network.\n"
"This growth continued for many years, resulting in kernels with millions of lines of source code. As a result of this growth, kernels were \n"
"more prone to bugs and became increasingly difficult to maintain.The microkernel was designed to address the increasing growth of kernels and\n"
"the difficulties that came with them. In theory, the microkernel design allows for easier management of code due to its division into user space\n" 
"services. This also allows for increased security and stability resulting from the reduced amount of code running in kernel mode. For example,\n"
"if a networking service crashed due to buffer overflow, only the networking service's memory would be corrupted, leaving the rest of the system\n"
"still functional.\n"
"</pre>\n"
" </body>\n"
"</html>\n";

int send_buf(int sockfd, char *buf)
{
	int total = 0, ret = 0, cur = 0, len = 1400;
	total = strlen(buf) + 1;
	if (total < 1400) len = total;
	printf("total length = %d\n", total);
	while (cur < total) {
		ret = send(sockfd, buf + cur, len, 0); 
		if (ret < 0) {
			printf("send error\n");
			return -1; 
		}   
		cur += ret;
		len = total - cur;
		if (len > 1400) len = 1400;
		printf("current = %d ret = %d\n", cur, ret);
	}

	return (total - cur);
}



int web_server() {    
	int create_socket, new_socket;    
	socklen_t addrlen;    
	int bufsize = 1024;    
	char *buffer = (char *)malloc(bufsize);    
	struct sockaddr_in address;    

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {    
		printf("socket() error\n");
		return -1;
	}
	printf("create_socket = %d\n", create_socket);

	address.sin_family = AF_INET;    
	address.sin_addr.s_addr = INADDR_ANY;    
	address.sin_port = htons(80);

	if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {    
		printf("bind() error\n");
		return -1;
	}

	int flage = 0;
	addrlen = sizeof(address);		// 一定要初始化此变量

	if (listen(create_socket, 3) < 0) {    
			printf("server: listen error\n");    
			return -1;
	}

	while (1) {
		printf("------------------------------------[loop]\n");
		if ((new_socket = accept(create_socket, (struct sockaddr *)&address, &addrlen)) == -1) {    
			printf("server: accept error\n");    
			return -1;
		}

		printf("The Client is connected........................new_socket = %d\n", new_socket);

		recv(new_socket, buffer, bufsize, 0);
		//printf("[pthread(1)]%s\n", buffer);

		printf("------------------------------------[send]\n");
		flage = 0;
		if (flage == 0) {
			if (send_buf(new_socket, page) != 0) {
				printf("send_buf error\n");
			}
			flage = 1;
		} else {
			if (send_buf(new_socket, sbuf) != 0) {
				printf("send_buf error\n");
			}
		}
		printf("------------------------------------[End]\n");
#if 1
		if (closesocket(new_socket) == -1) {
			printf("close() error\n");
		}
		printf("------------------------------------[close]\n");
#endif
	}    

	close(create_socket);    
	return 0;    
}
