#include "hisfv300/hieth.h"
#include <stdio.h>
#include "../../include/shared_ip_addr.h"
#include "../../lib/analyzer/etypes.h" //added by DuLi,2012-12-3
#include <l4/ankh/dldebug.h>
#ifdef HISF_TEST
// test code start

void data_format_print(unsigned char *buf,unsigned int length)
{
    int i;
    unsigned int line = 0,len;

    printf("\n===========================================================\n");
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
    printf("===========================================================\n");
}


static int send_mac_packet_test(void)
{
    unsigned char arp_request[128] = {
        0xff,0xff,0xff,0xff,0xff,0xff, // dst mac
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // src mac
        0x08,0x06, // type: arp
        0x00,0x01, // hw type: ethernet
        0x08,0x00, // protocol type: IP
        0x06,      // hw size
        0x04,      // protocol size
        0x00,0x01, // opcode: request
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // sender mac
        0x0a,0xff,0x01,0xd3,   // sender ip: 10.255.1.211
        0x00,0x00,0x00,0x00,0x00,0x00, // target mac
        0x0a,0xff,0x01,0x0a,   // target ip: 10.255.1.10
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00
    };

    int i,j;
    int arp_request_length = 60;

    printf("\nsend arp_request...\n");
    for(i=0;i<100;i++)
    {
        udelay(100000);
        arp_request[41]++;
        //data_format_print(arp_request,arp_request_length);
        eth_send(arp_request,arp_request_length);
    }
    printf("done!\n\n");

    return 0;
}

static int is_same_mac(unsigned char* mac1,unsigned char* mac2)
{
   int i;
   for(i=0;i<6;i++)
   {
       if (mac1[i] != mac2[i])
           return 0;
   }
   return 1;
}
static int arp_test(void)
{
    unsigned char mac[6] = {0xc2,0x83,0x51,0xc6,0x1a,0x64};

    unsigned char arp_request[128] = {
        0xff,0xff,0xff,0xff,0xff,0xff, // dst mac
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // src mac
        0x08,0x06, // type: arp
        0x00,0x01, // hw type: ethernet
        0x08,0x00, // protocol type: IP
        0x06,      // hw size
        0x04,      // protocol size
        0x00,0x01, // opcode: request
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // sender mac
        0x0a,0xff,0x01,0xd3,   // sender ip: 10.255.1.211
        0x00,0x00,0x00,0x00,0x00,0x00, // target mac
        0x0a,0xff,0x01,0x0a,   // target ip: 10.255.1.10
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00
    };

    unsigned char arp_reply[128] = {
        0x00,0x1f,0x16,0x23,0x10,0xbf, // dst mac
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // src mac
        0x08,0x06, // type: arp
        0x00,0x01, // hw type: ethernet
        0x08,0x00, // protocol type: IP
        0x06,      // hw size
        0x04,      // protocol size
        0x00,0x02, // opcode: reply
        0xc2,0x83,0x51,0xc6,0x1a,0x64, // sender mac
        0x0a,0xff,0x01,0xd3,   // sender ip: 10.255.1.211
        0x00,0x1f,0x16,0x23,0x10,0xbf, // target mac
        0x0a,0xff,0x01,0x0a,   // target ip: 10.255.1.10
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00
    };

    int arp_request_length = 60;
    int arp_reply_length = 42;
    unsigned char buf[2048];
    unsigned int length;
    int i,j;

#if 0
    for(i=0;i<100;i++)
{
    if (!eth_rx(buf,&length))
    {
        if (is_same_mac(buf,mac))
        {
            eth_send(arp_reply,arp_reply_length);
            printf("\nrecv arp OK!\n");
            data_format_print(buf,length);
            break;
        }
    }
    else
        printf("\nrecv arp timeout!\n\n");
}
#endif

#if 1
    for(j=0;j<5;j++)
{
    udelay(1000000);

    printf("\nsend arp request...\n");
    data_format_print(arp_request,arp_request_length);
    for(i=0;i<3;i++)
    {
        udelay(10000);
        arp_request[arp_request_length-1] = i+1;
        eth_send(arp_request,arp_request_length);
    }

    //printf("\nrecv arp...");
    for(i=0;i<100;i++)
    {
        if (!eth_rx(buf,&length))
        {
            if (is_same_mac(buf,mac))
            {
              printf("\nrecv arp OK!\n");
              data_format_print(buf,length);
              break;
            }
        }
        else
            printf("\nrecv arp timeout!\n\n");
    }
}
#endif
    return 0;
}

