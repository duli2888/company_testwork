PKGDIR          ?= ../..
L4DIR           ?= $(PKGDIR)/../..

TARGET           = ping
##SYSTEMS          = x86-l4f arm-l4f
REQUIRES_LIBS    = l4util l4re-util shmc_ringbuf libstdc++ libboost-lite
L4_MULTITHREADED = y
SRC_CC   = ping.cc

include $(L4DIR)/mk/prog.mk
