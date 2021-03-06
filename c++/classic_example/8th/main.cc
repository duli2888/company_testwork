//protected 访问控制属性在继承的意义
//Example of treating derived class object as base class objects. Point------Circle

#include <iostream>
using namespace std;

class Point
{ 
	friend ostream & operator << (ostream &, Point &);
	public:
		Point (double xval = 0, double yval = 0 ) { x = xval; y = yval; }
	public:
		void print()
		{
			cout << " Point:X:Y: " << x << "," << y << "\n";
		}
	protected:				// accessed by derived class
		double  x;
		double  y;
};

ostream & operator << (ostream & os, Point & apoint)
{
	cout << " Point:X:Y: " << apoint.x << "," << apoint.y << "\n";
	return os;  
}

class Circle : public Point
{
	friend ostream & operator << (ostream &, Circle &);
	public:
		Circle (double r = 0, double xval = 0, double yval = 0) : Point(xval, yval)
		{ radius = r; }
		void print()
		{
			cout << "Circle:radius:" << radius << endl;
			cout << " Point:X:Y: " << x << "," << y << "\n";
		}
		double area()
		{ 
			return (3.14159 * radius * radius);
		}
	protected:
		double radius;
};

ostream & operator << (ostream & os, Circle & aCircle)
{
	cout << "Circle:radius:" << aCircle.radius;
	cout << (Point &)aCircle << "\n";           
	return os;      
}

int main (void )
{
	Point p(2, 3);			cout << "Point P  =  " << p;
	Point pp(0, 0);			cout << "Point PP =  " << pp;
	Circle c(7, 6, 5);		cout << "Circle c =  " << c;        // radius = 7
	pp = p;					cout << "Point PP =  " << pp;		// built in assign =

	// a circle is a member of the point class so assign a circle to a point.
	pp = c;							// legal; also assignment O.K.
	cout << "Point PP =  " << pp;
	cout << "------------------------------" << endl;

	pp = (Point)c;					// but better  use the cast
	cout << "Point PP =  " << pp;	// note we get only the point part of the Circle
	cout << "------------------------------" << endl;
	// c = (Circle) pp;				// illegal Cannot convert 'class Point' to 'class Circle'
	// c = pp;						// illegal assignment not defined
	Point * ppp;
	ppp = &c;
	ppp->print();					// call base class print
	((Circle *)ppp)->print();
	Point& r = c;
	r.print();
	((Circle &)r).print();

	return 0;
}
