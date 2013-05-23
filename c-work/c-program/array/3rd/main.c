#include <stdio.h>
#include <string.h>

int main()
{
#if 1
	char str[12];
	(char *)str = "hello world";	
	//strcpy(str, "hello world");
	//char [] str = "hello world";
	//str = "hello world";
	/* char []和char *指针是完全不同的类型，"hello world"是字符串常量，
	 * 是完全和char []类型是不同的.所以报告错误类型不同, char *则不同，和"hello world"类型是相同的.
	 * char [] str是属于栈区，
	 */
#else
	char *str;
	str = "hello world";
#endif
	printf("%s\n", str);

	return 0;
}


/*
 * 注："hello world"是存储在全局区, str[12]的str的指针值和str *的指针值是放在栈区，
 */
