#ifndef _LINUX_STDDEF_H
#define _LINUX_STDDEF_H

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#ifndef _SIZE_T
#if 11
//#include "types.h"
#define _SIZE_T
typedef unsigned int	size_t;
#else
#include <linux/types.h>
#endif
#endif

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif
