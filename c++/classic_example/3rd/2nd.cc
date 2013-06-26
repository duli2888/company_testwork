#include <iostream>
using namespace std;

class A
{
	protected:
		int m_a;
	public:
		A(int a) { m_a = a; cout << "A 构造函数" << m_a << endl;}
};

class B:public A
{
	public:
		B(int a):A(a) { cout << "B 构造函数" << m_a << endl; }
};

class C:public A
{
	public:
		C(int a):A(a) { cout << "C 构造函数" << m_a << endl; }
};

class D:private B,private C
{
	public:
		inline void Display();
		D(int a,int b):B(a),C(b) { cout << "D构造函数 " << endl; }
};

inline void D::Display()
{
	cout << "B::m_a=" <<B::m_a << endl;
	cout << "C::m_a=" << C::m_a << endl;		
	// 以上B::m_a的值和C::m_a的值是不同的，可以确定D类继承B类和C类的时候，是同时继承了A类，所以D类有两个m_a，一个是B->A->m_a,另一个是C->A->m_a.
	// cout << "A::m_a=" << A::m_a << endl;		// 这行是错误的,由于A不能确定是从B继承还是从C继承
}

int main()
{
	D d(10,20);
	cout << "--------------------" << endl;
	d.Display();
	return 0;
}
