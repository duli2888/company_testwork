
#include "hieth.h"
#include "mdio.h"
#if 0 // 2012-04-18 sunray
#include <config.h>
#endif
/* MDIO Bus Interface */
static struct hieth_mdio_local mdio_bus_ld;
extern unsigned char U_PHY_ADDR;
extern unsigned char D_PHY_ADDR;
extern char U_PHY_NAME[MAX_PHY_NAME_LEN];
extern char D_PHY_NAME[MAX_PHY_NAME_LEN];
static int hieth_mdiobus_read(char *devname, unsigned char addr,
		unsigned char reg, unsigned short *value)
{
	*value = hieth_mdio_read(&mdio_bus_ld, addr, reg);

	return 0;
}

static int hieth_mdiobus_write(char *devname, unsigned char addr,
		unsigned char reg, unsigned short value)
{
	hieth_mdio_write(&mdio_bus_ld, addr, reg, value);

	return 0;
}

//added by wzh 2009-4-15
unsigned int get_phy_device(char *devname, unsigned char phyaddr)
{
	u32 phy_id;
	u16 id1, id2;

    SFE_OFF;

	miiphy_read (devname, phyaddr, PHY_PHYIDR1, &id1);
	miiphy_read (devname, phyaddr, PHY_PHYIDR2, &id2);

	phy_id = (id1 & 0xffff) << 16;
	phy_id |= (id2 & 0xffff);

	/* If the phy_id is all Fs, there is no device there */
	if (0xffffffff == phy_id || 0 == phy_id)
	{
        sunray_debug_off("!!! no phy device found for '%s'!\n",devname); // 2012-04-18 sunray
        SFL;
		return -1;
	}

    sunray_debug_off("phy device found for '%s'! phy_id = 0x%x\n",devname,phy_id);// 2012-04-18 sunray
    SFL;
	return 0;
}

int hieth_mdiobus_driver_init(void)
{
    SFE_OFF;
	mdio_bus_ld.iobase_phys = ETH_IO_ADDRESS_BASE;
	mdio_bus_ld.mdio_frqdiv = ETH_MDIO_FRQDIV;

    miiphy_init();  // 2012-04-26 sunray, init miiphy before use

	hieth_mdio_init(&mdio_bus_ld);

	/* UpEther PHY init */
	miiphy_register(U_PHY_NAME, hieth_mdiobus_read, hieth_mdiobus_write);


	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
	{
		miiphy_reset(U_PHY_NAME, U_PHY_ADDR);
	    miiphy_set_current_dev(U_PHY_NAME);
	}
	/* DownEther PHY init */
	miiphy_register(D_PHY_NAME, hieth_mdiobus_read, hieth_mdiobus_write);

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
	{
		miiphy_reset(D_PHY_NAME, D_PHY_ADDR);
	    miiphy_set_current_dev(D_PHY_NAME);
	}

    miiphy_listdev(); // 2012-04-17 sunray, for debug info
    SFL;
	return 0;
}
void hieth_mdiobus_driver_exit(void)
{
    SFE_OFF;

    /*add this to avoid the first time to use eth will print 'No such device: XXXXX' message.*/
    if (!miiphy_get_current_dev())
        return;

	/* UpEther PHY exit */
	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
	{
		miiphy_reset(U_PHY_NAME, U_PHY_ADDR);
	}

	/* DownEther PHY exit */
	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
	{
		miiphy_reset(D_PHY_NAME, D_PHY_ADDR);
	}

	hieth_mdio_exit(&mdio_bus_ld);

    SFL;
}

