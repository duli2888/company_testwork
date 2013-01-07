#include <stdio.h>

#define ASSIGN(a,b) b; a=b

int main()
{
	int tmp1 = 5, tmp2 = 6;
	ASSIGN(tmp1, tmp2++);
	printf("tmp1 = %d\n",tmp1);
	printf("tmp2 = %d\n",tmp2);
}
