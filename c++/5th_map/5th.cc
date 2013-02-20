#include <map>
#include <iostream>
#include <stdint.h>
#define ARP_LENGTH 32
using namespace std;

         

class student_score{
	private:
		map<int, int> st_score;
	public:
		/* This function is just for test */
		void fill_some_records(int *id, int *score ,int len)
		{
			int i;
			for (i = 0; i < len; i++) {
				st_score.insert(make_pair(*(id + i), *(score + i)));
			}
		}

		int find_record(int id)
		{
			if(st_score.find(id) == st_score.end()) 
				return -1; // do't find
			else  
				return 0; // find
		}

		void add_student_record(int id, int score)
		{
			st_score.insert(make_pair(id, score));
		}

		int delete_score_record(int id)
		{
			if(!find_record(id)) {
			//	arp_table.erase(it);
			} else {
				return -1; // delete failed
			}
			return 0;	// delete success
		}

		int delete_old_record()
		{
			st_score.erase(st_score.begin());
		}

		int display_student_score()
		{
			map<int,int>::iterator it;
			for ( it=st_score.begin() ; it != st_score.end(); it++ )
				cout << (*it).first << " => " << (*it).second << endl;	
		}

};

int main()
{
	int i;
	
	student_score *student;
   	student = new student_score;
	int id[10];
	int score[10];
	for (i = 0; i < 10; i++) {
		id[i] = i; 
		score[i] = 90 + i;
	
	}

	student->fill_some_records(id, score, 10);
	student->display_student_score();

	if (!student->find_record(8)){
		cout << "we find it" << endl;
	} else {
		cout << "can't find it" << endl;
	}

#if 0	
	cout << "\n[add_student_record]" << endl;
	student->add_student_record(10, 100);
	student->display_student_score();
#endif

#if 0
	cout << "\n[delete_old_record]" << endl;
	student->delete_old_record();
	student->display_student_score();
#endif

	return 0;
}
