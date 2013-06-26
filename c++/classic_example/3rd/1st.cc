/* 多继承 */
#include <iostream>

using namespace std;

class A
{
	private:
		int a;
	public:
		A(int i):a(i){}

		//virtual void print() { cout << "A:" << endl; cout << a << endl; }
		void print() { cout << "A:" << endl; cout << a << endl; }

		int get_a(){ return a; }
};

class B
{
	private:
		int b;
	public:
		B(int j):b(j){}

		virtual void print(){ cout << "B:" << endl; cout << b << endl; }
		//void print(){ cout << "B:" << endl; cout << b << endl; }

		int get_b(){ return b; }
};

class C:public B,public A		// 继承父类的顺序没有关系
{
		int c;
	public:
		C(int i,int j,int k):A(i),B(j),c(k){}

		void print()	{ cout << "C:" << endl; A::print(); B::print(); }	
		/* 此函数可以注释掉，则不会影响ap->print()和bp->print()函数的执行
		 * 如果是在主函数中调用ap->print();如果基类的print是虚函数，则会调用本函数，如果基类是实函数，
		 * 则会调用基类的print函数,则一个类中父类和子类有相同的函数，会优先调用哪个函数的问题。
		 */
		
		void get_ab()	{ cout << get_a() << " " << get_b() << endl; }
		//use get_a() and get_b() without scope resolution
};

int main()
{
	C x(1,2,3);
	A* ap = &x;
	B* bp = &x;
	cout << "-------[ap]---------" << endl;
	ap->print();							// use C::print(); 
	cout << "-------[bp]---------" << endl;
	bp->print();							// use B::print();
	cout << "-------[x]---------" << endl;
	//x.print();								// use C::print();
	// bp->A::print();						// as if x is inherited from B only,
											// cannot access A::print();
	cout << "-------[x.A::print]---------" << endl;
	x.A::print();							// use A::print();
	cout << "-------[x.get_ab]---------" << endl;
	x.get_ab();
	return 0;
}
