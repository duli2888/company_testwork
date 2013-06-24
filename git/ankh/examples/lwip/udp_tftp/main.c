/*
 * Author: DuLi
 * Time: 2013-3-19
 * 这个程序实现了TFTP客户端，在Linux上编译通过并正常使用。
 * 使用socket编程，具体的TFTP协议 可以参考网址:
 * ftp://ftp.rfc-editor.org/in-notes/rfc1350.txt
 */ 
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>

#include <l4/ankh/lwip-ankh.h>
#include <l4/ankh/client-c.h>
#include <l4/util/util.h>
#include "arch/cc.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include <l4/ankh/shared_ip_addr.h>

#include <l4/sys/l4int.h> 

// 下面定义的几个宏是TFTP协议的操作码，参考协议
#define OP_RRQ          1					// 读请求
#define OP_WRQ          2					// 写请求
#define OP_DATA         3					// 数据包
#define OP_ACK          4					// 确认包
#define OP_ERROR        5					// 错误信息

#define FILE_NAME			"hello.txt"
#define WRITE_FILE_NAME		"ankh_client.txt"
#define DEST_IP				"172.21.123.28"

// TFTP错误信息编码
static char *error_string[] = {
    "Not defined error!",                   // 0
    "File not found!",                      // 1
    "Access denied!",                       // 2
    "Disk full or allocation exceeded!",    // 3
    "Illegal TFTP operation!",              // 4
    "Unknown transfer ID!",                 // 5
    "File already exists!",                 // 6
    "No such user!"                         // 7
};

ip_addr_t ipaddr, netmask, gw;
struct netif my_netif;
extern err_t ankhif_init(struct netif*);
static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

int myankh_init(void)
{
    IP4_ADDR(&ipaddr, 172,21,123,27);
    IP4_ADDR(&netmask, 255,255,255,0);
    IP4_ADDR(&gw, 172,21,123,1);			// 网关是否设置为172.21.123.1

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
    printf("GW: "); print_ip(&my_netif.gw);		 printf("\n");

    shared_ip_addr(my_netif.ip_addr.addr);

	return 0;
} 


int tftp_down_file(const char *filename, const char *dest_ip)
{
	int socket_fd;                              	// socket描述符
	struct sockaddr_in tftp_server, tftp_client;    // 自身和服务器的socket信息
	char buffer[516];                           	// 网络通信用的buffer
	int last_packet, current_packet;                // 包计数

	unsigned short *s_ptr; 
	char *c_ptr;
	int ret;

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);		// 创建socket
	if (socket_fd == -1) {
		printf("Can not create socket!\n");
		return 0;
	}

#if 0
	// 客户端自身信息
	bzero(&tftp_client, sizeof (tftp_client));
	tftp_client.sin_family = AF_INET;
	tftp_client.sin_addr.s_addr = INADDR_ANY;
	tftp_client.sin_port = htons(0);

	// 绑定socket到本机IP地址
	ret = bind(socket_fd, (struct sockaddr *)&tftp_client, sizeof(tftp_client));
	if (ret != 0) {
		printf("Can not bind socket!\n");
		return -1;
	}
#endif

	// 服务器信息
	bzero(&tftp_server, sizeof(tftp_server));
	tftp_server.sin_family = AF_INET;
	tftp_server.sin_addr.s_addr = inet_addr(dest_ip);
	tftp_server.sin_port = htons(69);							// TFTP服务器端口

	s_ptr = (unsigned short *)&buffer[0];
	*s_ptr = htons(OP_RRQ);										// 操作码

	c_ptr = &buffer[2]; 
	strcpy(&buffer[2], filename);								// 远程文件名
	c_ptr += strlen(filename);
	*c_ptr++ = 0;
	strcpy(c_ptr, "netascii");									// 模式
	c_ptr += 8;
	*c_ptr++ = 0;

	last_packet = 0;

	while (1) {
		// 发送请求或者回应包到服务器
		if (sendto(socket_fd, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftp_server, sizeof(tftp_server)) == -1) {
			printf("Can not send buffer!\n");
			return -1;
		}
		u32_t addrlen= sizeof(tftp_server);
loop_again:
		// 从服务器获取数据包
		ret = recvfrom(socket_fd, buffer, 516, 0, (struct sockaddr *)&tftp_server, &addrlen);
		if (ret < 0) {
			printf("Receive data error!\n");
			goto loop_again;
		} else {
			switch (ntohs(*s_ptr)) {
				case OP_DATA:										// 数据包
					current_packet = ntohs(*(s_ptr+1));
					if ((last_packet + 1) != current_packet) {		// 验证包的顺序
						printf("ERROR: packet error!\n");
						goto loop_again;
					}
					last_packet = current_packet;
					ret -= 4;
					// printf("%s\n", &buffer[4]);
					// 准备返回信息
					*s_ptr = htons(OP_ACK);
					*(s_ptr + 1) = htons(current_packet);
					c_ptr = &buffer[4];
					if (ret != 512) {	// 如果等于512则不是最后一个包
						if (sendto(socket_fd, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftp_server, sizeof(tftp_server)) == -1) {
							printf("Can not send buffer!\n");
							return -1;
						}
						sleep(2);	// 需要等待，否则的话使得最后一次的确认包可能发不出去;
						close(socket_fd);
						return 0;
					}
					break;

				case OP_ERROR:										// 服务器返回错误信息, 根据错误码，打印错误信息
					printf("ERROR: %s\n", error_string[ntohs(*(s_ptr+1))]);
					return -1;

				default:											// 发生其他未知错误
					printf("ERROR: Unknow error!\n");
					return -2;
			}
		}
	}

	close(socket_fd);

	return 0;
}

