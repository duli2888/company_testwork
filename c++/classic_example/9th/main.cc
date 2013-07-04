//类的兼容性规则

#include <iostream>
using namespace std;

class Base
{ 
	public:  
		void func() 
		{ cout << "Base class function.\n"; } 
};

class Derived : public Base
{ 
	public:  
		void func() 
		{ cout << "Derived class function.\n"; }
};

void foo(Base b)
{ 
	b.func(); 
}

int main()
{
	Base b;
	Derived d;

	Base * p = &d;
	Base & br = d;

	b = d;

	cout << "--------b.func----------" << endl; 
	b.func();
	cout << "--------d.func----------" << endl; 
	d.func();

	cout << "--------p->func----------" << endl; 
	p->func();
	cout << "--------foo(d)----------" << endl; 
	foo(d);
	cout << "--------br.func----------" << endl; 
	br.func();

	return 0;
}


/*
 * 1.引用与指针的区别
 * 2.类之间的类型的转换
 */
