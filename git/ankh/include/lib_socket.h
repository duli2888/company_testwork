#ifndef LIB_SOCKET_H_
#define LIB_SOCKET_H_

#include <stdio.h>



int socket(int domain, int type, int protocol);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		        const struct sockaddr *dest_addr, socklen_t addrlen);


#endif
