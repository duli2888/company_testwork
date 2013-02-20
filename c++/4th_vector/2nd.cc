#include <vector>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string>
using namespace std;

         
typedef union 
{
	uint32_t ip_value;
}ip_addr;

typedef union
{
	unsigned char a[6];
}mac_addr;


struct arp_queue {
	ip_addr ip;
	mac_addr mac;
};




class ip_value{
	private:
		vector<arp_queue> ip_table;
	public:
		/* This function is just for test */
		void fill_some_records(struct arp_queue record,int len)
		{
			int i;
			for (i = 0; i < len; i++) {
				ip_table.push_back(record);
			}
		}

		int find_record(ip_addr ip)
		{
			if(st_score.find(ip) == st_score.end()) 
				return -1; // do't find
			else  
				return 0; // find
		}

		void add_student_record(ip_addr ip_id, int score)
		{
			st_score.insert(make_pair(ip_id, score));
		}

		int delete_score_record(ip_addr ip_id)
		{
			if(!find_record(ip_id)) {
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
			map<string,int>::iterator it;
			for ( it=st_score.begin() ; it != st_score.end(); it++ )
				cout << (*it).first << " => " << (*it).second << endl;	
		}

};

int main()
{
	int i;
	
	ip_value *head;
   	head = new ip_value;
	ip_addr ip_id[10];
	int score[10];
	for (i = 0; i < 10; i++) {
		ip_id.ip_value[i] = 0x01020301 + i;
		score[i] = 90 + i;
	
	}

	student->fill_some_records(ip_id, score, 10);
	student->display_student_score();
#if 1
	if (!student->find_record(0x01020305)){
		cout << "we find it" << endl;
	} else {
		cout << "can't find it" << endl;
	}
#endif


#if 1	
	cout << "\n[add_student_record]" << endl;
	student->add_student_record(0x010203FF, 100);
	student->display_student_score();
#endif

#if 1
	cout << "\n[delete_old_record]" << endl;
	student->delete_old_record();
	student->display_student_score();
#endif

	return 0;
}
