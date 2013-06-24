#include <l4/sys/err.h>
#include <l4/sys/types.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>
#include <stdio.h>
#include <unistd.h>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>
#include "../../include/shared.h"
#include "lib_socket.h"

#define PING 0
#define DNS_TEST 0
#define TCP_TEST 0
#define UDP_TEST 0
#define FTEST 0
#define WEB_SERVER 0
#define FTP 1

#define MODE_IP 1  // 1: static 0:dynamic

int if_config(int mode, ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2);
int ankh_ping(void);
void mysocket_server(void);
void mysocket_client(void);
void dns_test(void);
int web_server(void);
int ftp_server(void);

#define FILE_NAME			"hello.txt"
#define WRITE_FILE_NAME		"ankh_client.txt"
#define DEST_IP				"172.21.123.28"

int tftp_down_file(const char *filename, const char *dest_ip);
int tftp_up_file(const char *filename, const char *dest_ip);
int ftest(void);

#define IP4_ADDR(ipaddr, a,b,c,d) \
		(ipaddr)->addr = ((u32_t)((d) & 0xff) << 24) | \
		((u32_t)((c) & 0xff) << 16) | \
		((u32_t)((b) & 0xff) << 8)  | \
		(u32_t)((a) & 0xff)



int func_app_main_call()
{

#if 1
	int ret = 0;
#if MODE_IP
	ip_addr_t ipaddr, netmask, gw, dns1, dns2;
	IP4_ADDR(&ipaddr, 172,21,123,27);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 172,21,123,1);
	IP4_ADDR(&dns1, 192,168,6,3);
	// IP4_ADDR(&dns1, 172,21,123,28); // 测试设置的DNS服务器是否其作用
	IP4_ADDR(&dns2, 10,1,1,1);

	ret = if_config(STATIC_IP, ipaddr, netmask, gw, dns1, dns2);
#else
	ret = if_config(DYNAMIC_IP, ipaddr, netmask, gw, dns1, dns2);
#endif
	if (ret < 0) {
		printf("if_config error\n");
		return -1;
	}
#endif

#if FTP
	ftp_server();
#endif

#if FTEST
	ftest();
#endif

#if UDP_TEST 
	tftp_down_file(FILE_NAME, DEST_IP);
	//tftp_up_file(WRITE_FILE_NAME, DEST_IP);
#endif

#if DNS_TEST
	dns_test();
#endif

#if PING
	ankh_ping();
#endif

#if TCP_TEST  // tcp protocol test
	//mysocket_server();
	mysocket_client();
#endif

#if WEB_SERVER
	web_server();
#endif

	printf("Application Exit Success\n");

	return 0;
}

int main()
{
	static L4Re::Util::Registry_server<> client_as_server;
	static Buf_send_to_server buf_sender;
	if (!client_as_server.registry()->register_obj(&buf_sender, "Stream_Map_Send").is_valid()) {
         printf("Could not register my service, is there a 'Stream_Map_Send' in the caps table?\n");
         return -1;
    }

	pthread_t tid;
	if (pthread_create(&tid, NULL, pthread_app, NULL) != 0) {
		printf("pthread_create error\n");
		return -1;
	}

    client_as_server.loop();

	return 0;
}
