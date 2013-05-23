#include <stdio.h>

char *fun(void)
{
	static char arr[20] = "hello world";
	return arr;
}


int main()
{
	char *c;
	c = fun();
	printf("%s\n", c);

	return 0;
}
