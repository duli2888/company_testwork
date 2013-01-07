#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

static int16_t
lwip_standard_chksum(void *dataptr, uint16_t len)
{
 static uint32_t acc;
  uint16_t src;
  uint8_t *octetptr;

  acc = 0;
#if 0
  /* dataptr may be at odd or even addresses */
  octetptr = (uint8_t*)dataptr;

	int i = 0;

	for (i = 0; i < len; i++) {
		printf("[%d] = [%x] ", i, *octetptr);
		octetptr++;
	}
#endif
	octetptr = (uint8_t *)dataptr;
	
  while (len > 1) {
    /* declare first octet as most significant
       thus assume network order, ignoring host order */
    src = (*octetptr) << 8;
    octetptr++;
    /* declare second octet as least significant */
    src |= (*octetptr);
    octetptr++;
    acc += src;
    len -= 2;
  }

	printf("\n[acc] = %x\n", acc);

  if (len > 0) {
    /* accumulate remaining octet */
    src = (*octetptr) << 8;
    acc += src;
  }
  /* add deferred carry bits */
  acc = (acc >> 16) + (acc & 0x0000ffffUL);
  if ((acc & 0xffff0000UL) != 0) {
    acc = (acc >> 16) + (acc & 0x0000ffffUL);
  }
  /* This maybe a little confusing: reorder sum using htons()
     instead of ntohs() since it has a little less call overhead.
     The caller must invert bits for Internet sum ! */
  return htons((uint16_t)acc);
}

unsigned short checksumcalculate(unsigned short *szBUF,int iSize)
{       
	unsigned long ckSum=0;

	for(;iSize>1;iSize-=sizeof(unsigned short))
		ckSum+=*szBUF++;

	if(iSize==1)
		ckSum+=*(unsigned char *)szBUF;

	ckSum=(ckSum>>16)+(ckSum&0xffff);
	ckSum+=(ckSum>>16);

	return(unsigned short )(~ckSum);
}


int main()
{
	unsigned char	ip[] = { 
		0x00,0x00,0x00,0x33,
		0x45,0x00,0x00,0x1c,  // begin IP packet																										
		0x00,0x00,0x40,0x00,  // [identif(16bit)] [flag(3bit)] [(13bit)]
		0xff,0x01,0x00,0x00,  // [TTL(8bit)] [Protocol(8bit)] [checksum(16bit)] checksum 6306 -> 620e
		0x0a,0xff,0x01,0xf9,  // src ip
		0x0a,0xff,0x01,0xdc,  // dst ip
	};

	unsigned int int_check = 0;
	unsigned int calculate_check = 0;

	calculate_check = (unsigned short)checksumcalculate((unsigned short *)(ip + 4), 20);
	int_check = (unsigned short)lwip_standard_chksum(ip, 20);

	printf("calculate_check = %x,int_check = %x\n",calculate_check, int_check);

	return 0;
}
