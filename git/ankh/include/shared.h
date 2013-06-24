#pragma once
#include <stdio.h>
#include <sys/types.h>
namespace Opcode {
	enum Opcodes {
		NIC_MODE,
		socket,
		connect,
		accept,
		accept4,
		bind,
		getsockopt,
		setsockopt,
		listen,
		shutdown,
		recv,
		recvfrom,
		send,
		sendto,
		getsockname,
		getpeername,
		socketpair,			// 此函数没有实现，需要实现
		gethostbyname,
		ioctlsocket,
		select,
		read,
		write,
		closesocket,
		Op_buf_send,
	};
};

namespace Protocol {
	enum Protocols {
		Ankh_Call,
		Pro_Send_Buf
	};
};

#define TR_BUF_SIZE 1500
#define STATIC_IP	0
#define DYNAMIC_IP 1
