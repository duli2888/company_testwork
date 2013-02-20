#include <iostream>
#include <string>

using namespace std;

class Thing
{
	public:
		virtual void what_Am_I() { cout << "I am a Thing.\n"; }
		virtual ~Thing() {  cout << "Thing destructor" << endl; } // 加不加virtual 区别会不同

};

class Animal:public Thing
{
	public:
		virtual void what_Am_I() { cout << "I am an Animal.\n"; }
		virtual ~Animal() { cout << "Animal destructor" << endl; }  // 加不加virtual 区别会不同

};

int main()
{
	Thing *t = new Thing;				// new 关键字申请空间，将申请到的指针返回；不加new的情况；
	Animal *a = new Animal;
	Thing *array[2];					// 定义的是指针数组 数组指针定义：Thing (*array)[2]
	// Animal *array[2];				// 基类到派生类转换无效,派生类可以转换为基类

	array[0] = t;	
	array[1] = a;

	for(int i = 0; i < 2; i++) array[i]->what_Am_I();

	delete array[0];
	delete array[1];					// 由于array[1]是派生类,所以析构派生类的时候先析构基类

	return 0;

}
