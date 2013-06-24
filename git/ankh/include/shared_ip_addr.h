/*
 * Added by DuLi[2012-11-26]
 * This file only for global ip address, that will modify conveniently.
 */

#ifndef SHARED_IP_ADDR_H
#define SHARED_IP_ADDR_H


#include <l4/sys/l4int.h>

#define _IP4_ADDR(a,b,c,d) ((l4_uint32_t)((d) & 0xff) << 24) | ((l4_uint32_t)((c) & 0xff) << 16) | ((l4_uint32_t)((b) & 0xff) << 8) | (l4_uint32_t)((a) & 0xff)


#define GLOBAL_IP_ADDR _IP4_ADDR(10, 255, 1, 249) 

l4_uint32_t v_global_ip_addr;

L4_CV void shared_ip_addr(unsigned int ip_addr) L4_NOTHROW;
#endif 

