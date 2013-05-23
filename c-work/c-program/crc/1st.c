#include <stdio.h>

unsigned char test[16] ="hello world";
unsigned char len = 11;
void main( void )
{
	unsigned int crc = 0;
	unsigned char i;
	unsigned char *ptr = test;

	while(len--)
	{
		for(i = 0x80; i != 0; i = i >> 1) {
			if((crc & 0x8000) != 0) {
				crc = crc << 1;
				crc = crc ^ 0x1021;
			} else {
				crc = crc << 1;
			}
			if((*ptr & i) != 0) {
				crc = crc ^ 0x1021; 
			}
		}
		ptr++;
	}
	printf("%d \n",crc);
}
