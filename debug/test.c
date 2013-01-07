#include <stdio.h>
#include "dldebug.h"

int main()
{
	int a, b, sum = 0;
	printf("This program is for debug\n");
	sum = a + b;
	DLPRINTF("----------[sum = %d]\n", sum);
	
	return 0;
}
