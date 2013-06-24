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
#include <l4/ankh/shared_ip_addr.h>
#include <l4/sys/l4int.h> 

#include "arch/cc.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"

// 下面定义的几个宏是TFTP协议的操作码，参考协议
#define OP_RRQ          1 // 读请求
#define OP_WRQ          2 // 写请求
#define OP_DATA         3 // 数据包
#define OP_ACK          4 // 确认包
#define OP_ERROR        5 // 错误信息

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

int network_init(void);
int tftp_pull(const char *str_ip, const char *file_name);

int network_init(void)
{
	ip_addr_t ipaddr, netmask, gw;
	struct netif my_netif;
	extern err_t ankhif_init(struct netif*);
	static ankh_config_info cfg = { 1024, L4_INVALID_CAP, L4_INVALID_CAP, "" };

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

	return 0;
}

int tftp_pull(const char *str_ip, const char *file_name)
{
    int socket_fd;                              	// socket描述符
    struct sockaddr_in tftp_server, tftp_client;    // 自身和服务器的socket信息
    char buffer[516];                           	// 网络通信用的buffer
    int last_packet, current_packet;                // 包计数

    unsigned short *s_ptr; 
    char *c_ptr;
	int ret;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        printf("Can not create socket!\n");
        return -1;
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
        goto error;
    }

    // 服务器信息
    bzero(&tftp_server, sizeof(tftp_server));
    tftp_server.sin_family = AF_INET;
	tftp_server.sin_addr.s_addr = inet_addr(str_ip);
	tftp_server.sin_port = htons(69);                       // TFTP服务器端口

	s_ptr = (unsigned short *)&buffer[0];
	*s_ptr = htons(OP_RRQ);                                 // 操作码

	c_ptr = &buffer[2]; 
	strcpy(&buffer[2], file_name);                          // 远程文件名
	c_ptr += strlen(file_name);
	*c_ptr++ = 0;
	strcpy(c_ptr, "netascii");                              // 模式

	c_ptr += 8;
    *c_ptr++ = 0;

    last_packet = 0;

	while (1) {
		printf("[Send Datagram]\n");
		// 发送请求或者回应包到服务器
		if (sendto(socket_fd, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftp_server, sizeof(tftp_server)) == -1) {
			printf("Can not send buffer!\n");
			goto error;
		}

		// 从服务器获取数据包
		printf("---[Receive Datagram]---\n");
		u32_t addrlen= sizeof(tftp_server);
		ret = recvfrom(socket_fd, buffer, 516, 0, (struct sockaddr *)&tftp_server, &addrlen);
		printf("%s\n", &buffer[4]);
		if (ret < 0) {
			printf("Receive data error!\n");
			goto error;
		} else {
			switch (ntohs(*s_ptr)) {
                case OP_DATA:
                    current_packet = ntohs(*(s_ptr + 1));
                    if ((last_packet + 1) != current_packet) {				// 验证包的顺序
                        printf("ERROR: packet error!\n");
                        goto error;
                    }
					
                    last_packet = current_packet;
                    ret -= 4;
#if 0
					int i = 0;
					for (i = 0; i < ret; i++)								// 打印接收到的信息
						printf("%c", buffer[i + 4]);
					printf("\n--------------------------------------------------------------\n");
#endif
                    if (ret != 512) {										// 如果等于512则不是最后一个包
                        goto read_over;
                    }

                    // 准备返回信息
                    *s_ptr = htons(OP_ACK);
                    *(s_ptr + 1) = htons(current_packet);
                    c_ptr = &buffer[4];
                    break;

                case OP_ERROR:												// 服务器返回错误信息, 根据错误码，打印错误信息
                    printf("ERROR: %s\n", error_string[ntohs(*(s_ptr+1))]);
                    goto error;
                    break;

                default:													// 发生其他未知错误
                    printf("ERROR: Unknow error!\n");
                    goto error;
                    break;
            }
        }
    }

read_over:
    printf("\n---------------[Get Datagram From tftp Server Over]------------\n");
    close(socket_fd);                                           
    return 0;

error:
	printf("ERROR: tftp_pull()\n");
    close(socket_fd);
	return -1;
}

int main(void)
{
	if (network_init() == 1) {
		printf("network_init error\n");
		return -1;
	}

	tftp_pull("172.21.123.28", "hello.txt");

    return 0;
}
