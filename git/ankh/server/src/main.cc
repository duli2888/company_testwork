#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <boost/format.hpp>

#include <l4/re/env>
#include <l4/re/env>
#include <l4/re/namespace>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/re/protocols>
#include <l4/re/util/meta>
#include <l4/cxx/ipc_server>
#include <l4/cxx/iostream>
#include <l4/cxx/l4iostream>
///#include <l4/dde/linux26/dde26.h>  // 2012-05 sunray, deleted

#include <l4/sys/debugger.h>
#include <l4/ankh/protocol>
#include <l4/ankh/packet_analyzer.h>
#include "session"
#include "device"
#include "linux_glue.h"

static L4Re::Util::Registry_server<> server;

int Ankh::ServerSession::dispatch(l4_umword_t, L4::Ipc_iostream &ios)
{
	l4_msgtag_t t;
	ios >> t;
	int ret = -1;

	if (t.label() != Ankh::Protocol::Ankh)
		return -L4_EBADPROTO;

	L4::Opcode op;
	ios >> op;

	switch(op)
	{
		case Ankh::Opcode::Activate:
			ret = shm_create();
			activate();
			break;

		case Ankh::Opcode::Deactivate:
			deactivate();
			break;
		case Ankh::Opcode::SharedIp: // Added by DuLi
			eth_shared_ip_addr(shared_ip_addr());
			break;
		default:
			std::cout << "Unknown op: " << op << "\n";
			ret = -L4_ENOSYS;
	}

	return ret;
}



class Ankh_server : public L4::Server_object
{
	public:
		int dispatch(l4_umword_t, L4::Ipc_iostream &ios);
};


int Ankh_server::dispatch(l4_umword_t, L4::Ipc_iostream &ios)
{
	l4_msgtag_t t;
	ios >> t;

	switch (t.label()) {
		/*
		 * Used by Ned to find out our supported protocols
		 */
		case L4::Meta::Protocol:
			return L4Re::Util::handle_meta_request<L4::Factory>(ios);

		/*
		 * Factory protocol to create new sessions
		 */
		case L4::Factory::Protocol:
			{
				unsigned long size = 250;
				char buf[size];
				memset(buf, 0, size);

				unsigned op;
				ios >> op;
				if (op != 0)
					return -L4_EINVAL;

				L4::Ipc::Varg opt;
				ios.get(&opt);

				if (!opt.is_of<char const *>()) {
					std::cerr << "[ERR] Parameter needs to be a string constant.\n";
					return -L4_EINVAL;
				}

				strncpy(buf, opt.value<char const*>(), size);
				unsigned len = opt.length();
				buf[len] = 0;
					
				Ankh::ServerSession *ret = Ankh::Session_factory::get()->create(buf);
				if (ret != 0)
				{
					server.registry()->register_obj(ret);
					ios << ret->obj_cap();
					return L4_EOK;
				}
				else
				{
					L4::cerr << "Error creating Ankh session object.\n";
					return -L4_ENOMEM;
				}
			}
		default:
			std::cout << "Unknown protocol: " << t.label() << "\n";
			return -L4_ENOSYS;
	}

	return L4_EOK;
}

EXTERN_C void netdev_add(void *ptr)
{
	Ankh::Device_manager::dev_mgr()->add_device(ptr);
}


EXTERN_C void enable_ux_self()
{
	l4_thread_control_start();
	l4_thread_control_ux_host_syscall(1);
	l4_thread_control_commit(pthread_getl4cap(pthread_self()));
}

// 此函数是将网卡接收到的数据发给client端
EXTERN_C int packet_deliver(void *packet, unsigned len, char const * const dev, unsigned local)
{
#if 0
	static int callcount = 0;
	if (++callcount >= 7500) {
	    L4Re::Env::env()->parent()->signal(0, 1);
	}
#endif

	assert(packet != 0);
	unsigned cnt = 0;
	int c = l4_debugger_global_id(pthread_getl4cap(pthread_self()));
	(void)c;

#if 0
	Ankh::Util::print_mac(static_cast<unsigned char*>(packet));
	std::cout << " <- ";
	Ankh::Util::print_mac(static_cast<unsigned char*>(packet)+6);
	std::cout << std::endl;
#endif
	// [DuLi]: 创建一个新的Session_factory,并通过目的MAC地址,或者设备名来返回第一个Session;
	Ankh::ServerSession *s = Ankh::Session_factory::get()->find_session_for_mac(
	                               static_cast<const char * const>(packet), dev);

	while (s != 0)
	{
		if (cnt == 0) {
			if (s->debug()) {
				packet_analyze(static_cast<char*>(packet), len);
			}
		}
		if (local)
			s->dev()->lock();
		s->deliver(static_cast<char*>(packet), len);
	   	// DuLi:deliver() transmit to client,client can print
														
		if (local)
			s->dev()->unlock();
		++cnt;
		s = Ankh::Session_factory::get()->find_session_for_mac(
		          static_cast<const char * const>(packet), dev, s);
	}
	// [DuLi]: 将网卡收到的包给每个Session将此包发一份，如果开启了广播，则所有的Session都会收到此包

	return cnt;
}

int main(int argc, char **argv)
{
	(void)argc;
	Ankh_server ankh;

	char mac[6];

	std::cout << argv[1] << "\n";
	sscanf(argv[1],"mac=%x:%x:%x:%x:%x:%x",(unsigned int*)&mac[0],(unsigned int*)&mac[1],(unsigned int*)&mac[2],
			(unsigned int*)&mac[3],(unsigned int*)&mac[4],(unsigned int*)&mac[5]);

	int num_devs = open_network_devices(1,mac);
	std::cout << "Opened " << num_devs << " network devices.\n";
	Ankh::Device_manager::dev_mgr()->dump_devices();



	server.registry()->register_obj(&ankh, "ankh_service");
	std::cout << "Registered @ registry.\n";



	std::cout << "Gooood Morning Ankh-Morpoooooork! TID = 0x" << std::hex << l4_debugger_global_id(pthread_getl4cap(pthread_self())) << std::dec << "\n";
	l4_debugger_set_object_name(pthread_getl4cap(pthread_self()), "ankh.main");

	try
	{
		server.loop();
	}
	catch (L4::Base_exception const &e)
	{
		L4::cerr << "Error: " << e << '\n';
		return -1;
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}

	return 0;
}
