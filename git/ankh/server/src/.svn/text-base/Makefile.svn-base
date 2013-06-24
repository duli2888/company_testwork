PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

TARGET		= ankh
SRC_CC		= main.cc session.cc device.cc

CFLAGS	+= -UDLDEBUG  ## add by DuLi
## 2012-05 sunray, modified

SRC_C  = $(SRC_DIR)/../netlib/linux_glue.c

## Hi-eth driver
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/ctrl.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/glb.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/mac.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/mdio.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/mii-drv.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/net-drv.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/sys.c
SRC_C  += $(SRC_DIR)/../netlib/hisfv300/common/miiphyutil.c

## for suka_malloc()
SRC_CC		+= $(SRC_DIR)/../netlib/suka_mem.cc


REQUIRES_LIBS += libstdc++ libsupc++ l4re-util l4re l4re_c \
				 l4re_c-util log slab libboost-lite shmc \
				 shmc_ringbuf libankh_packet_analyzer
				
CONFIG_VERBOSE = $(empty) $(empty)
				 
L4_MULTITHREADED = y

PRIVATE_INCDIR += $(SRC_DIR)/../netlib  ## for linux_glue.h
CONTRIB_INCIDR += boost-1_39-lite
CONTRIB_INCIDR += libstdc++-v3

CONTRIB_INCIDR += slem
EXTRA_LIBS     := -ldevmgr -lslem

include $(L4DIR)/mk/prog.mk

