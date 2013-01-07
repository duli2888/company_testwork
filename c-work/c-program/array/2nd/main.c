#include <stdio.h>

int cal_length(int *arr)
{
	int l;
	// l = sizeof(arr);
	// l = strlen(arr);
	for (l = 0; l < 50; l++)
		printf("arr[l] = %d\n", arr[l]);

	return l;
}

int main()
{
	int arr[20] = {1,2,3,4,5,6,7,8,9,9,9,9};
	int arr_length = 0;

	arr_length = cal_length(arr);
	printf("length = %d\n", arr_length);
	return 0;
}
