#include <stdio.h>
#include <stdlib.h>

char *func()
{
	static char *p = NULL;
	p = malloc(30);
	if (p == NULL) {
		printf("malloc error\n");
		return NULL;
	}
	char arr[30] = "hello world";
	printf("strlen = %d sizeof = %d\n", strlen(arr), sizeof(arr));

	memcpy(p, arr, strlen(arr));
	return p;
}

int main()
{
	char *p;
	p = func();
	printf("[%s]\n", p);
}
