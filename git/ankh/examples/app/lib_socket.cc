#include <stdio.h>
#include <l4/sys/err.h>
#include <l4/sys/types.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>

#include <l4/ankh/shared.h>
#include "lib_socket.h"
#include <ctype.h>

L4::Cap<void> client_app;
char g_send_buf[TR_BUF_SIZE];

int if_config(int mode, ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw, ip_addr_t dns1, ip_addr_t dns2)
{
	int reval;
	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::NIC_MODE) << mode << ipaddr.addr << netmask.addr << gw.addr << dns1.addr << dns2.addr;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("if_config ios.call error\n");
	   	return r;
	}
	ios >> reval;

	return reval;
}

int socket(int domain, int type, int protocol)
{
	int reval;
	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::socket) << domain << type << protocol;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("socket ios.call error\n");
	   	return r;
	}
	ios >> reval;

	return reval;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
				const struct sockaddr *dest_addr, socklen_t addrlen)
{
	//printf("sendto\n");
	//strncpy(g_send_buf, (const char *)buf, TR_BUF_SIZE);
	memcpy(g_send_buf, (const char *)buf, len);

	ssize_t reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::sendto) << sockfd << len << flags << addrlen;
	ios.put(*dest_addr);
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("sendto ios.call error\n");
	   	return r;
	}
	ios >> reval; 

	return reval;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
		struct sockaddr *src_addr, socklen_t *addrlen)
{
	//printf("recvfrom\n");
	L4::Ipc::Iostream s(l4_utcb());

	l4_addr_t addr = 0;
	ssize_t reval = 0;
	socklen_t taddrlen;
	struct sockaddr_in tsrc_addr;
	int err;

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return 1;
	}

	s << l4_umword_t(Opcode::recvfrom) << sockfd << len << flags << *addrlen << (l4_addr_t)addr;
	s << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0);
	//s.put(*src_addr);

	int r = l4_error(s.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("recvfrom ios.call error\n");
	   	return r;
	}
	s >> taddrlen >> reval; 
	*addrlen = taddrlen;
	s.get(tsrc_addr);
	memcpy((char *)src_addr, (const char *)&tsrc_addr, sizeof(struct sockaddr_in));
	memcpy((char *)buf, (const char *)addr, reval);

#if 0
			printf("\n----------------[receive buf]----------------------------------------\n");
			int i;
			char *tp = (char *)buf;
			for (i = 1; i <= 80; ++i) {
				printf("%.2x ", *(char *)tp);
				tp++;
				if (i % 8 == 0) printf("  ");
				if (i % 16 == 0 ) printf("\n");

			}
			printf("[before sendto]addrlen = %d reval = %d\n", taddrlen, reval);
			printf("[before sendto]sin_len = %d sin_family = %d, sin_addr = %d\n", tsrc_addr.sin_len, tsrc_addr.sin_family, tsrc_addr.sin_addr);
			printf("\n--------------------------------------------------------\n");
#endif

	return reval;
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	memcpy((void *)g_send_buf, optval, optlen);

	int reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::setsockopt) << sockfd << level << optname << optlen;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("setsockopt ios.call error\n");
	   	return r;
	}
	ios >> reval; 

	return reval;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::bind) << sockfd << addrlen;
	ios.put(*addr);
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("bind ios.call error\n");
	   	return r;
	}
	ios >> reval; 

	return reval;
}

int listen(int sockfd, int backlog)
{
	int reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::listen) << sockfd << backlog;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("listen ios.call error\n");
	   	return r;
	}
	ios >> reval; 

	return reval;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	//printf("accept\n");
	L4::Ipc::Iostream ios(l4_utcb());

	int reval;
	struct sockaddr taddr;
	socklen_t taddrlen;


	if (addrlen == NULL) {
		taddrlen = 0;
	} else {
		taddrlen = *addrlen;
	}

	ios << l4_umword_t(Opcode::accept) << sockfd << taddrlen;
	if (addr != NULL) {
		ios.put(*addr);
	}
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("accept ios.call error\n");
	   	return r;
	}
	ios.get(taddr);
	ios >> taddrlen >> reval; 
	if (addrlen != NULL) {
		*addrlen = taddrlen;
	}
	if (addr != NULL) {
		*addr = taddr;
	}

	return reval;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
	l4_addr_t addr = 0;
	ssize_t reval = 0;
	int err;
	L4::Ipc::Iostream ios(l4_utcb());

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return -1;
	}

	ios << l4_umword_t(Opcode::recv) << sockfd << len << flags << (l4_addr_t)addr;
	ios << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0);

	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("recv ios.call error\n");
	   	return r;
	}
	ios >> reval; 
	memcpy((char *)buf, (const char *)addr, reval);

	return reval;
}

