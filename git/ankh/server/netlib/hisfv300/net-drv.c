#include <l4/sys/cache.h>
#include <stdio.h>
#include <string.h>

#define HIETH_SFV300
#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"
#if 0 // 2012-04-18 sunray
#include <config.h>
#include <miiphy.h>
#include <net.h>
#define CONFIG_RANDOM_ETHADDR
#endif
/*************************************************************************/
int hieth_mdiobus_driver_init(void);
void hieth_mdiobus_driver_exit(void);
extern unsigned int get_phy_device(char *devname, unsigned char phyaddr);
static struct hieth_netdev_local hieth_devs[2];
int eth_init_rx(void);
int hisf_recv(void *packet, unsigned len);
void eth_halt(void);
int eth_init(unsigned char *mac);
int eth_send(volatile void *packet, int length);
int eth_rx(unsigned char *buf,unsigned int *length);
void udelay(int);
int suka_malloc(int size,unsigned int *virt,unsigned int *phys,unsigned int *handle);
int suka_free(void *virt_addr);
//int request_irq(unsigned int, irqreturn_t (*handler)(int, void *, struct pt_regs *), unsigned long, const char *, void *);



// 2012-04-18 sunray, added start
#ifndef	NULL
#define NULL 0
#endif
// 2012-04-18 sunray, added end

static struct hieth_netdev_local *hieth_curr=NULL;

/* Used when trying to connect to a specific phy (mii bus id:phy device id) */
/* refer linux-2.6.23 ./include/linux/phy.h line 76 */
char U_PHY_NAME[MAX_PHY_NAME_LEN];
char D_PHY_NAME[MAX_PHY_NAME_LEN];

/* only use for uboot */
unsigned char U_PHY_ADDR = HISFV_PHY_U;
unsigned char D_PHY_ADDR = HISFV_PHY_D;

#define MAC_LEN 6

#define print_mac(mac)	 do{ int i;\
	printf("MAC:  ");\
	for (i = 0; i < MAC_LEN; i++)\
		printf("%c%02X", i ? '-' : ' ', *(((unsigned char*)mac)+i));\
	printf("\n");\
}while(0)

#if 0 // 2012-04-18 sunray
void string_to_mac(unsigned char *mac, char* s)
{
	int i;
	char *e;

	for (i=0; i<MAC_LEN; ++i) {
		mac[i] = s ? simple_strtoul(s, &e, 16) : 0;
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}
}

static void print_mac_address(const char *pre_msg, const unsigned char *mac, const char *post_msg)
{
	int i;

	if(pre_msg)
		printf(pre_msg);

	for(i=0; i<6; i++)
		printf("%02X%s", mac[i], i==5 ? "" : ":");

	if(post_msg)
		printf(post_msg);
}

#ifdef CONFIG_RANDOM_ETHADDR
static unsigned long rstate = 1;
/* trivial congruential random generators. from glibc. */
void srandom(unsigned long seed)
{
	rstate = seed ? seed : 1;  /* dont allow a 0 seed */
}

unsigned long random(void)
{
  unsigned int next = rstate;
  int result;

  next *= 1103515245;
  next += 12345;
  result = (unsigned int) (next / 65536) % 2048;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  rstate = next;

  return result;
}

//void random_ether_addr(char *mac)
void random_ether_addr(unsigned char *mac)
{
    unsigned long ethaddr_low, ethaddr_high;

    srandom(get_timer(0) );

    /*
     * setting the 2nd LSB in the most significant byte of
     * the address makes it a locally administered ethernet
     * address
     */
    ethaddr_high = (random() & 0xfeff) | 0x0200;
    ethaddr_low = random();

    mac[0] = ethaddr_high >> 8;
    mac[1] = ethaddr_high & 0xff;
    mac[2] = ethaddr_low >>24;
    mac[3] = (ethaddr_low >> 16) & 0xff;
    mac[4] = (ethaddr_low >> 8) & 0xff;
    mac[5] = ethaddr_low & 0xff;

    mac [0] &= 0xfe;	/* clear multicast bit */
    mac [0] |= 0x02;	/* set local assignment bit (IEEE802) */

 }
