#include <stdio.h>

int add(int a, int b)
{
	return (a + b);
}

int main()
{
	int sum = 0;
	printf("hello world");


	sum = add(3, 5);

	printf("sum = %d\n", sum);

	return 0;
}
