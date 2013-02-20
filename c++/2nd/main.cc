#include <stdio.h>
#include "student.h"
#include <iostream>

#if 0

class Student
{

	public:
		int score;
		Student(int val)
		{
			no = val;
			score= 60 + no;
		}
	protected:

	private:
		int no;
		int age;
	
};	

#endif

using namespace std;

int myprintf(int);

int main()
{
	int student_no = 20;
	myprintf(student_no);
}


int myprintf(int student_no)
{

		
	Student good_one(student_no,30);
	std::cout << good_one.score << "\n";
	printf("Hello,This is printf for C++\n");

	return 0;

}