#endif

#endif

#if 1  // 2012-04-17 sunray
#define ETH_GLB_REG( n )    (SUKA_IO_ADDRESS(GLB_HOSTMAC_L32 + ETH_IO_ADDRESS_BASE + n*4))
#else
//added by wzh 2009-4-9 begin
#define ETH_GLB_REG( n )    (GLB_HOSTMAC_L32 + ETH_IO_ADDRESS_BASE + n*4)
#endif


// 2012-04-17 sunray, copied from net.c
/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
static inline int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline int is_multicast_ether_addr(const u8 *addr)
{
	return (0x01 & addr[0]);
}

/**
 * is_valid_ether_addr - Determine if the given Ethernet address is valid
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * Return true if the address is valid.
 */
static inline int is_valid_ether_addr(const u8 * addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

static int set_mac_address(char * mac)
{
	unsigned char t[4] = {0};

	t[0] = mac[1];
	t[1] = mac[0];
	*(unsigned int *)ETH_GLB_REG(1) = *((unsigned int*)t);

	t[0] = mac[5];
	t[1] = mac[4];
	t[2] = mac[3];
	t[3] = mac[2];
	*(unsigned int *)ETH_GLB_REG(0) = *((unsigned int*)t);

	return 0;
}
#if 00 // 2012-04-18 sunray
static int set_random_mac_address(unsigned char * mac, unsigned char * ethaddr)
{
	random_ether_addr(mac);
	print_mac_address( "Set Random MAC address: ", mac, "\n");

	sprintf ((char *)ethaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	setenv("ethaddr",(char *)ethaddr);

	set_mac_address((char *)mac);
    return 0;
}
#endif

static int eth_set_host_mac_address(unsigned char *mac)
{
    SFE_OFF;

	print_mac(mac);
	set_mac_address((char *)mac);

    SFL;
	return 0;
}

//modified by wzh 2009-4-14 end


static void phy_print_status(struct hieth_netdev_local *ld, int stat)
{
	printf("%s : phy status change : LINK=%s : DUPLEX=%s : SPEED=%s\n", \
			(ld->port==UP_PORT) ? "UP_PORT" : "DOWN_PORT", \
			(stat & HIETH_LINKED) ? "UP" : "DOWN", \
			(stat & HIETH_DUP_FULL) ? "FULL" : "HALF", \
			(stat & HIETH_SPD_100M) ? "100M" : "10M");
}

static void hieth_adjust_link(struct hieth_netdev_local *ld)
{
	int stat = 0;
	int timeout_us = 1000;
    /*this env phy_link_time used to solve the difference phy auto-negotiation time of  various phys*/

    SFE_OFF;

    sunray_debug("phy device '%s' adjust link,please wait...\n\n",ld->phy_name); // sunray

#if 00 // 2012-04-18 sunray, deleted
    char* timeout = getenv("phy_link_time");
    if (timeout){
        timeout_us = simple_strtol(timeout,0, 10);
        if(timeout_us < 0)
            timeout_us = 1000;
    }
#endif

    {
        u16 reg = 0;
        miiphy_read(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR), PHY_ANAR, &reg);
        reg |= 0x1E0;
        miiphy_write(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR), PHY_ANAR, reg);
    }
retry:
	udelay(1);

	stat |= miiphy_link(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) ? HIETH_LINKED : 0;
	stat |= miiphy_duplex(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) == FULL ? HIETH_DUP_FULL : 0;
	stat |= miiphy_speed(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) == _100BASET ? HIETH_SPD_100M : 0;

	if( --timeout_us && !(stat & HIETH_LINKED) )
		goto retry;

	if(stat != ld->link_stat) {
        sunray_debug("phy device '%s' adjust link ok,retry %d times\n",ld->phy_name,1000-timeout_us); // sunray
		hieth_set_linkstat(ld, stat);
		phy_print_status(ld, stat);
		ld->link_stat = stat;
	}
    SFL;
}

