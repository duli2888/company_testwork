#include <stdio.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>
#include <l4/sys/l4int.h>

#include <cstring>
#include <l4/ankh/shared.h>

#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <pthread-l4.h>
#include <l4/util/util.h>
#include <l4/ankh/client-c.h>
#include <l4/ankh/lwip-ankh.h>
#include <l4/ankh/session.h>

#include "arch/cc.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"

#define DEST_IP_STR	"172.21.123.28"	// 测试同一网段的PING

extern char g_recv_buf[TR_BUF_SIZE];
extern char g_send_buf[TR_BUF_SIZE];

int ankh_init(void);
int if_config(int mode, ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2);
int ankh_ping(void);
int get_data();
int send_val_to_client(void *val);
int send_val(struct sockaddr_in val);
int send_packet();


int ankh_dhcp_init(void);

extern "C" {
	#include "lwip/netdb.h"
	struct hostent *lwip_gethostbyname(const char *name);
};

class Ankh_interface_server : public L4::Server_object
{
	public:
		int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);
};

int Ankh_interface_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios)
{
	l4_msgtag_t t;
	ios >> t;

	if (t.label() != Protocol::Ankh_Call)
		return -L4_EBADPROTO;

	L4::Opcode opcode;
	ios >> opcode;

	switch (opcode) {
		case Opcode::NIC_MODE:			// if_config 函数调用
			{
				int mode, reval;
				ip_addr_t ipaddr, netmask, gw, dns1, dns2;
				int i, n, g, d1, d2;
				ios >> mode >> i >> n >> g >> d1 >> d2;
				ipaddr.addr = i;
				netmask.addr = n;
				gw.addr = g;
				dns1.addr = d1;
				dns2.addr = d2;
				reval = if_config(mode, ipaddr, netmask, gw, dns1, dns2);
				ios << reval;
				return L4_EOK;
			}
		case Opcode::socket:			// socket 函数调用
			{
				l4_uint32_t domain, type, protocol, sockfd;
				ios >> domain >> type >> protocol;
				sockfd = socket(domain, type, protocol);
				// printf("socket(%d, %d, %d) -------- sockfd = %d\n", domain, type, protocol, sockfd);
				ios << sockfd;

				return L4_EOK;
			}
		case Opcode::sendto:			// sendto 函数调用
			{
				size_t len;
				socklen_t addrlen;
				int sockfd, flags;
				//struct sockaddr dest_addr;
				struct sockaddr_in dest_addr;
				addrlen = sizeof(dest_addr);

				ios >> sockfd >> len >> flags >> addrlen; 
				get_data();				// 获取buf
				ios.get(dest_addr);		// 获取目的地址

#if 0
				printf("\n--------------------------[sendto() Send Packet packetsize = %d]-------------------------------\n", 80);
				printf("[sendto]sockfd = %d len = %d flags = %d addrlen = %d\n", sockfd, len, flags, addrlen);
				printf("[sendto]sin_len = %d sin_family = %d, sin_addr = %d\n", dest_addr.sin_len, dest_addr.sin_family, dest_addr.sin_addr);
				int i;
				char *tp = g_recv_buf;
				for (i = 1; i <= 80; i++) {
					printf("%.2x ", *(char *)tp);
					tp++;
					if (i % 8 == 0) printf("  ");
					if (i % 16 == 0 ) printf("\n");

				}
				printf("\n--------------------------[sendto()]---------------------------------------------\n");
#endif

				ssize_t reval = sendto(sockfd, g_recv_buf, len, flags, (struct sockaddr *)&dest_addr, addrlen);
				ios << reval;

				return L4_EOK;
			}
		case Opcode::recvfrom:
			{
				int sockfd, flags;
				size_t len;
				struct sockaddr_in dest_addr;
				socklen_t addrlen;
				ssize_t reval;

				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				l4_addr_t snd_base;
				ios >> sockfd >> len >> flags >> addrlen >> snd_base; 

				//get_data();				// 获取buf, 由于此buf，只作返回值, 所以不需要获取buf的数据
				//memcpy(page_to_map, g_recv_buf, L4_PAGESIZE);
				//ios.get(dest_addr);

#if 0	// for test
				//snprintf(page_to_map, sizeof(page_to_map), "Hello from the server!");
				printf("sockfd = %d len = %d flags = %d addrlen = %d\n", sockfd, len, flags, addrlen);
				printf("sin_len = %d sin_family = %d sin_addr = %d\n", dest_addr.sin_len, dest_addr.sin_family, dest_addr.sin_addr);
				//reval = 303;
				//addrlen = 202;
				printf("reval = %d addrlen = %d\n", reval, addrlen);
#endif

				reval = recvfrom(sockfd, page_to_map, len, flags,(struct sockaddr *)&dest_addr, &addrlen);

				ios << addrlen << reval;
				ios.put(dest_addr);
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);

				return L4_EOK;
			}
		case Opcode::setsockopt:	// 未做专门的测试，但是此函数在ping应用中调用无误??
			{
				//printf("setsockopt\n");
				int sockfd = 0, level = 0, optname = 0, reval = 0;
				char *optval = NULL;
				socklen_t optlen;

				ios >> sockfd >> level >> optname >> optlen;
				get_data();				// 获取buf
				optval = (char *)malloc(optlen);
				if (optval == NULL) {
					printf("ankh interface setsockopt malloc() error\n");
					return -L4_ENOSYS;
				}
				memcpy(optval, (const void *)g_recv_buf, optlen);

				//printf("sockfd = %d level = %d, optname = %d optval = %c optlen = %d\n", sockfd, level, optname, optval, optlen);
				reval = setsockopt(sockfd, level, optname, (const void *)optval, optlen);
				//printf("setsockopt reval = %d\n", reval);
				ios << reval;
				free(optval);
				return L4_EOK;
			}
		case Opcode::bind:
			{
				int sockfd = -1, reval = 0;
			   	socklen_t addrlen = -1;
				struct sockaddr addr;
				ios >> sockfd >> addrlen;
				ios.get(addr);
				reval = bind(sockfd, &addr, addrlen);
				ios << reval;
			
				return L4_EOK;
			}
		case Opcode::listen:
			{
				int sockfd = -1, backlog = -1, reval = 0;
				ios >> sockfd >> backlog;
				reval = listen(sockfd, backlog);
				ios << reval;
			
				return L4_EOK;
			}
		case Opcode::accept:
			{
				int sockfd = -1, reval = 0;
			   	struct sockaddr addr;
				socklen_t addrlen;
				ios >> sockfd >> addrlen;
				ios.get(addr);
				reval = accept(sockfd, &addr, &addrlen);
				ios.put(addr);
				ios << addrlen << reval; 

				return L4_EOK;
			}
		case Opcode::recv:
			{
				int sockfd = -1, flags = 0;
				ssize_t reval = 0, len = 0;
				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				l4_addr_t snd_base;

				ios >> sockfd >> len >> flags >> snd_base; 

				//get_data();				// 获取buf, 由于此buf，只作返回值, 所以不需要获取buf的数据
				//memcpy(page_to_map, g_recv_buf, L4_PAGESIZE);

				reval = recv(sockfd, page_to_map, len , flags);
				ios << reval; 
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);

				return L4_EOK;
			}
		case Opcode::gethostbyname:  //lwip_gethostbyname
			{
				l4_addr_t snd_base;
				ios >> snd_base;
				struct hostent *hptr;
				get_data();				// 获取buf

				char name[TR_BUF_SIZE];
				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				memcpy(name, g_recv_buf, TR_BUF_SIZE);
				hptr = gethostbyname(name);
				if (hptr == NULL) {
					printf("gethostbyname error\n");
					return -L4_ENOSYS;
				}

				bzero(page_to_map, TR_BUF_SIZE);

				int ano = 0, lno = 0, i, tlen, pos = 0;
				char *pptr;
				pptr = NULL;

				pptr = hptr->h_name;
				tlen = strlen(pptr);
				memcpy(page_to_map + pos, pptr, tlen);
				pos += tlen;
				page_to_map[pos] = '\0';
				pos ++;
				page_to_map[pos] = '*';
				pos ++;

				pptr = hptr->h_aliases[0];
				for(i = 0; pptr != NULL; i++) {
					tlen = strlen(pptr);
					memcpy(page_to_map + pos, pptr, tlen);
					pos += tlen;
					page_to_map[pos] = '\0';
					pos++;
					page_to_map[pos] = '*';
					pos++;
					++ano;
					pptr = hptr->h_aliases[i + 1]; 
				}   
				hptr->h_aliases[i + 1] = NULL;

				pptr = hptr->h_addr_list[0];
				for(i = 0; pptr != NULL; i++) {
					tlen = strlen(pptr);
					memcpy(page_to_map + pos, pptr, tlen);
					pos += tlen;
					page_to_map[pos] = '\0';
					pos++;
					page_to_map[pos] = '*';
					pos++;
					++lno;
					pptr = hptr->h_addr_list[i + 1]; 
				}   
				pptr = hptr->h_addr_list[i + 1] = NULL;

				ios << ano << lno << hptr->h_addrtype << hptr->h_length;
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);

				return L4_EOK;
			}
			case Opcode::connect:
			{
				int sockfd = -1, reval = 0;
			   	socklen_t addrlen = -1;
				struct sockaddr_in addr;
				//struct sockaddr addr;
				ios >> sockfd >> addrlen;
				ios.get(addr);
				//printf("[Server Before]fd = %d, addrlen = %d, addr.sin_len = %d, addr.sin_family = %d, add.sin_port = %d, addr.sin_addr = %d reval = %d\n", sockfd, addrlen, addr.sin_len, addr.sin_family, addr.sin_port, addr.sin_addr, reval);
				reval = connect(sockfd, (struct sockaddr *)&addr, addrlen);
				//printf("[Server After]fd = %d, addrlen = %d, addr.sin_len = %d, addr.sin_family = %d, addr.sin_port = %d, addr.sin_addr = %d reval =%d\n", sockfd, addrlen, addr.sin_len, addr.sin_family, addr.sin_port, addr.sin_addr, reval);
				ios << reval;
			
				return L4_EOK;
			}
			case Opcode::getsockopt:
			{
				int sockfd = -1, level = 0, optname = 0;
				int reval = 0;
				socklen_t optlen;


				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				l4_addr_t snd_base;

				ios >> sockfd >> level >> optname >> optlen >> snd_base;

#if 0
				get_data();				// 获取buf, 由于此buf，只作返回值, 所以不需要获取buf的数据
				memcpy(page_to_map, g_recv_buf, L4_PAGESIZE);
				char buf[100] = "Hello, Novel SuperTv Company!!!";
				memcpy(page_to_map, buf, 100);
				printf("sockfd = %d, level = %d, optname = %d, optlen = %d\n buf = [%s]\n", sockfd, level, optname, optlen, page_to_map);
#endif

				reval = getsockopt(sockfd, level, optname, page_to_map, &optlen);
				ios << optlen << reval; 
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);

				return L4_EOK;
			}
			case Opcode::shutdown:
			{
				int sockfd = -1, how = 0;
				int reval = 0;
				ios >> sockfd >> how;
				reval = shutdown(sockfd, how);
				//printf("sockfd = %d, how = %d, reval = %d\n", sockfd, how, reval);
				ios << reval;
				return L4_EOK;
			}
			case Opcode::send:
			{
				size_t len;
				int sockfd, flags;
				ssize_t reval = 0;

				ios >> sockfd >> len >> flags; 
				get_data();				// 获取buf

				reval = send(sockfd, g_recv_buf, len, flags);
				//printf("sockfd = %d len = %d flags = %d sendpacket = [%s] reval = %d\n", sockfd, len, flags, g_recv_buf, reval);
				ios << reval;

				return L4_EOK;
			}
			case Opcode::getsockname:
			{
				int sockfd = -1, reval = 0;
			   	struct sockaddr addr;
				socklen_t addrlen;
				ios >> sockfd >> addrlen;
	//			ios.get(addr);
#if 0
				addr.sa_len = 77;
				addr.sa_data[0] = 0;
				addr.sa_data[1] = 1;
				addr.sa_data[13] = 13;
				addrlen = 99;
				printf("sockfd = %d addr.sa_len = %d addr.sa_family = %d addr.sa_data = %d addr.sa_data = %d addr.sa_data = %d addrlen = %d reval = %d\n", 
						sockfd, addr.sa_len, addr.sa_family, addr.sa_data[0], addr.sa_data[1], addr.sa_data[13], addrlen, reval);
#endif
				reval = getsockname(sockfd, &addr, &addrlen);
				ios.put(addr);
				ios << addrlen << reval; 

				return L4_EOK;
			}
			case Opcode::getpeername:
			{
				int sockfd = -1, reval = 0;
			   	struct sockaddr addr;
				socklen_t addrlen;
				ios >> sockfd >> addrlen;
	//			ios.get(addr);
#if 0
				addr.sa_len = 77;
				addr.sa_data[0] = 0;
				addr.sa_data[1] = 1;
				addr.sa_data[13] = 13;
				addrlen = 99;
				printf("sockfd = %d addr.sa_len = %d addr.sa_family = %d addr.sa_data = %d addr.sa_data = %d addr.sa_data = %d addrlen = %d reval = %d\n", 
						sockfd, addr.sa_len, addr.sa_family, addr.sa_data[0], addr.sa_data[1], addr.sa_data[13], addrlen, reval);
#endif
				reval = getpeername(sockfd, &addr, &addrlen);
				ios.put(addr);
				ios << addrlen << reval; 

				return L4_EOK;
			}
			case Opcode::ioctlsocket:
			{
				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				int s, reval;
				long cmd;
				l4_addr_t snd_base;
				ios >> s >> cmd >> snd_base;
				get_data();				// 获取buf
#if 0
				printf("[before]s = %d, cmd = %ld argp = %s\n", s, cmd, g_recv_buf);
				memcpy(page_to_map, "world", strlen("world"));
				reval = 222;
				printf("[after]s = %d, cmd = %ld argp = [%s], reval = %d\n", s, cmd, page_to_map, reval);
#endif
				reval = ioctlsocket(s, cmd, g_recv_buf);
				ios << reval;
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);

				return L4_EOK;
			}
			case Opcode::select:
			{
				int nfds = 0, reval = 0;
				int rf = 0, wf = 0, ef = 0;
				fd_set *readfds;
			   	fd_set *writefds;
			    fd_set *exceptfds;
				struct timeval timeout;

				ios >> nfds >> rf >> wf >> ef >> timeout.tv_sec >> timeout.tv_usec;

				if(rf) {
					readfds = (fd_set *)malloc(sizeof(fd_set));
					if (readfds == NULL) { printf("readfs malloc error\n"); return -1;}
					ios.get(*(readfds->fds_bits));
				} else 
					readfds = NULL;
				if(wf) { 
					writefds = (fd_set *)malloc(sizeof(fd_set));
					if (writefds == NULL) { printf("writefds malloc error\n"); return -1;}
					ios.get(*(writefds->fds_bits));
				} else 
					writefds = NULL;
				if(ef) {
					exceptfds = (fd_set *)malloc(sizeof(fd_set));
					ios.get(*(exceptfds->fds_bits));
					if (exceptfds == NULL) { printf("exceptfds malloc error\n"); return -1;}
				} else 
					exceptfds = NULL;

#if 0
				printf("nfds = %d timeout.tv_sec = %lu timeout.tv_usec = %lu\n", nfds, timeout.tv_sec, timeout.tv_usec);
				printf("readfs:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", readfds->fds_bits[0], readfds->fds_bits[1], 
						readfds->fds_bits[2], readfds->fds_bits[3], readfds->fds_bits[4], readfds->fds_bits[5], readfds->fds_bits[7], readfds->fds_bits[8]);
				printf("writefds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", writefds->fds_bits[0], writefds->fds_bits[1], 
						writefds->fds_bits[2], writefds->fds_bits[3], writefds->fds_bits[4], writefds->fds_bits[5], writefds->fds_bits[7], writefds->fds_bits[8]);
				printf("exceptfds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", exceptfds->fds_bits[0], exceptfds->fds_bits[1], 
						exceptfds->fds_bits[2], exceptfds->fds_bits[3], exceptfds->fds_bits[4], exceptfds->fds_bits[5], exceptfds->fds_bits[7], exceptfds->fds_bits[8]);
#endif

				reval = select(nfds, readfds, writefds, exceptfds , &timeout);

				if(rf) ios.put(*(readfds->fds_bits));
				if(wf) ios.put(*(writefds->fds_bits));
				if(ef) ios.put(*(exceptfds->fds_bits));
				ios << timeout.tv_sec << timeout.tv_usec << reval;

				return L4_EOK;
			}
			case Opcode::closesocket:
			{
				int s, reval;
				ios >> s;
				reval = close(s);
				ios << reval;
#if 0
				printf("-------------------------------s = %d, reval = %d\n", s, reval);
#endif
				return L4_EOK;
			}
			default:
				return -L4_ENOSYS;
	}
}

static L4Re::Util::Registry_server<> server;

int main()
{
	static Ankh_interface_server inter_server;

	if (!server.registry()->register_obj(&inter_server, "ankh_inter").is_valid()) {	
		printf("Could not register my service, is there a 'ankh_interface' in the caps table?\n");
		return 1;
	}

	server.loop();			// Wait for client requests

	return 0;
}
