#include <stdio.h>
#include "test"

int add(int a, int b)
{
	return (a + b);
}

int sub(int a, int b)
{
	return (a - b);
}

int main()
{

	test *tmp;
	tmp	= new test;

	tmp->test_add();
	
	int a = 20;
	int b = 10;
	int sum = 0;
	sum = add(a, b);
	printf("[a + b] = %d\n", sum);
	sum = sub(a, b);
	printf("[a - b] = %d\n", sum);
	printf("hello world\n");	


	return 0;

}

