// 共同基类的多继承
#include <iostream>

using namespace std;

class R
{
		int r;
	public:
		R(int anInt) { r = anInt;}
		void print(){ cout << "r = " << r << endl; }
};

class A : public R
{
		int a;
	public:
		A(int m, int n) : R(n) { a = m; }
};

class B : public R
{
		int b;
	public:
		B(int m, int n) : R(n) { b = m; }
};

class C : public A, public B
{
	int c;
	public:
	C (int m, int n, int w) : A(n, w), B(n, w) { c = m; }
};

int main( )
{    
	R rr(10);
	A aa(20, 30);
	B bb(40, 50);
	C cc(5, 7, 9);
	rr.print();
	aa.print();						//inherits R print  
	bb.print();						//inherits R print
	cc.A::print();					// 需要指明是A还是B的print	
	cc.B::print();					// 需要指明是A还是B的print	
	//cc.print();					//would give error
	return 0;
}