static int hieth_net_open(struct hieth_netdev_local *ld)
{
    SFE_OFF;
    sunray_debug_off("hieth_net_open(phy='%s')\n",ld->phy_name);// 2012-04-18 sunray
	/* enable sys-ctrl-en and clk-en  */
	hieth_sys_startup();

	/* setup hardware tx dep */
	hieth_writel_bits(ld, HIETH_HW_TXQ_DEPTH, UD_REG_NAME(GLB_QLEN_SET), BITS_TXQ_DEP);

	/* setup hardware rx dep */
	hieth_writel_bits(ld, HIETH_HW_RXQ_DEPTH, UD_REG_NAME(GLB_QLEN_SET), BITS_RXQ_DEP);

	ld->link_stat = 0;
	hieth_adjust_link(ld);

	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_RX_RDY));

    SFL;
	return 0;
}

static int hieth_net_close(struct hieth_netdev_local *ld)
{
    SFE_OFF;
	hieth_glb_preinit_dummy(ld);

	hieth_sys_allstop();

    SFL;
	return 0;
}

char PHY_NAME[MAX_PHY_NAME_LEN] = "0:01"; // 2012-04-18 sunray, added

static int hieth_dev_probe_init(int port)
{
	struct hieth_netdev_local *ld = &hieth_devs[port];

    SFE_OFF;

	if( (UP_PORT != port) && (DOWN_PORT != port) )
		return -1;//-ENODEV

	ld->iobase_phys = ETH_IO_ADDRESS_BASE;

    ld->port = port;

	ld->phy_name = UD_REG_NAME(PHY_NAME);

	hieth_glb_preinit_dummy(ld);

	hieth_sys_allstop();

    SFL;
	return 0;
}

static int hieth_dev_remove(struct hieth_netdev_local *ld)
{
	(void)ld;
    SFE_OFF;
    SFL;
	return 0;
}

static int hieth_init(void)
{
	int ret = 0;

    SFE_OFF;
#if 00 // 2012-04-18 sunray, deleted
    /*get phy addr of up port*/
    char* phyaddr;
    phyaddr = getenv("phyaddru");
    if(phyaddr){
        unsigned long tmp = simple_strtoul(phyaddr, 0, 10);
        /*phy addr == 0, notify user to check wether its ok.*/
        if(tmp == 0){
            printf("Detected env 'phyaddru' had been set to 0,is it right?\n");
        }
        /*check phyaddr > 0xff*/
        else if (tmp >= (unsigned long)(~((unsigned char)0))){
            printf("Detected env 'phyaddru' had been set greater than 0x%lx,this may not correct.\n",
                    (unsigned long)(~((unsigned char)0)));
            return -1;
        }
        U_PHY_ADDR = (unsigned char)tmp;
        sprintf(U_PHY_NAME,"0:%d",U_PHY_ADDR);
    }
    else
#endif
    {
	    sprintf(U_PHY_NAME,"0:%d",U_PHY_ADDR);
    }

#if 00 // 2012-04-18 sunray, deleted
    /*get phy addr of down port*/
    phyaddr = getenv("phyaddrd");
    if(phyaddr){
        unsigned long tmp = (int)simple_strtoul(phyaddr, 0, 10);
        /*phy addr == 0, notify user to check wether its ok.*/
        if(tmp == 0){
            printf("Detected env 'phyaddrd' set to 0,is it right?\n");
        }
        /*check phyaddr > 0xff*/
        else if (tmp >= (unsigned long)(~((unsigned char)0))){
            printf("Detected env 'phyaddrd' had been set greater than 0x%lx,this may not correct.\n",
                    (unsigned long)(~((unsigned char)0)));
            return -1;
        }
        D_PHY_ADDR = (unsigned char)tmp;
        sprintf(D_PHY_NAME,"0:%d",D_PHY_ADDR);
    }
    else
#endif
    {
        sprintf(D_PHY_NAME,"0:%d",D_PHY_ADDR);
    }

	printf(OSDRV_MODULE_VERSION_STRING"\n");

	memset(hieth_devs, 0, sizeof(hieth_devs));

	/* sys-func-sel */
	hieth_sys_init();

	/* register MDIO bus to uboot */
	if(hieth_mdiobus_driver_init()) {
		goto _error_mdiobus_driver_init;
	}

	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR)){

		ret = hieth_dev_probe_init(UP_PORT);
		if(ret) {
			hieth_error("register UpEther netdevice driver failed!");
			goto _error_register_driver;
		}
	}
	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR)){

		ret = hieth_dev_probe_init(DOWN_PORT);
		if(ret) {
			hieth_error("register DownEther netdevice driver failed!");
			goto _error_register_driver;
		}
	}

    SFL;
	return ret;

