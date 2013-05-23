#include <stdio.h>
static int g_variable = 30;

void test_function(void)
{
	printf("[test_function] g_variable = %d\n", g_variable);
}
