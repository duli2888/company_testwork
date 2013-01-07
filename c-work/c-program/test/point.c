#include <stdio.h>

static unsigned int tmp;

void change(unsigned int *p)
{
	*p = 10;
}


int main()
{

	change(&tmp);
	

	printf("%d\n",tmp);

}