_error_register_driver:
	hieth_mdiobus_driver_exit();

_error_mdiobus_driver_init:

	return -1;
}

static void hieth_exit(void)
{
    SFE_OFF;

	hieth_mdiobus_driver_exit();

	hieth_sys_exit();
    SFL;
}

/*
   2012-04-17 sunray
   allocate tx buffer
   eth_send use physical addr,so use suka_malloc()
*/

static unsigned int tx_buf_phys,tx_buf_virt,tx_buf_handle;
static unsigned int rx_buf_phys,rx_buf_virt,rx_buf_handle;
static int alloc_txrx_buffer(void)
{
    suka_malloc(2048, &tx_buf_virt, &tx_buf_phys,&tx_buf_handle);
    sunray_debug("allocate tx buffer, phys = 0x%x virt = 0x%x\n",tx_buf_phys,tx_buf_virt);
    //suka_malloc(2048, &rx_buf_virt, &rx_buf_phys,&rx_buf_handle);
    suka_malloc(2048 * 32, &rx_buf_virt, &rx_buf_phys,&rx_buf_handle); // [2012-10-24:DuLi]
    sunray_debug("allocate rx buffer, phys = 0x%x virt = 0x%x\n",rx_buf_phys,rx_buf_virt);
	return 0;
}

static int free_txrx_buffer(void)
{
    suka_free((void *)tx_buf_virt);
    suka_free((void *)rx_buf_virt);
    return 0;
}

// 2012-04-18 sunray
// mac: 6 bytes ascii
int eth_init(unsigned char *mac)
{
	int res;
	int count = 30;

    SFE_OFF;

	res = hieth_init();
	if(res)
		goto _error_hieth_init;

	eth_set_host_mac_address(mac);

retry:
	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR)){
       // 2012-04-17 sunray, code goes here
		/* open UpEther net dev */
		hieth_net_open(&hieth_devs[UP_PORT]);
		if(hieth_devs[UP_PORT].link_stat & HIETH_LINKED) {
			hieth_curr = &hieth_devs[UP_PORT];
			goto _link_ok;
		}
	}

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR)){
       // 2012-04-17 sunray, code does not go here
       // DownEther not installed on Hi3716 Ref board
		/* open DownEther net dev */
		hieth_net_open(&hieth_devs[DOWN_PORT]);
		if(hieth_devs[DOWN_PORT].link_stat & HIETH_LINKED) {
			hieth_curr = &hieth_devs[DOWN_PORT];

			goto _link_ok;
		}
	}
	if( --count )
		goto retry;

	printf("Up/Down PHY not link.\n");

_error_hieth_init:

	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
		hieth_net_close(&hieth_devs[UP_PORT]);

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
		hieth_net_close(&hieth_devs[DOWN_PORT]);

	return -1;

_link_ok:

    alloc_txrx_buffer(); // 2012-04-17 sunray

    eth_init_rx();  // 2012-05-08 sunray

    SFL;
	return 0;
}

