#include <l4/re/env>
#include <l4/re/namespace>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>
#include <l4/cxx/iostream>
#include <l4/ankh/protocol>
#include <l4/ankh/shm>
#include <l4/ankh/session>
#include <l4/util/util.h>
#include <l4/shmc/shmc.h>
#include <boost/format.hpp>
#include <iostream>
#include <cstring>
#include <l4/sys/debugger.h>
#include <pthread-l4.h>
#include <cstdlib>

Ankh::Shm_receiver *recv;
Ankh::Shm_sender   *send;
Ankh::Shm_chunk    *info;
char *shm_name;

int cfg_shmsize = 2048;
l4shmc_area_t ankh_shmarea;

#define mac_fmt       "%02X:%02X:%02X:%02X:%02X:%02X"
#define mac_str(mac)  (unsigned char)((mac)[0]), (unsigned char)((mac)[1]), \
                      (unsigned char)((mac)[2]), (unsigned char)((mac)[3]), \
                      (unsigned char)((mac)[4]), (unsigned char)((mac)[5])

static const int print_rate = 30;// 2012-05-07 sunray 30000;
unsigned char const bcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
 * Print a MAC formatted
 */
void print_mac(unsigned char *macptr)
{
	char macbuf_size = 32;
	char mac_buf[macbuf_size];
	snprintf(mac_buf, macbuf_size, mac_fmt, mac_str(macptr));
	boost::format f("%1$-18s");
	f % &mac_buf[0];
	std::cout << f;
}


char *human_readable_size(unsigned size) {
	char sizebuf[40];

	if (size < 1000)
		snprintf(sizebuf, sizeof(sizebuf), "%d B", size);
	else if (size < 1000000)
		snprintf(sizebuf, sizeof(sizebuf), "%.02f kiB", size/1000.0);
	else if (size < 1000000000)
		snprintf(sizebuf, sizeof(sizebuf), "%.02f MiB", size/1000000.0);
	else
		snprintf(sizebuf, sizeof(sizebuf), "%.02f GiB", size/1000000000.0);

	return strdup(sizebuf);
}


/*
 * Print my device's stats
 */
void print_stats()
{
	struct AnkhSessionDescriptor *sd = reinterpret_cast<struct AnkhSessionDescriptor*>(info->addr());

	std::cout << "info @ " << (void*)sd << "\n";
	print_mac(&sd->mac[0]); std::cout << "MTU " << sd->mtu << "\n";
	std::cout << "RX packets: " << sd->num_rx << " dropped: "    << sd->rx_dropped << "\n";
	std::cout << "TX packets: " << sd->num_tx << " dropped: "    << sd->tx_dropped << "\n";
	char *s1 = human_readable_size(sd->rx_bytes);
	char *s2 = human_readable_size(sd->tx_bytes);
	std::cout << "RX bytes: "  << s1
	          << " TX bytes: " << s2 << "\n";
	free(s1); free(s2);
}

static void ankh_activate()
{
	L4::Cap<void> ankh_server;

	if (!(ankh_server = L4Re::Env::env()->get_cap<void>("ankh"))) {
		printf("Could not find Ankh server.\n");
		assert(false);
	}

	L4::Ipc_iostream s(l4_utcb());

	s << l4_umword_t(Ankh::Opcode::Activate);

	l4_msgtag_t res = s.call(ankh_server.cap(), Ankh::Protocol::Ankh);
	ASSERT_EQUAL(l4_ipc_error(res, l4_utcb()), 0);

	printf("activated Ankh connection.\n");
}


