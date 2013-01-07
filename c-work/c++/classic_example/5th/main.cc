#include <iostream>
#include <string>

class C
{
private:
        std::string c;
public:
        C(){ this->c=c; std::cout<<"构造无参数函数"<<std::endl;};
        C(std::string c);
		~C();
		void showC();
};

C::C(std::string c)
{
	this->c=c;
	std::cout<<"[C]"<< this <<std::endl;
    std::cout<<"构造对象c"<<std::endl;
}

C::~C()
{
    std::cout<<"清理对象c"<<std::endl;
}
void C::showC()
{
    std::cout<<"c="<<this->c<<std::endl;
}

///////////////////////////////////////////////

class D:private C
{
	private:
		std::string d;
	public:
		D(std::string d,std::string c);
		~D();
		void showD();
};

D::D(std::string d,std::string c):C(c) //参数传递给基类,也可以不写C(c)，那么就调用了C类中无参构造函数
{
	this->d=d;
	std::cout<<"[D]"<< this <<std::endl;
	std::cout<<"构造对象d"<<std::endl;
}

D::~D()
{
	std::cout<<"清理对象d"<<std::endl;
}

void D::showD()
{
	showC();
	std::cout<<"d="<<this->d<<std::endl;
}

///////////////////////////////////////////////

int main()
{
	{
		D d("ddd","ccc");
		d.showD();
	}

	return 0;
}