static int recv_mac_packet_test(void)
{
    unsigned char buf[2048];
    unsigned int length;

    printf("\nrecv mac frame test...");
    if (!eth_rx(buf,&length))
        data_format_print(buf,length);
    else
        printf("timeout!\n\n");
    return 0;
}
// test code end
#endif

#if 1 // [DuLi] reason: struct net_device type

// open hisf device
int hisf_open(struct net_device *dev)
{
    printf("\n-== Hisf eth driver build 2012-05-03,sunraylee@msn.com ==-\n");

    eth_init((unsigned char *)&dev->dev_addr[0]);

#ifdef HISF_TEST
    send_mac_packet_test();
    recv_mac_packet_test();
    arp_test();
#endif
    return 0;
}
#endif
int hisf_close(struct net_device *dev)
{
  (void)dev;
  return 0;
}

int hisf_send(struct net_device *dev,char *addr, unsigned len)
{
  (void)dev;
  return eth_send(addr,len);
}

#if 1
static struct net_device hisf =
{
	"lo",  // name
	"hisf",  // dev_name
	{0x14,0xE6,0xE4,0x3C,0x33,0xA0}, // dev_addr, mac
	1024, // mtu
	hisf_open,  // open func
	hisf_close,  // close func
	hisf_send  // send func
};
#endif

struct packet_icmp{
	eth_hdr eth;
	ip_hdr ip;
	icmp_hdr icmp;
};

static unsigned short checksumcalculate(unsigned short *szBUF,int iSize)
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


// 2012-05-29 sunray [2012-10-10:DuLi] modify
#if 0
#define ARP_ENTRY_LENGTH 32
#define ARP_ENTRY_TIME ARP_ENTRY_LENGTH

struct arp_entry arp_table[ARP_ENTRY_LENGTH];
static int arp_len_point = 0;

mac_addr * find_arp_table(ip_addr *ip, mac_addr *mac)
{
	int i = 0;
	for (i = 0; i < ARP_ENTRY_LENGTH; i++) {
		if (memcmp(ip, (arp_table + i), 4) == 0) {
			mac = &arp_table[i].mac;
			return &arp_table[i].mac;
		}
	}
	mac = NULL;
	return NULL;
}


int delete_arp_old()
{

	arp_len_point++;
	if (arp_len_point >= 4) {
		arp_len_point = 0;
	}
#if 0
	for (i = 0; i < ARP_ENTRY_LENGTH; i++) {
		arp_table[i].time--;
		if (arp_table[i].time <= 0)	{
			
		}
	}
#endif
}


int add_arp_table(ip_addr *ip, mac_addr *mac)
{
	
	struct arp_entry *_start = arp_table;
	memcpy(&(_start + arp_len_point)->ip, ip, 4);
	memcpy(&(_start + arp_len_point)->mac, mac, 6);
	if (arp_len_point >= 32){
		delete_arp_old();
	}
}

int delete_arp_table(ip_addr *ip, mac_addr *mac)
{

}



int do_arp_broadcast(ip_addr *ip, mac_addr *mac)
{
	unsigned char arp_request[128] = {
	    0xff,0xff,0xff,0xff,0xff,0xff, // dst mac
		0x14,0xE6,0xE4,0x3C,0x33,0xA0, // src mac
	    0x08,0x06, // type: arp
	    0x00,0x01, // hw type: ethernet
	    0x08,0x00, // protocol type: IP
	    0x06,      // hw size
	    0x04,      // protocol size
	    0x00,0x01, // opcode: request
		0x14,0xE6,0xE4,0x3C,0x33,0xA0, // sender mac
		0x0a,0xff,0x01,0xf9,   // sender ip: 10.255.1.211
	    0x00,0x00,0x00,0x00,0x00,0x00, // target mac
		0x00,0x00,0x00,0x00,   // target ip: 10.255.1.10
	};
	memcpy(&arp_request[39], ip, 4);
	eth_send(arp_request,42);

	return 0;
}

