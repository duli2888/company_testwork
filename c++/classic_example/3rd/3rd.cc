#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

class A
{
	public:
		char str[20];
		void f() { cout << "class A" << endl; }
		void fff() { cout << "class A's str " << str << endl; } 
		void add() { cout << "class A address: " << this << endl; }
};

class B:public A
{
	public:
		int i;
		char sb[20];
		B() { cout << "class B constructor is run." << endl; }
		~B() { cout << "class B destructor is run." << endl; }
		void f() { cout << "class B" << endl; }
		void ff() { cout << "class B " << i << str << sb << endl; }
		void add() { cout << "class B address: " << this << endl; }
};

int main(int argc, char *argv[])
{
	A b;
	A *pa = &b;
	cout << "-----[pa->add]---------" << endl;
	pa->add();
	strcpy(pa->str, " class A's str.");

	B *pb = (B *)pa;
	/* 这个写法正确,不过很危险,因为pa到底是指向B还是A是不确定的.如果能明确的知道
	 * pa是指向B类对象的,就如现在写的这个,那么没问题,如果pa指向A类对象,就会存在
	 * 风险.改变指针的类型，并不会影响内存分配既不会调用构造函数
	 */

	cout << "-----[pb->add]---------" << endl;
	pb->add();
	/* 类型强制转化后,指向的地址相同. 但会按转化类型访问数据成员.
	 * 成员函数属于类而不属于对象.各对象共用类的成员函数.强制转换成B类型后,可
	 * 调用类的成员函数。编译pb->add();后的代码只是调用add函数的时候传入了pb的对象指针this
	 */


	// 由于pa指向的是父类对象的地址,这个指针被强制转换为派生类指针后,会出现内存越界访问的情况,是不安全的.
	pb->i = 111;		// 此i指向实际内存的哪??
	//strcpy(pb->sb, "class B's sb.");
	strcpy(pb->str, "class A's str.");
	
	cout << "-----[pb->f]---------" << endl;
	pb->f();
	cout << "-----[pb->ff]---------" << endl;
	pb->ff();
	cout << "-----[pb->fff]---------" << endl;
	pb->fff();
	// system("PAUSE");
	return 0;
}
// pb->f()调用的具体函数，视指针的原始类型而定，而不是视指针实际所指的对象的类型而定.如果是虚函数，则视实际所指的对象的类型而定
// 所以类型强制转换后，函数的指向是转换后类型的函数。
//
// 对于指针类型转换后，函数的调用则不是很明白.
