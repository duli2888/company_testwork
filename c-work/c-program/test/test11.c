#include <stdio.h>

int func(int * a)
{
	int b = 2;
	*a = b;
}

int main()
{
	int val = 1;

	printf("[1]val = %d\n", val);
	func(&val);
	printf("[2]val = %d\n", val);


}
