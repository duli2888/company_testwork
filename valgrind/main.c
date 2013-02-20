#include <stdlib.h>
#include <stdio.h>

int *x;
void fun(void)
{
	int i;
	x = malloc(10 * sizeof(int));
	for (i = 0; i < 11; i++) {	//问题1: 数组下标越界
		x[i] = 1000 + i;  
	}
}								//问题2: 内存没有释放

int main(void)
{
	printf("Begin.....\n");
	fun();
	int i;
	for (i = 0; i < 11; i++) {
		printf("%d\n", x[i]);
	}
	printf("\nEND......\n");
	return 0;
}
