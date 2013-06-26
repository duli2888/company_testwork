#include <iostream>
#include <string>

using namespace std;

class C
{
	private:
		string c;
	public:
		C(){ this->c = c; cout << "构造无参数函数" <<endl; };
		C(string c);
		~C();
		void showC();
};

C::C(string c)
{
	//this->c = c;
	cout << "C:this-->point " << this << "  c = "<< c << endl;
	cout << "构造对象c " << endl;
}

C::~C()
{
	cout << "清理对象c " << endl;
}

void C::showC()
{
	cout << "showC() c=" << this->c << endl;
}

///////////////////////////////////////////////
class D:private C
{
	private:
		string d;
	public:
		D(string d,string c);
		~D();
		void showD();
};

D::D(string d,string c):C(c) //参数传递给基类,也可以不写C(c)，那么就调用了C类中无参构造函数,先会去执行C类中的构造函数
{
	//this->d = d;
	cout << "D:this-->point " << this << endl;
	cout << "构造对象d " << endl;
}

D::~D()
{
	cout << "清理对象d " << endl;
}

void D::showD()
{
	showC();
	cout << "showD() d=" << this->d << endl;
}

///////////////////////////////////////////////

int main()
{
	D d("ddd","ccc");
	//d.showD();

	return 0;
}
