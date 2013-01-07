#define HIETH_SFV300

#if 00 // 2012-04-18,SUNRAY, DELETED
#include <config.h>
#endif

#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"

int hieth_glb_preinit_dummy(struct hieth_netdev_local *ld)
{
    SFE_OFF;// 2012-04-18,SUNRAY,
    ///printf("phy device = '%s'\n",ld->phy_name);// 2012-04-18,SUNRAY,
	local_lock_init(ld);

	//FIXME: hieth_glb_preinit_dummy
	//HW MAX DEFAULT RX-PKT-LEN [42,1518]
	//HW MAC FILTER TABLE DISABLE

	/*soft reset*/
	hieth_writel_bits(ld, 1, GLB_SOFT_RESET, BITS_ETH_SOFT_RESET);
	udelay(1000);
	hieth_writel_bits(ld, 0, GLB_SOFT_RESET, BITS_ETH_SOFT_RESET);

	hieth_set_endian_mode(ld, HIETH_LITTLE_ENDIAN);

	hieth_set_linkstat(ld, 0);

	hieth_set_negmode(ld, HIETH_NEGMODE_CPUSET);

	/* RMII mode */
	hieth_set_mii_mode(ld, UD_BIT_NAME(HIETH_MII_RMII_MODE));

	hieth_writel_bits(ld, ~0, GLB_RW_IRQ_ENA, UD_BIT_NAME(BITS_IRQS));
	hieth_writel_bits(ld, ~0, GLB_RW_IRQ_ENA, UD_BIT_NAME(BITS_IRQS_ENA));
	hieth_writel_bits(ld, ~0, GLB_RW_IRQ_ENA, BITS_IRQS_ENA_ALLPORT);
	hieth_irq_disable(ld, ~0);

	/* init */
	hieth_writel(ld, 0, GLB_FWCTRL);
	hieth_writel(ld, 0, GLB_MACTCTRL);

	/*disable vlan func*/
	hieth_writel_bits(ld, 0, GLB_FWCTRL, BITS_VLAN_ENABLE);

	/*enable UpEther<->CPU*/
	hieth_writel_bits(ld, 1, GLB_FWCTRL, BITS_FW2CPU_ENA_UP);
	hieth_writel_bits(ld, 0, GLB_FWCTRL, BITS_FWALL2CPU_UP);
    // 2012-04-18 sunray, enable upeth broadcast frame to CPU
	hieth_writel_bits(ld, 1/*0*/, GLB_MACTCTRL, BITS_BROAD2CPU_UP);
	// [2012-10-18:DuLi] modify the 
	// hieth_writel_bits(ld, 0, GLB_MACTCTRL, BITS_BROAD2CPU_UP);
	hieth_writel_bits(ld, 1, GLB_MACTCTRL, BITS_MACT_ENA_UP);

	/*enable DownEther<->CPU and UpEther<->CPU*/
	hieth_writel_bits(ld, 1, GLB_FWCTRL, BITS_FW2CPU_ENA_DOWN);
	hieth_writel_bits(ld, 0, GLB_FWCTRL, BITS_FWALL2CPU_DOWN);
	hieth_writel_bits(ld, 1, GLB_MACTCTRL, BITS_BROAD2CPU_DOWN);
	hieth_writel_bits(ld, 1, GLB_MACTCTRL, BITS_MACT_ENA_DOWN);

	hieth_set_mac_leadcode_cnt_limit(ld, 0);

    SFL; // 2012-04-18 sunray
	return 0;
}