struct hostent *gethostbyname(const char *name)
{
	bzero(g_send_buf, TR_BUF_SIZE);
	memcpy((void *)g_send_buf, name, strlen(name));
	printf("%s\n", g_send_buf);
	static struct hostent *hptr = NULL;
	hptr = (struct hostent*)malloc(sizeof(struct hostent));
	if (hptr == NULL) {
		printf("hptr malloc error\n");
		return NULL;
	}   
	hptr->h_name = (char *)malloc(50);
	if (hptr->h_name == NULL) {
		printf("hptr malloc error\n");
		free(hptr);
		return NULL;
	}   

	hptr->h_aliases = (char **)malloc(5);
	if (hptr->h_aliases == NULL) {
		printf("hptr malloc error\n");
		free(hptr->h_name);
		free(hptr);
		return NULL;
	}   

	hptr->h_addr_list = (char **)malloc(5);
	if (hptr->h_aliases == NULL) {
		printf("hptr malloc error\n");
		free(hptr->h_aliases);
		free(hptr->h_name);
		free(hptr);
		return NULL;
	}   

	L4::Ipc::Iostream s(l4_utcb());
	l4_addr_t addr = 0;
	int err;

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return NULL;
	}   

	s << l4_umword_t(Opcode::gethostbyname) << (l4_addr_t)addr;
	s << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0); 
	int r = l4_error(s.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) return NULL; 

	int ano = 0, lno = 0, tlen = 0, pos = 0, tno = 0;
	char *ph, *cur;
	s >> ano >> lno >> hptr->h_addrtype >> hptr->h_length;
	ph = cur = (char *)addr;
	while (*cur != '*') {
		cur++;
		tlen++;
	}
	cur++;
	memcpy(hptr->h_name, ph, tlen);
	pos += tlen;
	pos += 1;
	ph = (char *)addr + pos;

	tno = tlen = 0;
	for (tno = 0; tno < ano; tno++) {
		tlen = 0;
		while (*cur != '*') {
			cur++;
			tlen++;
		}
		hptr->h_aliases[tno] = (char *)malloc(50);
		if (hptr->h_aliases[tno] == NULL) {
			printf("hptr malloc error\n");
			return NULL;
		}
		memcpy(hptr->h_aliases[tno], ph, tlen - 1);
		cur++;
		pos += tlen;
		pos += 1;
		ph = (char *)addr + pos;
	}
	hptr->h_aliases[tno] = NULL;

	tno = tlen = 0;
	for (tno = 0; tno < lno; tno++) {
		tlen = 0;
		while (*cur != '*') {                                        
			cur++;
			tlen++;
		}
		hptr->h_addr_list[tno] = (char *)malloc(50);
		if (hptr->h_addr_list[tno] == NULL) {
			printf("hptr malloc error\n");
			return NULL;
		}
		memcpy(hptr->h_addr_list[tno], ph, tlen - 1);
		cur++;
		pos += tlen;
		pos += 1;
		ph = (char *)addr + pos;
	}
	hptr->h_addr_list[tno] = NULL;

	return hptr;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::connect) << sockfd << addrlen;
	ios.put(*addr);

	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("connect ios.call error\n");
	   	return r;
	}
	ios >> reval; 

	return reval;
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
	int reval;
	l4_addr_t addr = 0;
	socklen_t toptlen;
	int err;

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return -1;
	}


	L4::Ipc::Iostream ios(l4_utcb());

	ios << l4_umword_t(Opcode::getsockopt) << sockfd << level << optname << *optlen << (l4_addr_t)addr;
	ios << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0);

	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("getsockopt ios.call error\n");
	   	return r;
	}
	ios >> toptlen >> reval; 
	*optlen = toptlen;
	memcpy((char *)optval, (char *)addr, toptlen);

	return reval;
}

int shutdown(int sockfd, int how)
{
	int reval;
	L4::Ipc::Iostream ios(l4_utcb());

	ios << l4_umword_t(Opcode::shutdown) << sockfd << how;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("shutdown ios.call error\n");
	   	return r;
	}

	ios >> reval; 
	return reval;
}