int eth_rx(unsigned char *buf,unsigned int *length) // 2012-04-18 sunray
{
	(void)buf;
    SFE_OFF;

	int recvq_ready, timeout_us = 10000;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;

    *length = 0; // 2012-04-18 sunray

	/* check this we can add a Rx addr */
	recvq_ready = hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_RECVQ_RDY);
	if(!recvq_ready) {
		hieth_trace(7, "hw can't add a rx addr.");
	}

	/* enable rx int */
	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_RX_RDY));

	/* fill rx hwq fd */
	fd.frm_addr = (unsigned long)rx_buf_phys; // 2012-04-18 sunray
	fd.frm_len = 0;
	hw_recvq_setfd(ld, fd);

	/* receive packed, loop in NetLoop */
	while(--timeout_us && !is_recv_packet(ld))
		udelay(1);

	if( is_recv_packet(ld) ) {

		/*hwid = hw_get_rxpkg_id(ld);*/
		fd.frm_len = hw_get_rxpkg_len(ld);
		hw_set_rxpkg_finish(ld);

		if(HIETH_INVALID_RXPKG_LEN(fd.frm_len)) {
			hieth_error("frm_len invalid (%d).", fd.frm_len);
			goto _error_exit;
		}

#if 11 // 2012-04-18 sunray
        l4_cache_dma_coherent_full();
        //sunray_debug("%d bytes data received!\n",fd.frm_len);
       // memcpy(buf,rx_buf_virt,fd.frm_len);
        *length = fd.frm_len;
#else
		/* Pass the packet up to the protocol layers. */
		NetReceive ((void*)fd.frm_addr, fd.frm_len);
#endif
        SFL;
		return 0;
	}
	else {
		hieth_trace(7, "hw rx timeout.");
	}

_error_exit:
	return -1;
}


/*
   2012-04-17 sunray
   packet is phyical addr
*/
#include <l4/ankh/packet_analyzer.h>
#include "../../../lib/analyzer/etypes.h"
#include <pthread-l4.h>

int eth_send(volatile void *packet, int length)
{
	int ints, xmitq_ready, timeout_us = 3000;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;

    SFE_OFF;

#if 1
	/* 此函数的调用是为了解决发送buf拷贝不正确的问题 */
	int ret = sched_yield();
	if(ret == -1){
		printf("eth_send call ched_yield() failed\n");
		return -1;
	}
#endif

#if 0
	printf("\n------------[Send Packet length = %d]----------------------\n", length);
	int i;
	unsigned char *tp = packet;
	for (i = 1; i < length; i++) {
		printf("%.2x ", *(char *)tp);
		tp++;
		if (i % 8 == 0) printf("  ");
		if (i % 16 == 0 ) printf("\n");

	}
	printf("\n-----------------------------------------------\n");
#endif

#if 0
	eth_hdr *eth = (eth_hdr*)packet;
	l4_uint16_t packet_type = packet_analyze(packet, length);
	if (packet_type == udp_port_dns_srv) {		// DNS包,53
		l4_cache_dma_coherent_full();
		memcpy((unsigned char*)tx_buf_virt, (unsigned char*)packet,length);
	} else {
		memcpy((unsigned char*)tx_buf_virt, (unsigned char*)packet,length);
		l4_cache_dma_coherent_full();
	}
#else
    // 2012-04-17 sunray, copy data to our buf
    memcpy((void *)tx_buf_virt,(const void*)packet,length);
    l4_cache_dma_coherent_full();
#endif

    //sunray_debug("eth_send(length=%d)\n",length);
	/* check this we can add a Tx addr */
	xmitq_ready = hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_XMITQ_RDY);
	if(!xmitq_ready) {
		hieth_error("hw can't add a tx addr.");
		goto _error_exit;
	}

	/* enable tx int */
	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_TXQUE_RDY));

	/* fill tx hwq fd */
	fd.frm_addr = (unsigned long)tx_buf_phys; // 2012-04-18 sunray
	fd.frm_len = length + 4;
	hw_xmitq_setfd(ld, fd);

	do{
		udelay(1);
		ints = hieth_read_irqstatus(ld);
	}while( --timeout_us && !(ints & UD_BIT_NAME(HIETH_INT_TXQUE_RDY)) );

	hieth_clear_irqstatus(ld, ints);

	if( !timeout_us ) {
		hieth_error("hw tx timeout.");
		goto _error_exit;
	}

    SFL;
	return 0;

