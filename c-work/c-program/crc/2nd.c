#include <stdio.h>

#define DLDEBUG 0

unsigned short crc16( unsigned char *data, int length)  
{
	unsigned short reg_crc;
	unsigned short s_crcchk;

	s_crcchk = 0;
	reg_crc = 0x0;				// 应该是0，不是ffff

	while(length--) {
		reg_crc ^= *data++;
		if(DLDEBUG) printf("[%d]reg_crc = %d\n", length ,reg_crc);
		for(s_crcchk = 0; s_crcchk < 8; s_crcchk++) {
			if(reg_crc & 0x01) {    // 此位是1
				reg_crc = (reg_crc >> 1) ^ 0xa001;				// 这里填多项式A001, 每次右移动一次，与A001异或，总过是8位，则右移动8次
			} else {				// 此位是0
				reg_crc = reg_crc >> 1;
			}
			if(DLDEBUG) printf("[%d][%d]reg_crc = %d\n",length, s_crcchk ,reg_crc);
		}
		if(DLDEBUG) printf("[%d]reg_crc = %d\n", length ,reg_crc);
	}

	return reg_crc;  
}

int main()
{
	//unsigned char *a = "hello world";							// 这里填需要校验的数据
	//unsigned char *a = "novel-supertv";						// 这里填需要校验的数据
	unsigned char *a = "a";										// 这里填需要校验的数据:59585
	int me = crc16(a, 1);
	printf("%d\n", me);

	return 0;
}
