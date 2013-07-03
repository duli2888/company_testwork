#include <iostream>

class A
{
	private:
		int m_a;
	public:
		A() : m_a(0) {}
		int getA() const
		{
			return m_a; // 同return this->m_a;。
		}
		int GetA()
		{
			return m_a;
		}
		int setA(int a) const
		{
			// m_a = a;	// 这里产生编译错误。如果把前面的成员定义int m_a，改为mutable int m_a，就可以编译通过。
		}
		int SetA(int a)
		{
			m_a = a;	// 同this->m_a = a;
		}
};

int main()
{
	A a1;
	const A a2;
	int t;
	t = a1.getA();
	t = a1.GetA();
	t = a2.getA();
	// t = a2.GetA();		// a2是const对象，调用非const成员函数产生编译错误。

	return 0;
}

/*
 * 非静态成员函数后面加const（加到非成员函数或静态成员后面会产生编译错误），表示成员函数隐含传入的this指针为const指针，
 * 决定了在该成员函数中，任意修改它所在的类的成员的操作都是不允许的（因为隐含了对this指针的const引用）；唯一的例外是对于
 * mutable修饰的成员。加了const的成员函数可以被非const对象和const对象调用，但不加const的成员函数只能被非const对象调用。
 * 一般对于不需修改操作的成员函数尽量声明为const成员函数，以防止产生const对象无法调用该成员函数的问题，同时保持逻辑的清晰。
 */
