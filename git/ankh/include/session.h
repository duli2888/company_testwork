#pragma once

__BEGIN_DECLS

struct AnkhSessionDescriptor
{
	unsigned char mac[6];
	unsigned int  mtu;
	unsigned long num_rx;
	unsigned long rx_dropped;
	unsigned long rx_bytes;
	unsigned long num_tx;
	unsigned long tx_dropped;
	unsigned long tx_bytes;
	unsigned int ip_addr; // added by DuLi:2012-12-5
};

__END_DECLS
