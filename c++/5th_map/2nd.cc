#include <map>
#include <iostream>
using namespace std;

int main()
{
	map<string, int> word_count;
	string word;

	while(cin >> word) {
		// insert element with key equal to word and value 1;
		// if word already in word_count,insert function does nothing
		pair< map < string, int > :: iterator, bool> ret =
			word_count.insert(make_pair(word, 1));

		if(!ret.second)		// word already in word_count
			++ret.first->second;	// increment counter

		map<string,int>::iterator it = word_count.find("foobar");

		if (it == word_count.end())
			cout << "This is not foobar,please input foobar" << endl;
		else 
			cout << "we find foobar" << endl;
	}

	map<string,int>::iterator it = word_count.find("foobar");
	if (it != word_count.end())
		cout << "we do not find foobar" << endl;
	else 
		cout << "we find foobar" << endl;

	return 0;
}
