#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread-l4.h>

#include <l4/util/util.h>
/* 
 * Need to include this file before others.
 * Sets our byteorder.
 */
#include "sockets.h"

#include <l4/sys/l4int.h>

static void do_lookup(const char *hostname)
{
	printf("[----------------do_lookup---------------------]\n");
	ip_addr_t i;

	struct hostent *h = gethostbyname(hostname);

	printf("gethostbyname(%s): %s\n", hostname, h ? "SUCCESS" : "FAILED");
	if (h) {
		i.addr = *(u32_t*)h->h_addr_list[0];
		printf("h_name: '%s' -->  %d.%d.%d.%d \n", h->h_name, ip4_addr1(&i), ip4_addr2(&i), ip4_addr3(&i), ip4_addr4(&i));
	}
}


void dns_test(void)
{

	do_lookup("www.taobao.com");
	do_lookup("www.baidu.com");
	do_lookup("www.sina.com");
	do_lookup("www.novel-supertv.com");
	do_lookup("www.novel-supertv.com");
	do_lookup("www.163.com");
	do_lookup("www.sohu.com");
	do_lookup("www.google.com");
	do_lookup("www.android.com");
	do_lookup("www.douban.com");
	do_lookup("www.qq.com");
	do_lookup("www.jd.com");
}

