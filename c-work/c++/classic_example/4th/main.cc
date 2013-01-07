#include <iostream>

class A
{
	private:
		int a;
	public:
		A(int i):a(i){}
		virtual void print() {count << a << endl;}
		int get_a(){return a;}
};

class B
{
	private:
		int b;
	public:
		B(int j):b(j){}
		void print(){count << b << endl;}
		int get_b(){return b;}
};

class C:public A,public B
{
	int c;
	public:
	C(int i,int j,int k):A(i),B(j),c(k){}
	void print()	A{A::print(); B::print();}
	//use print() with scope resolution
	void get_ab()	{cout << get_a() << " " << get_b() << endl;}
	//use get_a() and get_b() without scope resolution
};

int main()
{
	Cx(5,3,10);


}
