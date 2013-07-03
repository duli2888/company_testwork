#include <iostream>
#include<stdlib.h>

using namespace std;    

double fuc(double x, double y)								// 定义函数
{
	if(y == 0) {
		throw y;											// 除数为0，抛出异常
		// throw 1.11111111;								// 此返回的值交给了catch的参数表,在catch中可以拿到次异常值
	}
	return x / y;											// 否则返回两个数的商
}

int main()
{
	double res;
	try {													// 定义异常
		res = fuc(6,3);
		cout << "The result of 6/3 is : " << res << endl;
		res = fuc(1,2);
		cout << "The result of 1/2 is : " << res << endl;
		res = fuc(4,0);										// 出现异常
		cout << "The result of 4/0 is : " << res << endl;
		res = fuc(1,3);
		cout << "The result of 1/3 is : " << res << endl;
	} catch(double z) {										// 捕获并处理异常
		cerr << "error of dividing " << z << endl;
		exit(1);											// 异常退出程序
	}

	return 0;
}

//  catch 的数据类型需要与throw出来的数据类型相匹配的。
