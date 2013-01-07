#include <map>
#include <iostream>
using namespace std;



int main()
{
	/* insert */
	map <string, int> word_count;
	word_count["duli"] = 100;

	/* insert */
	map <string, int> word_count;
	word_count.insert(map<string, int>::value_type("lisi", 99));

	cout << word_count["duli"] << endl;
	++word_count["duli"];
	cout << word_count["duli"] << endl;

	cout << word_count["lisi"] << endl;
	return 0;
}
