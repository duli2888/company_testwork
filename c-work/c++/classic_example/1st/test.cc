#include<iostream>

using namespace std;

class Base
{
	private:
		int b_number;
	public:
		Base(){	}

		Base(int i):b_number(i){ }

		int get_number(){ 	return b_number;  }

		void print(){  cout << b_number << endl;	}
};

class Derived:public Base
{
	private:
			int d_number;
	public:
			Derived(int i, int j):Base(i),d_number(j){  };

			void print()
			{
				cout << get_number() << "";
				cout << d_number << endl;
			}
};

int main()
{
	Base a(2);
	Derived b(3,4);
	cout << "a is";
	a.print();	// print() in Base; b_number = 2;
	cout << "b is";
	b.print();	//print() in Derived; b_number = 3; d_number = 4;
	cout << "base part of b is";
	b.Base::print();	//print() in Base; b_number = 3;

	return 0;
}
