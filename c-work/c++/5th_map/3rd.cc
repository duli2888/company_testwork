#include <map>
#include <iostream>
using namespace std;



int main()
{
	map<string, int> word_count;
	string word;

	map <string, int>::iterator word_Iter;

	word_count.insert(make_pair("hello", 1));
	word_count.insert(make_pair("world", 1));
	word_count.insert(make_pair("good", 1));
	word_count.insert(make_pair("function", 1));
	word_count.insert(make_pair("novel", 1));
	word_count.insert(make_pair("super", 1));

	// display all object
	
	for(word_Iter = word_count.begin(); word_Iter !=word_count.end(); word_Iter++){
		cout << "[" << word_Iter->first;
		cout << "]---" << word_Iter->second;
		cout << "." << endl;
	}


	while(cin >> word){
		map<string,int>::iterator it = word_count.find(word);
		if(it == word_count.end())
			cout << "we don't find --- " << word << endl;
		else 
			cout << "we find --- " << word << endl;

	}
	return 0;
}
