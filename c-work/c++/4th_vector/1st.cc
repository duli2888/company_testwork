#include <vector>
#include <stdio.h>
#include <iostream>

using std::vector;
using namespace std;

int main()
{
	vector<int> v1;
	vector<int> v2(20);

	v1.push_back(11);
	v1.push_back(12);
	v1.push_back(13);
	v2.push_back(21);
	printf("%d\n", v1.empty());
//	v1.size();
	vector<int>::size_type v1_size= v1.size();
	cout << v1_size << endl;
//	printf("[size] = %d\n",v1_size);
	//printf("[size]%d\n", v1.size());
	// v1 = v2;
	// v2[0] = 30;
	printf("v1[0] = %d, v1[1] = %d, v2[2] = %d, v2[20] = %d\n", v1[0], v1[1], v1[2], v2[20]);
}
