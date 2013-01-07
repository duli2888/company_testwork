#include<stdio.h>

int main()
{
	int i = 2;
	unsigned long off_set = 5;
	char a = 'a';
	char sum;
	sum = a + off_set;
	
	char * arr = malloc(10);
	
	printf("arr[address] %d\n",arr);
	arr = arr + i;
	printf("arr[address] %d\n",arr);
	

	printf("sum[size]=%d\n",sizeof(sum));
	printf("%d\n",sizeof(a + off_set)); 
	printf("sum[value]=%c\n",sum);
}
