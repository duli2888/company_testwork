#include <stdio.h>


#define TEST 1

char *str = "test";


void core_test()
{

#if TEST
	str[1] = 'T';

#else
	printf("[str] %s \n",str);
#endif

}

int main()
{

	int i = 10;

	char *arr = "hello"; // char *arr; arr = "hello"; arr[1] = 'H';
	arr[1] = 'H';


	
	printf("[i] %d \n", i);
	return 0;

}
