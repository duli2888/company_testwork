#ifndef __STUDENT__H
#define __STUDENT__H

class Student
{

	public:
		int score;
		Student(int val, int basic)
		{
			no = val;
			score= basic + no;
		}
	protected:

	private:
		int no;
		int age;
	
};	



#endif
