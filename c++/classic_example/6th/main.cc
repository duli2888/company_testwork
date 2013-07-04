//私有继承
// Access levels

#include <iostream>

using namespace std;

class Base
{
	private:
		int priv;
	protected:
		int prot;
		int get_priv() { return priv; }
	public:
		int publ;
		Base();
		Base(int a, int b, int c) : priv(a), prot(b), publ(c) { }
		int get_prot() { return prot; }
		int get_publ() { return publ; }
};

class Derived1 : private Base			// private inheritance
{
	public:
		Derived1 (int a, int b, int c) : Base(a, b, c) { }
		int get1_priv() { return get_priv(); }
		// priv not accessible directly
		int get1_prot() { return prot; }
		int get1_publ() { return publ; }
};

class Leaf1 : public Derived1
{
	public:
		Leaf1(int a, int b, int c) : Derived1(a, b, c) { }
		void print( )
		{
			cout << "Leaf1 members: " << get1_priv() << " "
				//                    << get_priv( )			// not accessible
				<< get1_prot() << " "
				//                    << get_prot( )			// not accessible
				//                    << publ					// not accessible
				<< get1_publ() << endl;
		}		// data members not accessible.  get_functions in Base not accessible
};

class Derived2 : protected Base			// protected inheritance
{
	public:
		Derived2 (int a, int b, int c) : Base(a, b, c) { }
};

class Leaf2 : public Derived2
{
	public:
		Leaf2(int a, int b, int c) : Derived2(a, b, c) { }
		void print( )
		{
			cout << "Leaf2 members: " << get_priv( ) << " "
				//                        << priv                 // not accessible
				<< prot << " "
				<< publ << endl;
		}  // public and protected data members accessible.  get_functions in Base accessible. 
};

class Derived3 : public Base  // public inheritance
{
	public:
		Derived3 (int a, int b, int c) : Base(a, b, c) { }
};

class Leaf3 : public Derived3
{
	public:
		Leaf3(int a, int b, int c) : Derived3(a, b, c) { }
		void print( )
		{
			cout << "Leaf3 members: " << get_priv( ) << " " 
				<< prot << " "
				<< publ << endl;
		}  // public and protected data members accessible.  get_ functions in Base accessible
};

int main( )
{
	Derived1 d1(1, 2, 3);
	Derived2 d2(4, 5, 6);
	Derived3 d3(7, 8, 9);
	//		cout << d1.publ;                // not accessible
	//      cout << d1.get_priv( );         // not accessible
	//      cout << d1.get_publ( );         // not accessible
	
	//      cout << d2.publ;                // not accessible
	//      cout << d2.get_priv( );         // not accessible
	//      cout << d2.get_publ( );         // not accessible
	
	cout << d3.publ << endl;                // OK
	cout << d3.get_prot() << endl;          // OK
	//		cout << d3.get_priv();          // not accessible

	cout << "-----------------" << endl;
	Leaf1 lf1(1, 2, 3);
	Leaf2 lf2(4, 5, 6);
	Leaf3 lf3(7, 8, 9);
	//         cout << lf1.publ << endl;                    // not accessible
	//         cout << lf2.publ << endl;					// not accessible
	cout << lf3.publ << endl;              // OK

	Base b1(1, 2, 3);
	// cout << b1.get_priv();
	cout << "-----------------" << endl;
	//		cout << b1.priv << endl;		// 不能访问,自己都不能访问
	//		cout << b1.prot << endl;		// 不能访问,自己都不能访问
			cout << b1.publ << endl;		// 不能访问,自己都不能访问
	cout << b1.get_prot() << endl;
	cout << b1.get_publ() << endl;
	return 0;
}


/* c++的继承关系的保护是在编译期间保证的,在生成代码的时候做一些检查，就像语法错误在编译的时候被检查出来一样
 * private:
 *		一、对象直接访问自己的类
 *			(1)成员变量：不能访问
 *			(2)成员函数：不能访问
 *		二、继承类的对象(public基类)
 *			(1)成员变量：不能访问
 *			(2)成员函数：不能访问
 *		三、类内成员函数访问成员变量:可以访问
 *		四、继承类的成员函数访问基类的成员变量(public基类)：不能访问
 *
 * protected:
 *		一、对象直接访问自己的类
 *			(1)成员变量：不能访问
 *			(2)成员函数：不能访问
 *		二、继承类的对象(public基类)
 *			(1)成员变量：不能访问
 *			(2)成员函数：不能访问
 *		三、类内成员函数访问成员变量:可以访问
 *		四、继承类的成员函数访问基类的成员变量(public基类)：可以访问
 *		子类的成员函数是可以访问父类的protected成员变量的，但是对象不能直接访问和自己对应类的成员函数和成员对象
 *
 * public:
 *		略:则各种情况都可以
 *
 * friend:
 *		是指特定指出哪些类或者模块可以看见本类的私有(private以及protected)成员
 *
 */
