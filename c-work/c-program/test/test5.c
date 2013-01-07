#include <stdio.h>

#define MM ({unsigned int tmp; tmp = 10; printf("Hello world\n");})

int t() {
	return 2;
	//3;
}

int main()
{
	int a;

	a = MM;
	printf("a = %d\n", a);

	a = t();
	printf("a = %d\n", a);
}
