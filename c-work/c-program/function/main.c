#include <stdio.h>

int show_function(const char *file, const char *func, int line, ...)
{
	printf("%s---%s---%d\n", file, func, line);
	return 0;
}

int main()
{
	show_function( __FILE__, __func__, __LINE__);

	return 0;
}
