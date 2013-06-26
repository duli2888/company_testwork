//类的兼容性规则

#include <iostream>
using namespace std;

class Base
{ 
	public:  
		void func() 
		{cout << "Base class function./n";} 
};

class Derived : public Base
{ 
	public:  
		void func() 
		{cout << "Derived class function./n";}
};

void foo(Base b)
{ 
	b.func(); 
}

int main()
{
	Derived d;
	Base b;
	Base * p = &d;
	Base& br = d;
	b = d;
	b.func();
	d.func();
	p -> func();
	foo(d);
	br.func();
	return 0;
}

