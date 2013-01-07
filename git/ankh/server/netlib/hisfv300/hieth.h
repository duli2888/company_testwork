#ifndef __HISILICON_ETH_H
#define __HISILICON_ETH_H

// 2012-04-18 sunray, added start
static inline char *suka_short_file_name(char *name)
{
   int i,pos;
   for(i = 0; name[i] ;i++)
   {
      if (name[i] == '/')
        pos = i;
   }
   return &name[pos+1];
}

#define PRINT printf

#define SUKA_FUNC_ENTRY(x)  \
  int _sft = x; \
  if (_sft) \
    PRINT("\nenter %s()[%s:%d]\n",__FUNCTION__, \
      suka_short_file_name(__FILE__),__LINE__)

#define SUKA_FUNC_EXIT()     \
  if (_sft) \
    PRINT("leave %s()[%s:%d]\n\n",__FUNCTION__,\
      suka_short_file_name(__FILE__),__LINE__)

#define SUKA_FUNC_CALL(f)    PRINT("call "f"()[%s:%d]\n",suka_short_file_name(__FILE__),__LINE__)

#define SFE_ON     SUKA_FUNC_ENTRY(1)
#define SFE_OFF    SUKA_FUNC_ENTRY(0)
#define SFL        SUKA_FUNC_EXIT()


#define sunray_debug(fmt,args...)	printf (fmt ,##args)
#define sunray_debug_off(fmt,args...)


typedef unsigned char		uchar;
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef signed char		s8;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long long	s64;

typedef unsigned long int ulong;


#define REG_BASE_PERI_CTRL	0x10200000
#define REG_PERI_CRG26         0x00A8

#define REG_BASE_SF			0X600D0000

#define PKTSIZE			1518
#define PKTSIZE_ALIGN		1536



// 2012-04-17 sunray, copied from godbox.h
/* hisfv300 */
#define CONFIG_NET_HISFV300
#define CONFIG_ARP_TIMEOUT 50000UL//FIXME
#define CONFIG_NET_RETRY_COUNT	50//FIXME

#define CONFIG_CMD_NET
#define CONFIG_CMD_PING /* do_ping common/cmd_net.c*/
#define CONFIG_CMD_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN 1

 /* set sfv300 mii/rmii mode */
#define HISFV_MII_MODE  0
#define HISFV_RMII_MODE 1
#define HIETH_MII_RMII_MODE_U   HISFV_RMII_MODE
#define HIETH_MII_RMII_MODE_D   HISFV_RMII_MODE
#define HISFV_PHY_U	1
#define HISFV_PHY_D	3

// 2012-04-18 sunray, added end

#if 11 // 2012-04-18 sunray
#include "common/miiphy.h"
#else
#include <config.h>
#include <common.h>
#include <command.h>

#include <linux/list.h>
#include <asm/io.h>

#include <malloc.h>         /* malloc, free, realloc*/

#include <net.h>
#include <miiphy.h>
#include <asm/arch/platform.h>
#endif

#include "mdio.h"

#define OSDRV_MODULE_VERSION_STRING "Hisilicon ETH net controler"

/* ***********************************************************
 *
 * Global varibles and defintions
 *
 *************************************************************
 */

/* configuerable values */

#define ETH_IO_ADDRESS_BASE REG_BASE_SF

#define ETH_MDIO_FRQDIV	2

/* mdiobus device name, such as platform device name */
#define HIETH_MDIOBUS_NAME "hieth_mdiobus"

/* eth device name, such as platform device name */
#define HIETH_SFV300_NAME "hieth_sfv300"
#define MAX_PHY_NAME_LEN 6 /*max limited as : 0:255*/
#define HIETH_MAX_QUEUE_DEPTH 64
#define HIETH_HW_RXQ_DEPTH 1 /*uboot*/
#define HIETH_HW_TXQ_DEPTH 1 /*uboot*/ 

#define HIETH_MAX_FRAME_SIZE PKTSIZE_ALIGN /*1536*/

#define hieth_dump_buf(buf, len) do{\
        int i;\
        char *p = (void*)(buf);\
        for(i=0;i<(len);i++){\
                printf("0x%.2x ", *(p+i));\
                if( !((i+1) & 0x0F) )\
                        printf("\n");\
        }\
        printf("\n");\
}while(0)

#define HIETH_TRACE_LEVEL 8
#define hieth_trace(level, msg...) do { \
		if((level) >= HIETH_TRACE_LEVEL) { \
			printf("hieth_trace:%s:%d: ", __FILE__, __LINE__); \
			printf(msg); \
			printf("\n"); \
		} \
	}while(0)

