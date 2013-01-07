#include <stdio.h>
int main()
{
	int i, result = 0;
	
	for (i = 0; i < 10; i++) {
		result += i;
	}
	
	printf("%d ",result);
	printf("\n");
	return 0;
}
