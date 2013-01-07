#include <stdio.h>

int g; 

void test_add(int a, int b)
{
	int tmp;
	static int test;
	tmp = a + b;
	printf("[test]%d\n",test);
	test++;
	return;
}

int main()
{
	
	int a = 5;
	int b = 10;
	
	int sum = 0;

	char arr[] = "hello gdb";

	test_add(a, b);
	a = 20;
	puts(arr);
	printf("[a + b] = %d \n",sum);


	return 0;
}