size_t send(int sockfd, const void *buf, size_t len, int flags)
{
	memcpy(g_send_buf, (const char *)buf, len);

	ssize_t reval;

	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::send) << sockfd << len << flags;
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("send ios.call error\n");
		return r;
	}
	ios >> reval; 

	return reval;
}

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	//printf("getsockname\n");
	L4::Ipc::Iostream ios(l4_utcb());

	int reval;
	struct sockaddr taddr;
	socklen_t taddrlen;


	if (addrlen == NULL) {
		taddrlen = 0;
	} else {
		taddrlen = *addrlen;
	}

	ios << l4_umword_t(Opcode::getsockname) << sockfd << taddrlen;
	if (addr != NULL) {
		//ios.put(*addr);
	}
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("getsockname ios.call error\n");
	   	return r;
	}
	ios.get(taddr);
	ios >> taddrlen >> reval; 
	if (addrlen != NULL) {
		*addrlen = taddrlen;
	}
	if (addr != NULL) {
		*addr = taddr;
	}

	return reval;
}

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	//printf("call getpeername\n");
	L4::Ipc::Iostream ios(l4_utcb());

	int reval;
	struct sockaddr taddr;
	socklen_t taddrlen;


	if (addrlen == NULL) {
		taddrlen = 0;
	} else {
		taddrlen = *addrlen;
	}

	ios << l4_umword_t(Opcode::getpeername) << sockfd << taddrlen;
	if (addr != NULL) {
		//ios.put(*addr);
	}
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) {
		printf("getpeername ios.call error\n");
	   	return r;
	}
	ios.get(taddr);
	ios >> taddrlen >> reval; 
	if (addrlen != NULL) {
		*addrlen = taddrlen;
	}
	if (addr != NULL) {
		*addr = taddr;
	}

	return reval;
}

int ioctlsocket(int s, long cmd, void *argp) 
{
	strcpy(g_send_buf, (const char *)argp);
	L4::Ipc::Iostream ios(l4_utcb());
	l4_addr_t addr = 0;
	int reval;
	int err;

	if ((err = L4Re::Env::env()->rm()->reserve_area(&addr, L4_PAGESIZE, L4Re::Rm::Search_addr))) {
		printf("The reservation of one page within our virtual memory failed with %d\n", err);
		return -1;
	}

	ios << l4_umword_t(Opcode::ioctlsocket) << s << cmd << (l4_addr_t)addr;
	ios << L4::Ipc::Rcv_fpage::mem((l4_addr_t)addr, L4_PAGESHIFT, 0);
	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) { printf("ioctlsocket ios.call error\n"); return r; }
	ios >> reval; 
	strcpy((char *)argp, (const char *)addr);

	return reval;
}

#if 1

int selectsocket(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int reval;
	int rf = 0, wf = 0, ef = 0;
	if(readfds != NULL) rf = 1;
	if(writefds != NULL) wf = 1;
	if(exceptfds != NULL) ef = 1;
	
	L4::Ipc::Iostream ios(l4_utcb());

	ios << l4_umword_t(Opcode::select) << nfds << rf << wf << ef << timeout->tv_sec << timeout->tv_usec;

	if(rf) ios.put(*(readfds->fds_bits));
	if(wf) ios.put(*(writefds->fds_bits));
	if(ef) ios.put(*(exceptfds->fds_bits));

#if 0
	printf("nfds = %d timeout.tv_sec = %lu timeout.tv_usec = %lu\n", 6, timeout->tv_sec, timeout->tv_usec);                                                    

	printf("readfds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", readfds->fds_bits[0], readfds->fds_bits[1], 
			readfds->fds_bits[2], readfds->fds_bits[3], readfds->fds_bits[4], readfds->fds_bits[5], readfds->fds_bits[7], readfds->fds_bits[8]);
	printf("writefds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", writefds->fds_bits[0], writefds->fds_bits[1], 
			writefds->fds_bits[2], writefds->fds_bits[3], writefds->fds_bits[4], writefds->fds_bits[5], writefds->fds_bits[7], writefds->fds_bits[8]);
	printf("exceptfds:%lu - %lu - %lu - %lu - %lu - %lu - %lu - %lu\n", exceptfds->fds_bits[0], exceptfds->fds_bits[1], 
			exceptfds->fds_bits[2], exceptfds->fds_bits[3], exceptfds->fds_bits[4], exceptfds->fds_bits[5], exceptfds->fds_bits[7], exceptfds->fds_bits[8]);
#endif

	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) { printf("selectsocket ios.call error\n"); return r; }

	if(rf) ios.get(*(readfds->fds_bits));
	if(wf) ios.get(*(writefds->fds_bits));
	if(ef) ios.get(*(exceptfds->fds_bits));
	ios >> timeout->tv_sec >> timeout->tv_usec >> reval; 

	return reval;
}
#endif

