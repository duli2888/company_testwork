#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void print_mac_address(const unsigned char *mac)                                                                         
{              
	int i;     

	for(i=0; i<6; i++)         
		printf("%02X%s", mac[i], i==5 ? "" : ":");
	printf("\n");
}

static int str_to_hwaddr(unsigned char *hwaddr, const char *str)
{
	char c;
	uint8_t used = 0;
	uint8_t quarter = 0;
	memset(hwaddr, 0, 6);
	while(isspace(*str)) ++str;
	while('\0' != *str) {
		c = *str++;
		switch(c) {
			default :
				if(strchr("-:. ", c)) {
					if(used) {
						++hwaddr;
						used = 0;
					}
					continue;
				}
				return -1;
			case '0' ... '9':
				quarter = c - '0';
				break;
			case 'a' ... 'f':
				quarter = c - 'a' + 10;
				break;
			case 'A' ... 'F':
				quarter = c - 'A' + 10;
				break;
		}
		*hwaddr = ((*hwaddr)<<4) + quarter;
		if(used) ++hwaddr;
		used = !used;
	}
	return 0;
}

static int print_hwaddr(const char *mac)
{
	unsigned char hwaddr[6];
	str_to_hwaddr(hwaddr, mac);
#if 0
	fprintf(stdout, "mac: %s, hwaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac,
			(uint32_t)hwaddr[0], (uint32_t)hwaddr[1],
			(uint32_t)hwaddr[2], (uint32_t)hwaddr[3],
			(uint32_t)hwaddr[4], (uint32_t)hwaddr[5]);
#endif
	print_mac_address(hwaddr);
	return 0;
}

int main(int argc, char *argv[])
{
	char mac[128];
	strcpy(mac, "d0:67:e5:1e:f8:82");
	print_hwaddr(mac);
	strcpy(mac, "01:02:03:04:05:06");
	print_hwaddr(mac);
	strcpy(mac, "01:02:03:04:05:06");
	print_hwaddr(mac);
	return 0;
}
