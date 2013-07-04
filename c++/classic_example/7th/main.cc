//多级继承,符号重载
// Point-Circle-Cylinder

#include <iostream>
using namespace std;

class Point
{
	friend ostream & operator << (ostream &, Point &);
	protected:					// accessed by derived class
		double x;
		double y;
	public:
		Point (double xval = 0, double yval = 0 ) { x = xval; y = yval; };  
};

ostream & operator << (ostream & os, Point & apoint)
{
	cout << "Point:X:Y: " << apoint.x << "," << apoint.y << endl;
	return os;
}

class Circle : public Point
{
	friend ostream & operator << (ostream &, Circle &);
	public:
		Circle (double r = 0, double xval = 0, double yval = 0) : Point(xval, yval), radius(r)
		{ 
			//radius  =  r;
		}
		double area()
		{ 
			return (3.14159 * radius * radius);
		}
	protected:
		double radius;
};

//note casting circle to point
ostream & operator << (ostream & os, Circle & aCircle)
{
	cout << "Circle:radius: " << aCircle.radius << endl;;
	os << aCircle.x << endl;	// 在这里，cout << aCircle.x << endl; 效果相同
	os << aCircle.y << endl;	// 在这里，cout << aCircle.y << endl; 效果相同
	
	return os;
}

// THE CYLINDER CLASS
class Cylinder : public Circle
{
	friend ostream & operator << (ostream & ,Cylinder &);
	public:
	Cylinder (double hv = 0, double rv = 0, double xv = 0,double yv = 0 ) : Circle(xv, yv, rv)
	{
		height = hv;
	}        
	double area();
	protected:			// may have derived classes
	double  height;
};

double Cylinder :: area () // Note that cylinder area uses Circle area
{
	return  2.0 * Circle::area() + 2.0 * 3.14159 * radius * height;
}

ostream & operator << (ostream & os, Cylinder & acylinder)
{
	cout << "Cylinder dimensions:";
	cout << "  x: " << acylinder.x;
	cout << "  y: " << acylinder.y;
	cout << "  radius: " << acylinder.radius;
	cout << "  height: " << acylinder.height << endl;
	return os;
}

int main(void)
{
	Point p(1, 2);
	Circle c(3, 4, 5);
	Cylinder cyl(10, 11, 12, 13);
	cout << p;
	cout << "------------------------------" << endl;
	cout << c;
	cout << "------------------------------" << endl;
	cout << "Area of cirle:" << c.area() << endl;
	cout << "------------------------------" << endl;
	cout << cyl;
	cout << "Area of cylinder:" << cyl.area() << endl;
	cout << "Area of cylinder base is " << cyl.Circle::area() << endl;
	return 0;
}
