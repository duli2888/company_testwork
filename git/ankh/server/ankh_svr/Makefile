PKGDIR	?= ../..

L4DIR	?= $(PKGDIR)/../..

TARGET			 = ankh_interface
SRC_CC			 = server.cc init_lib.cc
SYSTEMS          = x86-l4f arm-l4f
REQUIRES_LIBS    = ankh_lwip libpthread
L4_MULTITHREADED = y

include $(L4DIR)/mk/prog.mk
