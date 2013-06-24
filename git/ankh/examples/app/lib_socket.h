#ifndef LIB_SOCKET_H
#define LIB_SOCKET_H
#include "sockets.h"

void *pthread_app(void *);
int func_app_main_call();

class Buf_send_to_server : public L4::Server_object
{
	public:
		int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);
};


#endif
