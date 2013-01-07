#include <stdio.h>
#include <stdint.h>


//typedef unsigned int ip_addr;

typedef union {
	uint32_t ip_value;
}ip_addr;

int main()
{
	ip_addr ip ;
	ip.ip_value= 0x01020304;	

	printf("%x\n",ip.ip_value);


}
