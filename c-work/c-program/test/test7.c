#include <stdio.h>
struct _addr_t {
	int addr;
};


typedef struct _addr_t ip_addr_t;

int a;
a = 100;
	
int main()
{

	ip_addr_t ip_g_addr;

	ip_g_addr.addr = 20;
	
	printf("%p\n",&ip_g_addr.addr);
	printf(" a = %d\n", a);

	return 0;

}