_error_exit:
	return -1;
}

void eth_halt(void)
{
    SFE_OFF;
	if(hieth_devs[UP_PORT].iobase_phys)
		hieth_net_close(&hieth_devs[UP_PORT]);
	if(hieth_devs[DOWN_PORT].iobase_phys)
		hieth_net_close(&hieth_devs[DOWN_PORT]);

	hieth_dev_remove(&hieth_devs[UP_PORT]);
	hieth_dev_remove(&hieth_devs[DOWN_PORT]);

	hieth_exit();

    free_txrx_buffer(); // 2012-04-17 sunray
    SFL;
}

static void irq_handler(int irq,void* data)
{
	(void)irq;
	(void)data;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;
    int int_status;

	hieth_irq_enable(ld, 0);
	//hieth_irq_disable(ld, 0);

    int_status = hieth_read_irqstatus(ld);
	sunray_debug_off("int_status = %x\n",int_status);
	l4_cache_dma_coherent_full(); // [DuLi] add
	if ( int_status &  UD_BIT_NAME(HIETH_INT_RX_RDY) )
	{
		fd.frm_len = hw_get_rxpkg_len(ld);
		hw_set_rxpkg_finish(ld);

		// 2012-04-18 sunray
		//static unsigned int count = 0;
		// sunray_debug("irq_handler[%u]: %d bytes data received!\n",count,fd.frm_len);
		// data_format_print(rx_buf_virt,fd.frm_len);
		// printf("+++\n");
		hisf_recv((void *)rx_buf_virt,fd.frm_len);
		hieth_clear_irqstatus(ld, UD_BIT_NAME(HIETH_INT_RX_RDY));
		hieth_irq_enable(ld, 0xc0001);
		/* fill rx hwq fd */
		fd.frm_addr = (unsigned long)rx_buf_phys; // 2012-04-18 sunray
		fd.frm_len = 0;
		hw_recvq_setfd(ld, fd);
	}
}

int eth_init_rx(void) // 2012-05-08 sunray
{
    SFE_OFF;

	int recvq_ready;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;

   int err;
   int setting;
	
    /* add by [2012-10-25:DuLi] */
//	hieth_set_queue_number(ld, 32);
	/* check this we can add a Rx addr */
	recvq_ready = hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_RECVQ_RDY);
	if(!recvq_ready) {
		hieth_trace(7, "hw can't add a rx addr.");
        return 1;
	}

    // 2012-05-08 sunray
    err = request_irq(51+32, irq_handler, 0,"hisf",0);
    sunray_debug_off("request_irq return %d\n",err);

    // 2012-04-18 sunray, enable upeth broadcast frame to CPU
    if(0)
	  hieth_writel_bits(ld, 1, GLB_MACTCTRL, BITS_BROAD2CPU_UP);

	/* enable rx int */
	//hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_RX_RDY_U));
	//hieth_irq_enable(ld, ~0);
    ///setting = BITS_IRQS_ENA_ALLPORT | BITS_IRQS_ENA_U
    ///                   | BITS_IRQS_U;
    setting = 0xc0001;
    sunray_debug_off("setting = 0x%x\n",setting);

	hieth_irq_enable(ld, setting);

	/* fill rx hwq fd */
	fd.frm_addr = (unsigned long)rx_buf_phys; // 2012-04-18 sunray
	fd.frm_len = 0;
	hw_recvq_setfd(ld, fd);

	return 0;
}