int tftp_up_file(const char *filename, const char *dest_ip)
{
	int socket_fd;                              	// socket描述符
	struct sockaddr_in tftp_server, tftp_client;    // 自身和服务器的socket信息
	char buffer[516];                           	// 网络通信用的发送的buffer
	int last_packet, current_packet;                // 包计数

	unsigned short *s_ptr; 
	char *c_ptr;
	int ret;

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);		// 创建socket
	if (socket_fd == -1) {
		printf("Can not create socket!\n");
		return 0;
	}

	// 客户端自身信息
	bzero(&tftp_client, sizeof (tftp_client));
	tftp_client.sin_family = AF_INET;
	tftp_client.sin_addr.s_addr = INADDR_ANY;
	tftp_client.sin_port = htons(0);

	// 绑定socket到本机IP地址
	ret = bind(socket_fd, (struct sockaddr *)&tftp_client, sizeof(tftp_client));
	if (ret != 0) {
		printf("Can not bind socket!\n");
		return -1;
	}

	// 服务器信息
	bzero(&tftp_server, sizeof(tftp_server));
	tftp_server.sin_family = AF_INET;
	tftp_server.sin_addr.s_addr = inet_addr(dest_ip);
	tftp_server.sin_port = htons(69);							// TFTP服务器端口

	s_ptr = (unsigned short *)&buffer[0];
	*s_ptr = htons(OP_WRQ);										// 操作码[写]

	c_ptr = &buffer[2]; 
	strcpy(&buffer[2], filename);								// 远程文件名
	c_ptr += strlen(filename);
	*c_ptr++ = 0;
	strcpy(c_ptr, "netascii");									// 模式
	c_ptr += 8;
	*c_ptr++ = 0;

	last_packet = 0;
	int send_len = c_ptr - &buffer[0];


	while (1) {
		// 发送请求或者回应包到服务器
		if (sendto(socket_fd, buffer, send_len, 0, (struct sockaddr *)&tftp_server, sizeof(tftp_server)) == -1) {
			printf("Can not send buffer!\n");
			return -1;
		}
		u32_t addrlen= sizeof(tftp_server);
loop_again:
		// 从服务器获取数据包
		ret = recvfrom(socket_fd, buffer, 516, 0, (struct sockaddr *)&tftp_server, &addrlen);
		if (ret < 0) {
			printf("Receive data error!\n");
			goto loop_again;
		} else {
			switch (ntohs(*s_ptr)) {
				case OP_ACK:										// 应答 
					current_packet = ntohs(*(s_ptr+1));
					if (last_packet  != current_packet) {			// 验证包的顺序
						printf("ERROR: packet error!\n");
						goto loop_again;
					}
					last_packet++;
					// 准备返回信息
					*s_ptr = htons(OP_DATA);
					*(s_ptr + 1) = htons(last_packet);
					c_ptr = &buffer[4];
					strncpy(&buffer[5], "TFTP is a simple protocol to transfer files, and therefore was named the Trivial File Transfer Protocol or TFTP.  It has been implemented on top of the Internet User Datagram protocol (UDP or Datagram), so it may be used to move files between machines on different networks implementing UDP.  -------------------------------TFTP is a simple protocol to transfer files, and therefore was named the Trivial File Transfer Protocol or TFTP.  It has been implemented on top of the Internet User Datagram protocol (UDP or Datagram), so it may be used to move files between machines on different networks implementing UDP.", 512);
					send_len = 516;
					if (current_packet == 12) {	
						send_len = 100;
						strncpy(&buffer[5], "hello, I come from suka, hello, I come from suka\n[FILE OVER]\n", 100);
					} else if (current_packet == 13) {
						return 0;									// 发送结束
					}
					break;

				case OP_ERROR:										// 服务器返回错误信息, 根据错误码，打印错误信息
					printf("ERROR: %s\n", error_string[ntohs(*(s_ptr + 1))]);
					return -1;

				default:											// 发生其他未知错误
					printf("ERROR: Unknow error!\n");
					return -2;
			}
		}
	}

	close(socket_fd);

	return 0;

}

int main(void)
{
	myankh_init();
	printf("\n---------------[tftp Client]------------\n");
	tftp_down_file(FILE_NAME, DEST_IP);
	//tftp_up_file(WRITE_FILE_NAME, DEST_IP);
	printf("\n---------------[Get Datagram From tftp Server Over]------------\n");

	return 0;
}
