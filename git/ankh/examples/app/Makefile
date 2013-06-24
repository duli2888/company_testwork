PKGDIR          ?= ../..
L4DIR           ?= $(PKGDIR)/../..

SYSTEMS          = x86-l4f arm-l4f                                                                                                                                          
L4_MULTITHREADED = y

TARGET           = ankh_app 
SRC_CC			 = app.cc  ping.cc lib_socket.cc tcp_test.cc udp_tftp.cc dns_test.cc func_test.cc

SRC_CC	+=	web_server.cc
SRC_CC	+=	ftp/server.cc
SRC_CC	+=	ftp/handles.cc

L4_MULTITHREADED = y
 
CONTRIB_INCIDR += boost-1_39-lite
CONTRIB_INCIDR += libstdc++-v3

include $(L4DIR)/mk/prog.mk
