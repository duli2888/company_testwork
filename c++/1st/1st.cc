// example on insertion
#include <iostream>     // std::cout, std::right, std::endl
#include <iomanip>      // std::setw

using namespace std;

int main () {
	int val = 65;

	cout << right;       // right-adjusted (manipulator)
	cout << setw(20);    // set width (extended manipulator)

	cout << val;
	//cout << val << endl; // multiple insertions

	return 0;
}