int arp_reply()
{

}

int ping_reply(ip_addr *dst_ip, mac_addr *dst_mac)
{

}


#endif  // End add by [DuLi]

#if 1
void do_arp_reply(void *packet)
{
	static unsigned char arp_reply[128] = {                                 
		0x00,0x00,0x00,0x00,0x00,0x00, // dst mac                           
		0x14,0xE6,0xE4,0x3C,0x33,0xA0, // src mac                           
		0x08,0x06, // type: arp                                             
		0x00,0x01, // hw type: ethernet                                     
		0x08,0x00, // protocol type: IP                                     
		0x06,      // hw size                                               
		0x04,      // protocol size                                         
		0x00,0x02, // opcode: reply                                         
		0x14,0xE6,0xE4,0x3C,0x33,0xA0, // sender mac                        
		0x0a,0xff,0x01,0xf9,   // sender ip: 10.255.1.211                   
		0x00,0x00,0x00,0x00,0x00,0x00, // target mac                        
		0x0a,0xff,0x01,0xd2,   // target ip: 10.255.1.10                    
	};       

	/* mac address */
	arp_reply[0] = arp_reply[32] = *((char *)packet + 6);
	arp_reply[1] = arp_reply[33] = *((char *)packet + 7);
	arp_reply[2] = arp_reply[34] = *((char *)packet + 8);
	arp_reply[3] = arp_reply[35] = *((char *)packet + 9);
	arp_reply[4] = arp_reply[36] = *((char *)packet + 10);
	arp_reply[5] = arp_reply[37] = *((char *)packet + 11);

	/* ip address */
	arp_reply[38] = *((char *)packet + 28);                                                                                                  
	arp_reply[39] = *((char *)packet + 29);
	arp_reply[40] = *((char *)packet + 30);
	arp_reply[41] = *((char *)packet + 31);
	printf("[ARP packet][Target IP] = %d\n",*((unsigned char *)packet + 41));
	eth_send(arp_reply,42);

	return;
}
#endif

