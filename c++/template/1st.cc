#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

// 类模板 - 声明
template <class T>
class A
{
	public:
		A();
		A(T _a, T _b);
		T sum();
	private:
		T a;
		T b;
};

template <class T1, class T2>
struct tSample
{
	T1 key;
	T2 value;
};

// 类模板 - 实现
template <class T> A<T>::A()			// 类模板的成员函数是函数模板
{
	a = 0; b = 0;
}

template <class T> A<T>::A(T _a, T _b)	// 类模板的成员函数是函数模板
{
	a = _a; b = _b;
}

template <class T> T A<T>::sum()		// 类模板的成员函数是函数模板，注意中间的那个Ｔ代表sum()的返回值
{
	return a + b;
}

int main()
{
	A<int> obj1(3, 4);
	A<double> obj2(1.11, 2.22);

	cout << "obj1.sum(3, 4)=" << obj1.sum() << endl << "obj2.sum(1.11, 2.22)=" << obj2.sum() << endl;

	tSample<unsigned char, int> array1[] = {
		{255, 0xffffffff},
		{256, 0xfffffff},
	};

	tSample<unsigned short, int> array2[] = {
		{255, 0xffffffff},
		{256, 0xfffffff},
	};
	
	int idx = 0; 

	for (idx = 0; idx < sizeof(array1) / sizeof(array1[0]); idx++) {
		printf("template struct array1, index: %d, key: %d, value: 0x%x \n", idx, array1[idx].key, array1[idx].value);
	}

	for (idx = 0; idx < sizeof(array2) / sizeof(array2[0]); idx++) {
		printf("template struct array2, index: %d, key: %d, value: 0x%x \n", idx, array2[idx].key, array2[idx].value);
	}
}
/*
 * 声明和使用类模板： 如果在类模板外定义成员函数，应写成类模板形式： 
 * template<class 虚拟类型参数>
 * 函数类型 类模板名<虚拟类型参数>∷成员函数名(函数形参表列) {…}
 *
 */
