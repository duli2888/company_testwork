#include <map>
#include <iostream>
using namespace std;

int main()
{
	map <string, int> word_count;
	word_count["duli"] = 100;
	word_count.insert(map<string, int>::value_type("lisi", 99));

	cout << word_count["duli"] << endl;
	cout << word_count["lisi"] << endl;
	cout << word_count["wang"] << endl;		// 没有初始化或者没有赋值则默认是0

	++word_count["duli"];
	cout << word_count["duli"] << endl;

	return 0;
}
