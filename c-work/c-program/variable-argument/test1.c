#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MAX_LEN 10  //参数最大个数
#define FIND

char char_offset(int a, int b, char ch);

void *p_function(void *p, ...);

int ftest(int count,char *msg, ... )  
{  
	va_list argp;				    /* 定义保存函数参数的结构 */  
	int argno = 0;					/* 纪录参数个数 */  
	char *para[MAX_LEN];			/* 存放取出的字符串参数 */  

	/* argp指向传入的第一个可选参数，	msg是最后一个确定的参数 */  
	va_start( argp, msg );  

	int i;
	for (i = 0; i < count; i++)
	{  
		para = va_arg( argp, char *);                 /*	取出当前的参数，类型为char *. */  
		printf("Parameter #%d is: %s\n", argno, para);  
		argno++;  
	}  
	va_end( argp );                                   /* 将argp置为NULL */  
	return 0;  
}

void main(void)  
{  
	ftest(3, 6, add, 1, 2, b);
}  

char add(int a, int b, char ch)
{
	return (ch + a + b);
}

/*
 * va_start , va_arg, va_end 三个是宏
 *
 */
