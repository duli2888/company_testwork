#include <stdio.h>
#include <string.h>

int main()
{
	char arr[20] = "hello world";

	printf("sizeof = %lu\n", sizeof(arr));
	printf("strlen = %d\n", strlen(arr));
	return 0;
}
