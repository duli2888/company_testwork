
struct net_device
{
	char	name[16];
	char	dev_name[16];
    char    dev_addr[32];
    int     mtu;
    int     (*net_open)(struct net_device *dev);
    int     (*net_close)(struct net_device *dev);
    int     (*net_send)(struct net_device *dev, char *addr, unsigned len);
};

unsigned int linux_glue_ip_addr;



#include "hisf.c"

static struct net_device *devs[1] =
   { &hisf };

#include "linux_glue.h"

#define DEBUG(DBG, msg, ...) \
	do { if (DBG) \
			printf(msg, ##__VA_ARGS__); \
	} while (0);

#define ANKH_DEBUG_INIT     1
#define ANKH_DEBUG_PACKET   0

static int ankh_debug_linuxglue = 0;
void ankh_set_debug()   { ++ankh_debug_linuxglue; }
void ankh_unset_debug() { --ankh_debug_linuxglue; }

void eth_shared_ip_addr(unsigned int ip_addr)
{
	linux_glue_ip_addr = ip_addr;
}
/*****************************************************************************
 * Open the "real" network devices. Return the number of opened interfaces.
 *****************************************************************************/
int open_network_devices(int promisc, char *mac)
{
#if 11  // 2012-05 sunray, added

	struct net_device *dev = devs[0];

    memcpy(dev->dev_addr,mac,6);

	DEBUG(ANKH_DEBUG_INIT, "opening '%s',device name '%s'\n", dev->name,dev->dev_name);

    // beam us to promiscuous mode, so that we can receive packets that
	// are not meant for the NIC's MAC address --> we need that, because
	// ORe clients have different MAC addresses
	if (promisc && netdev_set_promisc(dev) == 0)
        printf("set interface to promiscuous mode.\n");

    dev->net_open(dev);
    netdev_add(dev);
    //DEBUG(ANKH_DEBUG_INIT, "leave open_network_devices()\n");
    return 1;
#else
	struct net_device *dev;
	struct net *net;
	int err = 0, cnt = 0;

	DEBUG(ANKH_DEBUG_INIT, "%s() \n", __func__);

	for_each_net(net) {
		for_each_netdev(net, dev)
		{
			DEBUG(ANKH_DEBUG_INIT, "opening %s\n", dev->name);

			// beam us to promiscuous mode, so that we can receive packets that
			// are not meant for the NIC's MAC address --> we need that, because
			// ORe clients have different MAC addresses
			if (promisc && netdev_set_promisc(dev) == 0)
					DEBUG(ANKH_DEBUG_INIT, "set interface to promiscuous mode.\n");

			err = dev_open(dev);
			if (err)
			{
				DEBUG(ANKH_DEBUG_INIT, "error opening %s : %d\n", dev->name, err);
				return err;
			}
			else // success
				netdev_add(dev);

		cnt++;
		//xmit_lock_add(dev->name);
		}
	}

	return cnt;
#endif
}

#if 0  // 2012-05 sunray, deleted
static int net_recv_irq(struct sk_buff *skb)
{
	int i;

	/*
	 * Special case: The loopback device is handled
	 * by local packet delivery beforehand. The loopback
	 * driver however, tries to directly call the recv() routine
	 * upon xmit(), too.
	 */
	if (!strcmp(skb->dev->name, "lo"))
		return NET_RX_SUCCESS;

	/*
	 * Push eth header back, so that skb->data is the raw packet.
	 */
	skb_push(skb, ETH_HLEN);

	/*
	 * Hand to upper layers.
	 */
#if 0
	DECLARE_MAC_BUF(mac);
	printk("%s\n", print_mac(mac, skb->data));
#endif
	packet_deliver(skb->data, skb->len, skb->dev->name, 0);

	/*
	 * Need to release skb here.
	 */
	kfree_skb(skb);

	return NET_RX_SUCCESS;
}


static void _init_linux_glue(void)
{
	l4dde26_softirq_init();
	skb_init();

	l4dde26_register_rx_callback(net_recv_irq);
}
subsys_initcall(_init_linux_glue);
#endif

/*****************************************************************************
 * wrapper functions for accessing net_device members
 *****************************************************************************/

#define ND(p) ((struct net_device *)p)

int netdev_irq(void *netdev)
{
  //printf("netdev_irq() called\n");
 return 0/*ND(netdev)->irq*/; }


char const *netdev_name(void *netdev)
//{ return dev_name(&ND(netdev)->dev); }
{
  //printf("netdev_name() called\n");

 return &ND(netdev)->name; }


int netdev_set_promisc(void *netdev)
{
	int err;
#if 1
    printf("warning: netdev_set_promisc() not handled!\n");
    err = 0;
#else
	struct net_device *dev = ND(netdev);
	if ((err = dev_change_flags(dev, dev->flags | IFF_PROMISC)) != 0)
		DEBUG(ANKH_DEBUG_INIT, "%s could not be set to promiscuous mode.\n",
		                      dev->name);
#endif
	return err;
}


int netdev_unset_promisc(void *netdev)
{
	int err;
    printf("warning: netdev_unset_promisc() not handled!\n");
#if 0
	struct net_device *dev = ND(netdev);
	if ((err = dev_change_flags(dev, dev->flags & ~IFF_PROMISC)) != 0)
		DEBUG(ANKH_DEBUG_INIT, "%s could not be set to promiscuous mode.\n",
		                      dev->name);
#endif
	return err;
}


void *alloc_dmaable_buffer(unsigned size)
{
  //printf("alloc_dmaable_buffer(size = %d) called\n",size);

	return malloc(size); ///kmalloc(size, GFP_KERNEL);
}


int netdev_get_promisc(void *netdev)
{
  //printf("netdev_get_promisc() called\n");

 return 0/*ND(netdev)->flags & IFF_PROMISC*/; }


char *netdev_dev_addr(void *netdev)
{
  //printf("netdev_dev_addr() called\n");
 return ND(netdev)->dev_addr; }


int netdev_mtu(void *netdev)
{
  //printf("netdev_mtu() called\n");

 return ND(netdev)->mtu; }


int netdev_xmit(void *netdev, char *addr, unsigned len)
{
#if 1  // 2012-05 sunray, added
  int err;

  err = ND(netdev)->net_send(netdev,addr,len);

#if 0  // 2012-05 sunray, for pingpong test
{
  char mac[6];
  memcpy(mac,addr,6);
  memcpy(addr,addr+6,6);
  memcpy(addr+6,mac,6);

  packet_deliver(addr, len, &ND(netdev)->name, 0);
}
#endif
  return err;
#else
	// XXX could we pass 0 as length here? data netdev is set
	//     below anyway
	struct sk_buff *skb = alloc_skb(len, GFP_KERNEL);
	assert(skb);

	skb->data = addr; // XXX
	skb_put(skb, len);
	skb->dev  = ND(netdev);

	while (netif_queue_stopped(ND(netdev)))
		msleep(1);

	int err;
	if (ND(netdev)->netdev_ops)
		err = ND(netdev)->netdev_ops->ndo_start_xmit(skb, ND(netdev));
	else
		err = ND(netdev)->hard_start_xmit(skb, ND(netdev));

	return err;
#endif
}
