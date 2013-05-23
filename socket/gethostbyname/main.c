#include <stdio.h>
#include <netdb.h>
//#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
	char *ptr,**pptr;
	struct hostent *hptr;
	char str[32];

	ptr = argv[1];

	
	if((hptr = gethostbyname(ptr)) == NULL) {				/* 调用gethostbyname()。调用结果都存在hptr中 */
		printf("gethostbyname error for host:%s\n", ptr);
		return 1; 
	}
	
	printf("	official hostname: %s\n",hptr -> h_name);			/* 将主机的规范名打出来 */

	for(pptr = hptr->h_aliases; *pptr != NULL; ++pptr)		/* 主机可能有多个别名，将所有别名分别打出来 */
		printf("  alias:%s\n",*pptr);

	switch(hptr->h_addrtype) {								/* 根据地址类型，将地址打出来 */
		case AF_INET:
		case AF_INET6:
			pptr = hptr->h_addr_list;
			for( ; *pptr !=NULL; ++pptr)						/* 将刚才得到的所有地址都打出来。其中调用了inet_ntop()函数 */
				printf("  address: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
			break;
		default:
			printf("unknown address type\n");
			break;
	}

	return 0;
}
