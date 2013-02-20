#include <iostream>
#include <math.h>

using namespace std;

class Point
{
	private:
		double x;
		double y;
	public:
		Point(double i, double j) : x(i), y(j) {} 
		void print() const
		{ cout << "(" << x << ", " << y << ")"; }
};
/*
 *	const对象只能访问const成员函数,而非const对象可以访问任意的成员函数,包括const成员函数.
 *	const对象只能调用const成员函数。   
 *	const对象的值不能被修改，在const成员函数中修改const对象数据成员的值是语法错误   
 *	const函数中调用非const成员函数是语法错误  
 *	const对象的成员是不可修改的,然而const对象通过指针维护的对象却是可以修改的.
 *	const成员函数不可以修改对象的数据,不管对象是否具有const性质.它在编译时,以是否修改成员数据为依据,进行检查.
 */

class Figure
{
	private:
		Point center;
	public:
		Figure(double i = 0, double j = 0) : center(i, j) {}         

		Point & location() { return center; }                // return an lvalue
		void move(Point p) {	center = p;		draw();	}
		virtual void draw() = 0;							 // 纯虚函数的声明,需函数的声明只要加virtual关键字则可以
		virtual void rotate(double) = 0;					 // 纯虚函数的声明
};

class Circle : public Figure
{
	private:
		double radius;
	public:
		Circle(double i = 0, double j = 0, double r = 0) : Figure(i, j), radius(r) {}
		void draw() {
			cout << "[Circle]--draw():A circle with center ";
			location().print();
			cout << " and radius " << radius << endl;
 		}
		void rotate(double) {	cout << "no effect.\n";	}        // must be defined
};


class Square : public Figure
{
	private:
		double side;        // length of the side
		double angle;        // the angle between a side and the x-axis
	public:
		Square(double i = 0, double j = 0, double d = 0, double a = 0): Figure(i, j), side(d), angle(a) { }
		void draw() {
			cout << "[Square]--draw():A square with center ";
			location( ).print( );
			cout << " side length " << side << ".\n"  
				<< "[Square]--draw():The angle between one side and the X-axis is " << angle << endl;
 		}
		void rotate(double a)
		{
			angle += a;
			cout << "[Square]--rotate():The angle between one side and the X-axis is " << angle << endl;
 		}
		void vertices( )
		{
			cout << "[Square]--vertices():The vertices of the square are:\n";
			// calculate coordinates of the vertices of the square
 		}
};

int main( )
{
	Circle c(1, 2, 3);
	Square s(4, 5, 6);
	Figure *f = &c, &g = s;		// 子类向父类转换, 实际仍然指向子类对象
	f->draw();
	f->move(Point(2, 2));

	g.draw();
	g.rotate(1);

	s.vertices();

	// Cannot use g here since vertices( ) is not a member of Figure.
	return 0;
}
/*
 *	在基类当中的需函数实际调用的是它的派生类中的函数
 */



