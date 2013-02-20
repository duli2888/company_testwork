#include <map>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <time.h>
#define ARP_LENGTH 32
using namespace std;
         
typedef union 
{
	unsigned char a[32];

}student_name;


typedef struct
{
	int score;
	time_t access_time;
}score;


class student_score{
	private:
		map<string, score> st_score;
	public:
		/* This function is just for test */
		void fill_some_records(string *name, score *record ,int len)
		{
			int i;
			for (i = 0; i < len; i++) {
				st_score.insert(make_pair(name[i], *(record + i)));
			}
		}

		int find_record(string name)
		{

			map<string, score>::iterator it;
			if((it = st_score.find(name)) == st_score.end()) 
				return -1; // do't find
			else  
				return 0; // find
		}

		void add_student_record(string name, score record)
		{
			st_score.insert(make_pair(name, record));
		}

		int delete_score_record(string name)
		{

			map<string, score>::iterator it;
			if((it = st_score.find(name)) == st_score.end()) {
				return -1; // delete failed
			} else {
				st_score.erase((*it).first);
			}
			return 0;	// delete success
		}

		int delete_old_record()
		{
			time_t t_min;
			map<string, score>::iterator it, it_tmp;
			it = st_score.begin();
			t_min = (*it).second.access_time;

			for ( ; it != st_score.end(); it++ ) {
				if ((*it).second.access_time < t_min) {
					t_min = (*it).second.access_time;
					it_tmp = it;
				}
			}

			st_score.erase((*it_tmp).first);
		}

		int display_student_score()
		{
			map<string, score>::iterator it;
			for ( it=st_score.begin() ; it != st_score.end(); it++ )
				cout << (*it).first << " => " << (*it).second.score << "---------->"<< (*it).second.access_time <<endl;	
		}

};

int main()
{
	int i;
	
	student_score *student;
   	student = new student_score;
	string name[10]= {"zhangli","li","fun","wang","tan", "good", "hello", "china", "novel", "map"};
	score s[10];
	for (i = 0; i < 10; i++) {
		s[i].score = 90 + i;
		s[i].access_time = time(NULL);
	
	}

	student->fill_some_records(name, s, 10);
	student->display_student_score();

#if 1
	if (!student->find_record("china")){
		cout << "\n [we find it]" << endl;
	} else {
		cout << "\n [can't find it]" << endl;
	}
#endif


#if 1	
	score s_tmp;
	s_tmp.score = 66;
	s_tmp.access_time = 555;
	cout << "\n[add_student_record]" << endl;
	student->add_student_record("duli", s_tmp);
	student->display_student_score();
#endif

#if 1
	cout << "\n[delete_old_record]" << endl;
	student->delete_old_record();
	student->display_student_score();
#endif

	cout << "\n[delete_score_record]" << endl;
	student->delete_score_record("hello");
	student->display_student_score();


	return 0;
}
