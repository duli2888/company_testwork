#include <stdio.h>

char literal[] =
"line 1\n"
"line 2";

char lit2[] =
"	lit2  1"
	"lit2 2";

int main()
{
	printf("[%s]\n", literal);
	printf("[%s]\n", lit2);
	return 0;
}
