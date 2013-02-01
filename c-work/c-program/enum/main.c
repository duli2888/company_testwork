#include <stdio.h>

enum test
{
	gz=101,
	bin,
	tar,
	targz
};

int main()
{	
	int tmp;
	
	tmp = bin;
	printf("%d\n", tmp);
	
	return 0;
}