#define hieth_error(s...) do{ \
		printf("hieth:%s:%d: ", __FILE__, __LINE__); \
		printf(s); \
		printf("\n"); \
	}while(0)

#define hieth_assert(cond) do{ \
		if(!(cond)) \
			printf("Assert:hieth:%s:%d\n", \
					__FILE__, \
					__LINE__); \
	}while(0)

#define hil_vqueue_assert(cond) hieth_assert(cond)
#define hil_vqueue_error(s...) hieth_error(s)
#define hil_vqueue_trace_printf(level, s...) do{ if((level) >=HIETH_TRACE_LEVEL)printf(s); }while(0)

/* Error number */
#define HIETH_E_QUEUE	(-1)
#define HIETH_E_BUSY	(-2)
#define HIETH_E_FULL	(-3)
#define HIETH_E_EMPTY	(-4)

struct hieth_frame_desc {
	unsigned long frm_addr;		/* required by the controler */
	unsigned int  frm_len :11;	/* required by the controler */
};
#define hieth_fd_copy(newfd, fd)	do{ newfd = fd; }while(0)
#define HIETH_FD_INIT(fd)		do{ fd.frm_addr=0; fd.frm_len=0; }while(0)

#define hieth_trace_fd(level, fd) hieth_trace(level, \
		#fd "<%p>={ .frm_addr=%08lx, .frm_len=%d}", \
		&(fd), (fd).frm_addr, (fd).frm_len)

/* port */
#define UP_PORT		0
#define DOWN_PORT	1

struct hieth_netdev_local {
	unsigned long iobase_phys; /* physical io addr */
	int port	:1; /* 0 => up port,    1 => down port */

	const char *phy_name;
	int link_stat;
};

/* ***********************************************************
 *
 * Only for internal used!
 *
 * ***********************************************************
 */

/* read/write IO */
// 2012-04-18 sunray, added start
#define cpu_to_le16(x)		(x)
#define cpu_to_le32(x)		(x)
#define cpu_to_le64(x)		(x)
#define le16_to_cpu(x)		(x)
#define le32_to_cpu(x)		(x)
#define le64_to_cpu(x)		(x)

#define __raw_readl(c)      (*(volatile unsigned int *)(SUKA_IO_ADDRESS(c)))
#define __raw_writel(v,c)   (*(volatile unsigned int *)(SUKA_IO_ADDRESS(c)) = (v))
// 2012-04-18 sunray, added end

#define _readl(c)	({ unsigned int __v = le32_to_cpu(__raw_readl(c)); __v; })
#define _writel(v,c)	__raw_writel(cpu_to_le32(v),c)

#define hieth_readl(ld, ofs) ({ unsigned long reg=_readl((ld)->iobase_phys + (ofs)); \
				hieth_trace(2, "_readl(0x%08X) = 0x%08lX", (int)((ld)->iobase_phys + (ofs)), reg); \
				reg; })
#define hieth_writel(ld, v, ofs) do{ _writel(v, (ld)->iobase_phys + (ofs)); \
				hieth_trace(2, "_writel(0x%08X) = 0x%08lX", (int)((ld)->iobase_phys + (ofs)), (unsigned long)(v)); \
			}while(0)

#define MK_BITS(shift, nbits)	((((shift)&0x1F)<<16) | ((nbits)&0x1F))

#define hieth_writel_bits(ld, v, ofs, bits_desc) do{ \
		unsigned long _bits_desc = bits_desc; \
		unsigned long _shift = (_bits_desc)>>16; \
		unsigned long _reg = hieth_readl(ld, ofs); \
		unsigned long _mask = ((1<<(_bits_desc & 0x1F)) - 1)<<(_shift); \
		hieth_writel(ld, (_reg &(~_mask)) | (((v)<<(_shift)) &_mask), ofs); \
	} while(0)
#define hieth_readl_bits(ld, ofs, bits_desc) ({ \
		unsigned long _bits_desc = bits_desc; \
		unsigned long _shift = (_bits_desc)>>16; \
		unsigned long _mask = ((1<<(_bits_desc & 0x1F)) - 1)<<(_shift); \
		(hieth_readl(ld, ofs)&_mask)>>(_shift); })

#define local_lock_init(ld)
#define local_lock_exit(ld)
#define local_lock(ld)
#define local_unlock(ld)

#define UD_REG_NAME(name)       ((ld->port==UP_PORT)? U_##name : D_##name)
#define UD_BIT_NAME(name)       ((ld->port==UP_PORT)? name##_U : name##_D)

#endif