#if 1
void do_ping_reply(void *packet)
{
	eth_hdr *e = (eth_hdr *)packet;
	static unsigned long icmp_count = 0;
	unsigned char  ping_reply[128] = {   //DuLi [2012-09-24]modify
		0xd0,0x67,0xe5,0x1e,0xf8,0x82, // dst mac
		0x14,0xE6,0xE4,0x3C,0x33,0xA0, // src mac
		0x08,0x00,  // type: IP
		0x45,0x00,0x00,0x54,  // begin IP packet [length (16bit)] 
		0x00,0x00,0x40,0x00,  // [identif(16bit)] [flag(3bit)] [(13bit)]
		0xff,0x01,0x00,0x00,  // [TTL(8bit)] [Protocol(8bit)] [checksum(16bit)] checksum 6306     -> 620e
		0x0a,0xff,0x01,0xf9,  // src ip
		0x0a,0xff,0x01,0xb7,  // dst ip
		0x00,0x00, // begin ICMP packet type(8 bit) code (8 bit)
		0x00,0x00, // checksum (16 bit)
		0x00,0x00, // Indentifier (16 bit)
		0x00,0x00, // Sequece number (16 bit)
		0x00,0x00,0x00,0x00, // icmp_otime
		0x00,0x00,0x00,0x00, // icmp_rtime
		0x00,0x00,0x00,0x00, // icmp_ttime
		0x00,0x00,0x00,0x00,0x00,0x00
	};
	struct packet_icmp * reply = (struct packet_icmp *)ping_reply;

	((eth_hdr *)ping_reply)->dst = e->src; // modify MAC addr
	// modify IP addr
	ping_reply[30] = *((char *)packet + 26);
	ping_reply[31] = *((char *)packet + 27);
	ping_reply[32] = *((char *)packet + 28);
	ping_reply[33] = *((char *)packet + 29);

	reply->icmp.indentifier = ((struct packet_icmp *)packet)->icmp.indentifier;
	reply->icmp.sequence = ((struct packet_icmp *)packet)->icmp.sequence;
	unsigned char type_bit0 = *((char *)packet + 14 + 20 );
	unsigned char type_bit1 = *((char *)packet + 14 + 21 );

	ping_reply[38] = *((char *)packet + 38);
	ping_reply[39] = *((char *)packet + 39);

	ping_reply[40] = *((char *)packet + 40);
	ping_reply[41] = *((char *)packet + 41);

	/* IP checksum */
	unsigned int checksum_val = 0;
	checksum_val = checksumcalculate((unsigned short *)(&ping_reply[14]), 20);
	ping_reply[24] = (unsigned char)checksum_val & (0xff);
	ping_reply[25] = checksum_val >> 8;

	/* data segament */
	//  memcpy(ping_reply + 42, e + 42, 56);
	int i;
	for (i = 0; i < 56; i++ ){
		*(ping_reply + 42 + i) = *((unsigned char *)packet + 42 + i); 
	}   

	/* ICMP checksum */
	checksum_val = checksumcalculate((unsigned short *)(&ping_reply[34]), 64);
	ping_reply[36] = (unsigned char)checksum_val & (0xff);
	ping_reply[37] = checksum_val >> 8;

	if ((type_bit0 == 0x08) && (type_bit1 == 0x00)){
		icmp_count++;
		printf("[icmp_count] = %lu\n",icmp_count);
		eth_send(ping_reply, 98);
	}
	//	eth_send(ping_reply, 98);

	return;
}
#endif


void * ip_jump_next_layer(ip_hdr * ip_packet)
{
	ip_hdr *ip = ip_packet;
	void *next_layer = NULL;
	l4_uint8_t hl = ip->ver_hlen;
	hl = hl & 0xF; 
	hl = hl * 4;
	next_layer = (char *)ip + hl;
	return next_layer;
		
}

