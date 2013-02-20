/*
 * This program is test for two-dimension vector
 */
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

int main()
{
	vector<vector<int> > a;			// 在嵌套模板实参列表中应当使用'> >'而非'>>'

	for(int i = 0; i < 5; i++) {
		a.push_back(vector<int>());
		for(int j=0; j<5; j++) {
			a[i].push_back(j);
		}
	}

	for(int i = 0; i < 5; i++) {
		for(int j=0; j<5;j++) {
			cout<< setw(4) << a[i][j] << " ";
		}
		cout << endl;
	}

	return 0;
}
