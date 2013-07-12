// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

int main () {
	ofstream myfile;
	myfile.open ("example.txt");
	if (myfile.is_open()) { /* ok, proceed with output */
		cout << "example.txt open success" << endl;
		myfile << "Writing this to a file.--- Hello World" << endl;
		myfile.close();
	}
	return 0;
}
