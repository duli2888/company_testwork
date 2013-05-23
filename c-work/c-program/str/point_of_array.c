#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char **p;

	p[0] = (char *)malloc(20);

	p[0] = (char *)malloc(20);
	strcpy(p[0], "hello1");
	p[1] = (char *)malloc(20);
	strcpy(p[1], "hello2");
	p[2] = NULL;
	char *cur;
	for (cur = *p; cur != NULL; cur++) 
		printf("%s\n", cur);
	

	return 0;
}