void get_shm_area(L4::Cap<void> /*session*/)
{
	int err = l4shmc_attach(shm_name, &ankh_shmarea);
	ASSERT_OK(err);
	std::cout << "Attached shm area '" << shm_name << "': " << err << "\n";

	Ankh::Shm_ringbuffer *send_rb = Ankh::Shm_ringbuffer::create(
	                                   &ankh_shmarea, "tx_ring",
	                                   "tx_signal", cfg_shmsize);
	Ankh::Shm_ringbuffer *recv_rb = Ankh::Shm_ringbuffer::create(
	                                   &ankh_shmarea, "rx_ring",
	                                   "rx_signal", cfg_shmsize);
	Ankh::Shm_chunk *info_chunk   = Ankh::Shm_chunk::create(&ankh_shmarea,
	                                   "info", sizeof(AnkhSessionDescriptor));

	std::cout << "SEND RB @ " << (void*)send_rb
	          << " RECV RB @ " << (void*)recv_rb
	          << " INFO @ " << (void *)info_chunk << "\n";

	ankh_activate();

	send = new Ankh::Shm_sender(&ankh_shmarea, "tx_ring",
	                            "tx_signal");
	std::cout << "sender: " << (void*)send << "\n";
	recv = new Ankh::Shm_receiver(&ankh_shmarea, "rx_ring",
	                              "rx_signal");
	std::cout << "receiver: " << (void*)recv << "\n";

	info = Ankh::Shm_chunk::get(&ankh_shmarea, "info",
	                            sizeof(struct AnkhSessionDescriptor));

	struct AnkhSessionDescriptor *sd = reinterpret_cast<struct AnkhSessionDescriptor*>(info->addr());

	std::cout << "Got assigned MAC: ";
	print_mac(sd->mac);
	std::cout << "\n";
	print_stats();
}

#if 0
#define DEBUG(x) \
	std::cout << x;
#else
#define DEBUG(x) {}
#endif

typedef struct {
	char _1[4];
	unsigned char txbuf[1024];
	char _2[4];
	unsigned char rxbuf[1024];
	char _3[4];
	const unsigned char packet_const;
	char _4[4];
} rx_struct;

rx_struct recv_buffer = {
	{0xAA, 0xAA, 0xAA, 0xAA},
	{0, },
	{0xBB, 0xBB, 0xBB, 0xBB},
	{0, },
	{0xCC, 0xCC, 0xCC, 0xCC},
	0x1B,
	{0xDD, 0xDD, 0xDD, 0xDD}
};

static void
generate_packet(unsigned char *buf,
				unsigned char dest[6],
				unsigned char src[6],
				unsigned char payload,
				unsigned size)
{
	memcpy(buf, dest, 6);
	memcpy(buf+6, src, 6);
	memset(buf+12, payload, size);
}

static void send_packet(Ankh::Shm_sender *snd,
						unsigned char *packet,
						unsigned size)
{
	DEBUG("sending()\n");
	int err = snd->next_copy_in((char*)packet, size);
	assert(err == 0);
	DEBUG("commit()\n");
	snd->commit_packet();
}

static void recv_packet(Ankh::Shm_receiver *rcv,
						unsigned char *buf,
						unsigned *size)
{
	DEBUG("recv()\n");
	recv->wait_for_data();
	DEBUG("got data.\n");
	int err = rcv->next_copy_out((char*)buf, size);
	assert(err == 0);
	DEBUG("copied out\n");
}


void sanity_check_packet(unsigned char *buf,
						 unsigned char src[6],
						 unsigned char dest[6],
						 unsigned char payload,
						 unsigned size)
{
	if (memcmp(buf, dest, 6)) {
		std::cout << "Dest mismatch in buf @ " << (void*)buf << " ";
		print_mac(buf);
		std::cout << " <-> ";
		print_mac(dest);
		std::cout << std::endl;
		assert(memcmp(buf, dest, 6) == 0);
	}
	if (memcmp(buf + 6, src, 6)) {
		std::cout << "Src mismatch in buf @ " << (void*)buf << " ";
		print_mac(buf+6);
		std::cout << " <-> ";
		print_mac(src);
		std::cout << std::endl;
		assert(memcmp(buf+6, src, 6) == 0);
	}

	unsigned char *c = buf + 12;
	for ( ; c < buf + size; ++c)
		assert(*c == payload);
}


