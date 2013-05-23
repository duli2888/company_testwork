#include <stdio.h>

int main()
{
	int arr[10] = {2, 5, 9, 3, 7, 8, 1, 4, 6, 0};
	int i = 0;
	int tmp = -1;
	int mid = 0;

	for (i = 0; i < 10; i++) {
		if (arr[0] > tmp) {
			mid = arr[0];
			arr[0] = tmp;
			tmp = mid;
		}
		
	}


}
