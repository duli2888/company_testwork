#include <iostream>

using namespace std;


class A
{
	public:
			A(){cout << "A Constructor!" << endl;}
			~A(){cout << "A Destructor!" << endl;}
};

class B
{
	public:
			B(){cout << "B Constructor!" << endl;}
			~B(){cout << "B Destructor!" << endl;}
};

class C:public A, public B   //基类的构造函数按照继承时声明的先后顺序从前到后执行，最后执行自己的构造函数；析构函数则按照相反的顺序执行。
{
	public:
			C(){cout << "C Constructor!" << endl;}
			~C(){cout << "C Destructor!" << endl;}
};

int main()
{
		C c;
		return 0;
}