void data_format_print(unsigned char *buf,unsigned int length)
{
    int i;
    unsigned int line = 0,len;

    printf("\n====================================================================\n");
    printf("    data length %d (0x%x)\n",length,length);
    printf("    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
    printf("    -----------------------------------------------\n");
    while(length)
    {
        printf("%02u: ",line++);
        len = length > 16 ? 16 :length;
        for (i=0;i<len;i++) printf("%02X ",*buf++);
        buf -=len;
        for (i=len;i<16;i++)
            printf("   ");
        for (i=0;i<len;i++,buf++) printf("%c",isprint(*buf) ? *buf : '.');
        length -= len;
        printf("\n");
    }
    printf("====================================================================\n");
}



static unsigned
NetCksum(unsigned char * ptr, int len)
{
	unsigned long	xsum;
	unsigned short *p = (unsigned short *)ptr;

	xsum = 0;
	while (len-- > 0)
		xsum += *p++;
	xsum = (xsum & 0xffff) + (xsum >> 16);
	xsum = (xsum & 0xffff) + (xsum >> 16);
	return (xsum & 0xffff);
}

static int
NetCksumOk(unsigned char * ptr, int len)
{
	return !((NetCksum(ptr, len) + 1) & 0xfffe);
}

static void send_test(void)
{
  unsigned char packet[128] = {
    0x00,0x1f,0x16,0x23,0x10,0xbf, // dst mac
    0xc2,0x83,0x51,0xc6,0x1a,0x64, // src mac
    0x00,0x40,  // length
  };

  int i;

  for(i=14;i<64+14+4;i++)
    packet[i] = 0xaa;

  send_packet(send, packet, 64+14);
}

unsigned short checksumcalculate(unsigned short *szBUF,int iSize)
{       
	unsigned long ckSum=0;

	for(;iSize>1;iSize-=sizeof(unsigned short))
		ckSum+=*szBUF++;

	if(iSize==1)
		ckSum+=*(unsigned char *)szBUF;

	ckSum=(ckSum>>16)+(ckSum&0xffff);
	ckSum+=(ckSum>>16);

	return(unsigned short )(~ckSum);
}

static void ping(char *myip, char* targetip)
{
	unsigned char mac[6]       = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char mymac[6]     = {0x00};
	unsigned bufsize = sizeof(recv_buffer.txbuf);
	// minimum size for sending test packets
	int min_size = 20;
	// maximum random element in size: bufsize - header len - minimum size
	int data_size = bufsize - 12 - min_size;
	unsigned size;

    unsigned char targetmac[6];

    unsigned char arp_request[128] = {
        0xff,0xff,0xff,0xff,0xff,0xff, // dst mac
        //0xc2,0x83,0x51,0xc6,0x1a,0x64, // src mac
        0x00,0x00,0x00,0x00,0x00,0x00, // src mac
        0x08,0x06, // type: arp
        0x00,0x01, // hw type: ethernet
        0x08,0x00, // protocol type: IP
        0x06,      // hw size
        0x04,      // protocol size
        0x00,0x01, // opcode: request
        0x00,0x00,0x00,0x00,0x00,0x00, // sender mac
        //0x0a,0xff,0x01,0xd3,   // sender ip: 10.255.1.211
        0x00,0x00,0x00,0x00,   // sender ip: 10.255.1.211
        0x00,0x00,0x00,0x00,0x00,0x00, // target mac
        //0x0a,0xff,0x01,0x0a,   // target ip: 10.255.1.10
        0x00,0x00,0x00,0x00,   // target ip: 10.255.1.10
        0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x2f,0x79,0x8a,0xb1
    };

      unsigned char  ping_request[128] = {   //DuLi [2012-09-24]modify
        0x00,0x00,0x00,0x00,0x00,0x00, // dst mac
        0x00,0x00,0x00,0x00,0x00,0x00, // src mac
        0x08,0x00,	// type: IP
		0x45,0x00,0x00,0x1c,  // begin IP packet
		0x00,0x00,0x40,0x00,  // [identif(16bit)] [flag(3bit)] [(13bit)]
		0xff,0x01,0x00,0x00,  // [TTL(8bit)] [Protocol(8bit)] [checksum(16bit)] checksum 6306 -> 620e
        0x0a,0xff,0x01,0xf9,  // src ip
		0x0a,0xff,0x01,0xdc,  // dst ip
		0x08,0x00,0xf7,0xff,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00
    };

	memcpy(mymac, reinterpret_cast<struct AnkhSessionDescriptor*>(info->addr())->mac, 6);

#if 1 // DuLi add
	unsigned int checksum_val = 0;
   checksum_val = checksumcalculate((unsigned short *)(&ping_request[14]), 20);
	printf("ping_request = %x,checksum_val = %x \n",ping_request[14], checksum_val);
	ping_request[24] = (unsigned char)checksum_val & (0xff);
	ping_request[25] = checksum_val >> 8;
	printf("ping_request_checksum = %x %x",ping_request[24],ping_request[25]);
#endif

	l4_sleep(100);
	printf("MY MAC: "); print_mac(mymac); printf("\n");

    printf("myip = %u.%u.%u.%u\n",myip[0],myip[1],myip[2],myip[3]);

    printf("targetip = %u.%u.%u.%u\n",targetip[0],targetip[1],targetip[2],targetip[3]);

    memcpy(recv_buffer.txbuf,arp_request,64);
    memcpy(recv_buffer.txbuf+6,mymac,6);
    memcpy(recv_buffer.txbuf+22,mymac,6);
    memcpy(recv_buffer.txbuf+28,myip,4);
    memcpy(recv_buffer.txbuf+38,targetip,4);
    size = 64;
  	printf("\n\n\n\nARP: who has %u.%u.%u.%u ?\n",
   	          recv_buffer.txbuf[38],
   	          recv_buffer.txbuf[39],
   	          recv_buffer.txbuf[40],
   	          recv_buffer.txbuf[41]);
   	std::cout << "call send_packet(size = " << size << ")\n";
    data_format_print(recv_buffer.txbuf,size);
	send_packet(send, &recv_buffer.txbuf[0], size);

	size = bufsize;
   	std::cout << "\n\ncall recv_packet(size = " << size << ")\n";
	recv_packet(recv, &recv_buffer.rxbuf[0], &size);
    std::cout << "\n\ncall recv_packet(size = " << size << ") done\n";
   	printf("ARP: %u.%u.%u.%u is at %02X:%02X:%02X:%02X:%02X:%02X\n",
            recv_buffer.rxbuf[28],
            recv_buffer.rxbuf[29],
            recv_buffer.rxbuf[30],
            recv_buffer.rxbuf[31],
            recv_buffer.rxbuf[22],
            recv_buffer.rxbuf[23],
            recv_buffer.rxbuf[24],
            recv_buffer.rxbuf[25],
            recv_buffer.rxbuf[26],
            recv_buffer.rxbuf[27]
            );

    data_format_print(recv_buffer.rxbuf,size);

    // save target mac
    memcpy(targetmac,recv_buffer.rxbuf+22,6);

    memcpy(recv_buffer.txbuf,ping_request,64);
    memcpy(recv_buffer.txbuf,targetmac,6);
    memcpy(recv_buffer.txbuf+6,mymac,6);
    memcpy(recv_buffer.txbuf+26,myip,4);
    memcpy(recv_buffer.txbuf+30,targetip,4);
    size = 64;
  	printf("\n\n\n\nping request: %u.%u.%u.%u\n",
    	          recv_buffer.txbuf[30],
    	          recv_buffer.txbuf[31],
    	          recv_buffer.txbuf[32],
    	          recv_buffer.txbuf[33]);
  	std::cout << "call send_packet(size = " << size << ")\n";
	data_format_print(recv_buffer.txbuf,size);
	send_packet(send, &recv_buffer.txbuf[0], size);

	size = bufsize;
	std::cout << "\n\ncall recv_packet(size = " << size << ")\n";
	recv_packet(recv, &recv_buffer.rxbuf[0], &size);
	std::cout << "\n\ncall recv_packet(size = " << size << ") done\n";
	printf("ping reply: %u.%u.%u.%u\n",
			recv_buffer.rxbuf[26],
			recv_buffer.rxbuf[27],
			recv_buffer.rxbuf[28],
			recv_buffer.rxbuf[29]
		  );

	data_format_print(recv_buffer.rxbuf,size);
#if 0	
	while (1) {
		recv_packet(recv, &recv_buffer.rxbuf[0], &size);
		printf("test reply: %u.%u.%u.%u\n",
			recv_buffer.rxbuf[26],
			recv_buffer.rxbuf[27],
			recv_buffer.rxbuf[28],
			recv_buffer.rxbuf[29]
		  );

		data_format_print(recv_buffer.rxbuf,size);
	}
#endif
}


static void setup_debug_names(std::string const &name)
{
	std::cout << "I am " << name << "\n";

	std::cout << "My TID = 0x" << std::hex << l4_debugger_global_id(pthread_getl4cap(pthread_self()))
	          << std::dec << "\n";
	//std::cout << "Ready to contact Ankh.\n";
	char const *c = name.c_str();
	for ( ; *c++ != '/'; ) ;

	std::cout << c << "\n";
	l4_debugger_set_object_name(pthread_getl4cap(pthread_self()), c);

}


int main(int argc, char **argv)
{

	if (argc > 1) {
		std::cout << "Using shm area: '" << argv[1] << "'\n";
		shm_name = strdup(argv[1]);
	}

	std::cout << argv[2] << "\n"<< argv[3] << "\n";

	L4::Cap<void> ankh_session;
	if (!(ankh_session = L4Re::Env::env()->get_cap<void>("ankh")))
	{
		std::cout << "Ankh not found.\n";
		return 1;
	}
	/* get_shm_area 
	 * 1. l4shmc_attach;
	 * 2. init send,recv,info;
	 * 3. ankh_activate;
	 */
	get_shm_area(ankh_session);

	std::string name(argv[0]);
	std::cout << name << "\n";
	setup_debug_names(name);
	std::cout << "txbuf @ " << (void*)recv_buffer.txbuf << ", rxbuf @ " << (void*)recv_buffer.rxbuf << std::endl;

    printf("%s %s\n",argv[2],argv[3]);

    int myip[4],targetip[4];
    char myip_c[4],targetip_c[4];
    char str[4][16];

    sscanf(argv[2],"myip=%u.%u.%u.%u",&myip[0],&myip[1],&myip[2],&myip[3]);

    myip_c[0] = myip[0];
    myip_c[1] = myip[1];
    myip_c[2] = myip[2];
    myip_c[3] = myip[3];

    sscanf(argv[3],"targetip=%u.%u.%u.%u",&targetip[0],&targetip[1],&targetip[2],&targetip[3]);

    targetip_c[0] = targetip[0];
    targetip_c[1] = targetip[1];
    targetip_c[2] = targetip[2];
    targetip_c[3] = targetip[3];

    printf("myip = %u.%u.%u.%u\n",myip_c[0],myip_c[1],myip_c[2],myip_c[3]);
    printf("targetip = %u.%u.%u.%u\n",targetip_c[0],targetip_c[1],targetip_c[2],targetip_c[3]);

	ping(myip_c,targetip_c);

  //  send_test();

//	l4_sleep_forever();
	return 0;
}
