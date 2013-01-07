#include <map>
#include <iostream>
#include <stdint.h>
#define ARP_LENGTH 32
using namespace std;


typedef union
{
	unsigned char a[6];

}mac_addr;


typedef union
{
	uint32_t a;

}ip_addr;

struct arp_entry {                                                 
    ip_addr ip;                                                    
    mac_addr mac;                                                  
 };         

class arp_queue{
	private:
		map<ip_addr, mac_addr> arp_table;
	public:
		/* This function is just for test */
		void fill_some_records(struct arp_entry *table,int len)
		{
			int i;
			for (i = 0; i < len; i++) {
				arp_table.insert(make_pair(table[i].ip, table[i].mac));
			}
		}

		int find_record(ip_addr ip)
		{
			if(arp_table.find(ip) == arp_table.end()) 
				return -1; // do't find
			else  
				return 0; // find
		}

		static void add_arp_record(struct arp_entry *table)
		{
			arp_table.insert(make_pair(table->ip, table->mac));
		}

		static int delete_arp_record(struct arp_entry *table)
		{
			if(!find_record(table->ip)) {
			//	arp_table.erase(it);
			} else {
				return -1; // delete failed
			}
			return 0;	// delete success
		}

		static int delte_arp_old_record()
		{
			arp_table.erase(arp_table.begin());
		}

		static int display_arp_table()
		{
			map<char,int>::iterator it;
			for ( it=arp_table.begin() ; it != arp_table.end(); it++ )
				cout << (*it).first << " => " << (*it).second << endl;	
		}

};

int main()
{
	int i;
	
	class arp_queue arp_head;
   	arp_head = new class arp_queue;
	struct arp_entry tmp[10];
	for (i = 0; i < 10; i++) {
		tmp[i].ip = 0x01020304 - 1; 
		tmp[i].mac = 0x0a0b0c0d0e0f -1;
	
	}
		fill_some_records(tmp, 10);
	display_arp_table();

//	find_record();

//	add_arp_record();
//	display_arp_table();

//	delte_arp_old_record();
//	display_arp_table();

	return 0;
}
