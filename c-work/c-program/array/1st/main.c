#include <stdio.h>

int cal_length(char *arr)
{
	int l;
	//l = sizeof(arr);
	l = strlen(arr);
	//for (l = 0; arr[l]; l++);

	return l;
}

int main()
{
	char arr[20] = "hello world";
	int arr_length = 0;

	arr_length = cal_length(arr);
	printf("length = %d\n", arr_length);
	return 0;
}
