#include <stdio.h>
extern "C" {
	#include "test.h"
}

int main()
{
	func();

	return 0;
}

/* 注：
 *		在C++中引用C语言中的函数和变量，在包含C语言头文件（假设为cExample.h ）时，需进行下列处理：
 *		extern "C" {
 *			#include "头文件.h"
 *		}
 *
 *		Ｃ语言中不支持extern "C" 声明，在.c文件或者.h文件中包含了 extern "C" 时，会出现编译语法错误
 *			error:expected indentifier or '(' before string constant.
 *
 *
 */

