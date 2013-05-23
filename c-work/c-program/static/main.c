#include <stdio.h>

static int g_variable;

int main()
{
	printf("[1][main] g_variable = %d\n", g_variable);
	test_function();
	printf("[2][main] g_variable = %d\n", g_variable);

	return 0;
}

/* 
 * 注：调用函数test_function()在另一个文件，则此函数则会使用另一个文件当中
 *		的全局变量，而不是此文件当中的。
 *
 */