int closesocket(int s)
{
	int reval = 0;
	L4::Ipc::Iostream ios(l4_utcb());
	ios << l4_umword_t(Opcode::closesocket) << s;

	int r = l4_error(ios.call(client_app.cap(), Protocol::Ankh_Call));
	if (r) { printf("close ios.call error\n"); return r; }
	ios >> reval; 

	return reval;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *pthread_app(void *)
{
	client_app = L4Re::Env::env()->get_cap<void>("ankh_inter");
	if (!client_app.is_valid()) {
		printf("Could not get capability!\n");
		return NULL;
	}   

	if (func_app_main_call()) {
		printf("Error talking to server\n");
		return NULL;
	}   
	return NULL;
}

int Buf_send_to_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios)
{
	l4_msgtag_t t;
	ios >> t;

	if (t.label() != Protocol::Pro_Send_Buf)
		return -L4_EBADPROTO;

	L4::Opcode opcode;
	ios >> opcode;

	switch (opcode) {
		case Opcode::Op_buf_send:
			{
				l4_addr_t snd_base;
				ios >> snd_base;
				static char page_to_map[L4_PAGESIZE] __attribute__((aligned(L4_PAGESIZE)));
				memcpy(page_to_map, g_send_buf, sizeof(page_to_map));
				ios << L4::Ipc::Snd_fpage::mem((l4_addr_t)page_to_map, L4_PAGESHIFT, L4_FPAGE_RO, snd_base);
				return L4_EOK;
			}
		default:
			return -L4_ENOSYS;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *ipaddr_ntoa_r(const ip_addr_t *addr, char *buf, int buflen);

#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

u32_t ipaddr_addr(const char *cp)
{
	ip_addr_t val;

	if (ipaddr_aton(cp, &val)) {
		return ip4_addr_get_u32(&val);
	}
	return (IPADDR_NONE);
}

int ipaddr_aton(const char *cp, ip_addr_t *addr)
{
	u32_t val;
	u8_t base;
	char c;
	u32_t parts[4];
	u32_t *pp = parts;

	c = *cp;
	for (;;) {
		/*  
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, 1-9=decimal.
		 */
		if (!isdigit(c))
			return (0);
		val = 0;
		base = 10; 
		if (c == '0') {
			c = *++cp;
			if (c == 'x' || c == 'X') {
				base = 16; 
				c = *++cp;
			} else
				base = 8;
		}   
		for (;;) {
			if (isdigit(c)) {
				val = (val * base) + (int)(c - '0');
				c = *++cp;
			} else if (base == 16 && isxdigit(c)) {
				val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
				c = *++cp;
			} else
				break;
		}
		if (c == '.') {
			/*  
			 * Internet format:
			 *  a.b.c.d                                     
			 *  a.b.c   (with c treated as 16 bits)
			 *  a.b (with b treated as 24 bits)
			 */
			if (pp >= parts + 3) {
				return (0);
			}
			*pp++ = val;
			c = *++cp;
		} else
			break;
	}
	if (c != '\0' && !isspace(c)) {
		return (0);
	}
	switch (pp - parts + 1) {
		case 0:
			return (0);       /* initial nondigit */

		case 1:             /* a -- 32 bits */
			break;

		case 2:             /* a.b -- 8.24 bits */
			if (val > 0xffffffUL) {
				return (0);
			}
			val |= parts[0] << 24;
			break;

		case 3:             /* a.b.c -- 8.8.16 bits */
			if (val > 0xffff) {
				return (0);
			}
			val |= (parts[0] << 24) | (parts[1] << 16);
			break;

		case 4:             /* a.b.c.d -- 8.8.8.8 bits */            
			if (val > 0xff) {
				return (0);
			}
			val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
			break;
		default:
			printf("ipaddr_aton() error\n");
			break;
	}
	if (addr) {
		ip4_addr_set_u32(addr, htonl(val));
	}
	return (1);
}
char *ipaddr_ntoa(const ip_addr_t *addr)
{
	  static char str[16];
	    return ipaddr_ntoa_r(addr, str, 16);
}

char *ipaddr_ntoa_r(const ip_addr_t *addr, char *buf, int buflen)
{
	u32_t s_addr;
	char inv[3];
	char *rp;
	u8_t *ap;
	u8_t rem;
	u8_t n;
	u8_t i;
	int len = 0;

	s_addr = ip4_addr_get_u32(addr);

	rp = buf;
	ap = (u8_t *)&s_addr;
	for(n = 0; n < 4; n++) {
		i = 0;
		do {
			rem = *ap % (u8_t)10;
			*ap /= (u8_t)10;
			inv[i++] = '0' + rem;
		} while(*ap);
		while(i--) {
			if (len++ >= buflen) {
				return NULL;
			}
			*rp++ = inv[i];
		}
		if (len++ >= buflen) {
			return NULL;
		}
		*rp++ = '.';
		ap++;
	}
	*--rp = 0;
	return buf;
}

u32_t htonl(u32_t n)
{
	return ((n & 0xff) << 24) |
		((n & 0xff00) << 8) |
		((n & 0xff0000UL) >> 8) |
		((n & 0xff000000UL) >> 24);
}

u32_t ntohl(u32_t n)
{
	return htonl(n);
}

u16_t htons(u16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

u16_t ntohs(u16_t n)
{
   return htons(n);
}



