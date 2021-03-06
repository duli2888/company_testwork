/************************************************* 
 * Author: DuLi
 * Time: 2012-11-23
 * 这个程序实现了TFTP客户端，在Linux上编译通过并正常使用。
 * 使用socket编程，具体的TFTP协议 可以参考网址:
 * ftp://ftp.rfc-editor.org/in-notes/rfc1350.txt
 ************************************************/ 
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

// 下面定义的几个宏是TFTP协议的操作码，参考协议
#define OP_RRQ			1 // 读请求
#define OP_WRQ			2 // 写请求
#define OP_DATA			3 // 数据包
#define OP_ACK			4 // 确认包
#define OP_ERROR		5 // 错误信息


// TFTP错误信息编码
static char *error_string[] = {
    "Not defined error!",					// 0
    "File not found!",						// 1
    "Access denied!",                       // 2
    "Disk full or allocation exceeded!",    // 3
    "Illegal TFTP operation!",              // 4
    "Unknown transfer ID!",                 // 5
    "File already exists!",                 // 6
    "No such user!"                         // 7
};

int main(int argc, char **argv)
{
    int socket_fd;								// socket描述符
    struct sockaddr_in tftp_server, tftp_client;	// 自身和服务器的socket信息
//    FILE *fp;									// 用于保存本地文件
    char buffer[516];							// 网络通信用的buffer
    int addrlen;
    int last_packet, current_packet;				// 包计数

    unsigned short *s_ptr; 
    char *c_ptr;
    int ret;

    if (argc < 3) {								// 使用方式
        printf("Usage: %s <ipaddr> <filename>\n", argv[0]);
        return 0;
    }

    // 创建socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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
        goto error_1;
    }

    // 服务器信息
    bzero(&tftp_server, sizeof(tftp_server));
    addrlen = sizeof(tftp_server);
    tftp_server.sin_family = AF_INET;
    tftp_server.sin_addr.s_addr = inet_addr(argv[1]);
    tftp_server.sin_port = htons(69);						// TFTP服务器端口

    s_ptr = (unsigned short *)&buffer[0];
    *s_ptr = htons(OP_RRQ);									// 操作码

    c_ptr = &buffer[2]; 
    strcpy(&buffer[2], argv[2]);							// 远程文件名
    c_ptr += strlen(argv[2]);
    *c_ptr++ = 0;
    strcpy(c_ptr, "netascii");								// 模式

    c_ptr += 8;
    *c_ptr++ = 0;

    // 创建一个本地文件
#if 0
    if (fp = fopen(argv[2], "w") == NULL) {
        printf("Can not create locale file!\n");
        goto error_1;
    }
#endif

    last_packet = 0;

    while (1) {
        // 发送请求或者回应包到服务器
        if (sendto(socket_fd, buffer, c_ptr - &buffer[0], 0, (struct sockaddr *)&tftp_server, sizeof(tftp_server)) == -1) {
            printf("Can not send buffer!\n");
            goto error_2;
            return 0;
        }

        // 从服务器获取数据包
        if (recvfrom(socket_fd, buffer, 516, 0, (struct sockaddr *)&tftp_server, &addrlen) == -1) {
            printf("Receive data error!\n");
            goto error_2;
			return 0;
		} else {
			switch (ntohs(*s_ptr)) {
				// 数据包
				case OP_DATA:
					current_packet = ntohs(*(s_ptr+1));
					// 验证包的顺序
					if ((last_packet + 1) != current_packet) {
						printf("ERROR: packet error!\n");
						goto error_2;
					}
					last_packet = current_packet;
					ret -= 4;
					// 写入数据到本地文件
#if 0
					int cnt;
					cnt = fwrite(&buffer[4], 1, ret, fp);
					if (ret != cnt) {
						printf("ERROR: write to local file error!\n");
						goto error_2;
					}
#endif
					printf("\n---------------[数据]------------\n%s", &buffer[4]);
					// 如果等于512则不是最后一个包
					if (ret != 512) {
						goto read_over;
					}
					// 准备返回信息
					*s_ptr = htons(OP_ACK);
					*(s_ptr + 1) = htons(current_packet);
					c_ptr = &buffer[4];
					break;

				case OP_ERROR:					// 服务器返回错误信息, 根据错误码，打印错误信息
					printf("ERROR: %s\n", error_string[ntohs(*(s_ptr+1))]);
					goto error_2;
					break;

				default:						// 发生其他未知错误
					printf("ERROR: Unknow error!\n");
					goto error_2;
					break;
			}
		}
	}

read_over:
//	fclose(fp);
	close(socket_fd);
	return 0;

error_2:
//	fclose(fp);
	unlink(argv[2]);
error_1:
	close(socket_fd);

	return 0;
}
