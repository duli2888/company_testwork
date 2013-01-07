#if 0 // 2012-04-18 sunray
#include <config.h>
#endif
#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"

#define HIETH_SYSREG_BASE	(REG_BASE_PERI_CTRL + REG_PERI_CRG26)

static void hieth_set_regbit(ulong reg, int bit, int shift)
{
	ulong val;

	val = _readl(reg);

	bit = bit ? 1 : 0;

	val &= ~(1<<shift);
	val |= bit<<shift;

	_writel(val, reg);
}

static void hieth_reset(int rst)
{
#define RESET_SHIFT	0
    SFE_OFF;

	hieth_set_regbit(HIETH_SYSREG_BASE, rst, RESET_SHIFT);

	udelay(100);
    SFL;
}

#define hieth_clk_ena() do{ _writel(1<<8, HIETH_SYSREG_BASE); udelay(100); }while(0)
#define hieth_clk_dis() do{ _writel(~(1<<8), HIETH_SYSREG_BASE); udelay(100); }while(0)

#if 0
#define REG_FUNC_SEL 0x040
#define FUNC_SEL_SHIFT 27

static int function_sel = 0;

static void hieth_function_sel(int dev, int sel)
{
//	hieth_set_regbit(HIETH_SYSREG_BASE + REG_FUNC_SEL, sel, FUNC_SEL_SHIFT);
}

#endif

void hieth_sys_reset(void)
{
    SFE_OFF;
//	hieth_reset(1);
	hieth_reset(0);
    SFL;
}

void hieth_sys_startup(void)
{
    SFE_OFF;
	hieth_clk_ena();
	hieth_sys_reset();
    SFL;
}

void hieth_sys_allstop(void)
{
    SFE_OFF;
//	hieth_reset(1);
//	hieth_clk_dis();
    SFL;
}
#if 0
void hieth_sys_shutdown(int port)
{
//	hieth_clk_dis();
}

void hieth_sys_resume(int port)
{
//	hieth_clk_ena();
}

static void my_udelay( int us){
	volatile long long i = 0;
#define AHB_FREQ 400	/*MHZ*/
	long long sycle = (us * AHB_FREQ) / 6;
	for(; i < sycle; ){
		i++;
	}
}
#endif
void hieth_sys_init(void)
{
    SFE_OFF;

#if 0
	/* enable mdio clk */
	unsigned int val = _readl(HIETH_SYSREG_BASE + ETH_X5V100_MDIO_CLK_EN);
	_writel(val | (1<<26), HIETH_SYSREG_BASE + ETH_X5V100_MDIO_CLK_EN);
#endif

#ifdef HISFV_RESET_GPIO_EN
	unsigned int val = 0;
    /*gpiox[x] reset*/
    /*first reset,then delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val,HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
    __raw_writew(HISFV_RESET_GPIO_DATA, HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

    udelay(200000);

    /*then,cancel reset,and should delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val,HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
    __raw_writew(((!HISFV_RESET_GPIO_DATA) << HISFV_RESET_GPIO_BIT), HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

	udelay(20000);
#endif
//	function_sel = _readl(HIETH_SYSREG_BASE + REG_FUNC_SEL);

//	hieth_reset(1);
//	hieth_function_sel(0, 1);
	hieth_clk_ena();
	hieth_reset(0);
    SFL;
}

void hieth_sys_exit(void)
{
    SFE_OFF;
//	hieth_function_sel(0, function_sel & (1<<FUNC_SEL_SHIFT));
    SFL;
}