#define DISCARD_PACKET -1  // 返回值，将此包丢掉
// packet_deliver()	函数是将网卡接收到的帧发送给发给client端
// 支持协议有
// IP (Internet Protocol) including packet forwarding over multiple network interfaces
// ICMP (Internet Control Message Protocol) for network maintenance and debugging
// IGMP (Internet Group Management Protocol) for multicast traffic management    !!!!
// UDP (User Datagram Protocol) including experimental UDP-lite extensions
// TCP (Transmission Control Protocol) with congestion control, RTT estimation
//          and fast recovery/fast retransmit
// Specialized raw/native API for enhanced performance
// Optional Berkeley-like socket API 
// DNS (Domain names resolver)								
// SNMP (Simple Network Management Protocol)				!!!!
// DHCP (Dynamic Host Configuration Protocol)				!
// AUTOIP (for IPv4, conform with RFC 3927)					!!!!
// PPP (Point-to-Point Protocol)							!!!!
// ARP (Address Resolution Protocol) for Ethernet			!!!!
int hisf_recv(void *packet, unsigned len)
{
	unsigned char my_ip[4]; 
	my_ip[3] = (linux_glue_ip_addr >> 24) & 0xFF; 
	my_ip[2] = (linux_glue_ip_addr >> 16) & 0xFF;
	my_ip[1] = (linux_glue_ip_addr >> 8) & 0xFF;
    my_ip[0] = (linux_glue_ip_addr >> 0) & 0xFF;

	eth_hdr *e = (eth_hdr *)packet;
	ip_hdr *ip = (ip_hdr *)((char *)e + ETHERTYPE_OFFSET);
	udp_hdr *tmp_udp = (udp_hdr *)((char *)e + ETHERTYPE_OFFSET);
	unsigned char broadcast_ip[4] ;//= {0xFF, 0xFF ,0xFF ,0xFF};
	broadcast_ip[0] = 0xFF;  
	broadcast_ip[1] = 0xFF;
	broadcast_ip[2] = 0xFF;
	broadcast_ip[3] = 0xFF;
	// 网卡支持单播非自己的包不接收，所以不需要在中断响应函数中做过滤
	DLPRINTF("[IP]%d:%d:%d:%d\n",my_ip[0],my_ip[1],my_ip[2],my_ip[3]);
	switch (ntohs(e->type)) {
		case ETHERTYPE_ARP: // ARP support
			if (memcmp((packet + 38), &my_ip, 4) != 0) {      // 只是接收发给自己IP地址的ARP请求作响应
				return DISCARD_PACKET;
			}
			return packet_deliver(packet,len,(char const *const)&hisf.name,0); 
			break;
		case ETHERTYPE_IP: // IP support
			if ((*((char *)e + ETHERTYPE_OFFSET) >> 4) != FILTER_IPV4) {    // IPv4版本的判断，非IPv4的丢掉
				DLPRINTF("This packet is not for IPV4,filtered\n");
				return DISCARD_PACKET;
			}
			switch(ip->proto)
			{
				case ip_proto_icmp:
				case ip_proto_tcp:
					if((memcmp(((unsigned char *)packet + 30), my_ip, 4) == 0)){   // 帧的目的IP地址是自己的接收，否则扔掉
#if 0
						printf("\n==========================================\n");
						int i;
						char *tp = packet;
						for (i = 1; i < 65; i++) {
							printf("%.2x ", *(char *)tp);
							tp++;
							if (i % 8 == 0) printf("  ");
							if (i % 16 == 0 ) printf("\n");
						}   
						printf("\n==========================================\n");
#endif
						return packet_deliver(packet,len,(char const *const)&hisf.name,0);
					} else
						return DISCARD_PACKET;
					break;
				case ip_proto_udp:
					// 现在支持UDP的DHCP和DNS	
					tmp_udp = (udp_hdr *)ip_jump_next_layer(ip);
	//					printf("tmp_udp->dst_port = %d\n",ntohs(tmp_udp->dst_port));  // This is for test
#if 0
						printf("\n==========================================\n");
						int i;
						char *tp = packet;
						for (i = 1; i < 65; i++) {
							printf("%.2x ", *(char *)tp);
							tp++;
							if (i % 8 == 0) printf("  ");
							if (i % 16 == 0 ) printf("\n");
						}   
						printf("\n==========================================\n");
#endif

#if 0				// 对于端口不作控制，如果有多个应用的话，可能会有很多自定义的端口，所以在这里无法获取到应用层给定的端口
					switch (ntohs(tmp_udp->dst_port)) {
						case udp_port_dns_srv:		// DNS support
						case udp_port_bootp_clnt:  // DHCP support
						//case udp_port_bootp_srv:   // DHCP support
						case udp_port_tftp_clnt:   // TFTP support
							if((memcmp(((unsigned char *)packet + 30), my_ip, 4) == 0)){    // 帧的目的IP是自己地址的接收
								return packet_deliver(packet,len,(char const *const)&hisf.name,0); 
							}
							if (memcmp((unsigned char *)packet + 30, broadcast_ip, 4) == 0) {  // 帧的目的IP地址是广播的接收
								return packet_deliver(packet,len,(char const *const)&hisf.name,0); 
							}
							break;
						default:
							break;
					}
#endif
#if 1
					if((memcmp(((unsigned char *)packet + 30), my_ip, 4) == 0)){    // 帧的目的IP是自己地址的接收
						return packet_deliver(packet,len,(char const *const)&hisf.name,0); 
					}
					if (memcmp((unsigned char *)packet + 30, broadcast_ip, 4) == 0) {  // 帧的目的IP地址是广播的接收
						return packet_deliver(packet,len,(char const *const)&hisf.name,0); 
					}
#endif

					break;
				default:
					return DISCARD_PACKET;
			}
			break;
		default:
			DLPRINTF("This packet is filtered[TYPE]-----%d \n",e->type);
			return DISCARD_PACKET;
	}
	return 0;
}

