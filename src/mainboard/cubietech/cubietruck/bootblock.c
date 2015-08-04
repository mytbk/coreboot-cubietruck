/*
 * Minimal bootblock for Cubieboard
 * It sets up CPU clock, and enables the bootblock console.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <arch/io.h>
#include <bootblock_common.h>
#include <console/uart.h>
#include <console/console.h>
#include <delay.h>
#include <cpu/allwinner/a10/gpio.h>
#include <cpu/allwinner/a10/clock.h>
#include <cpu/allwinner/a10/dramc.h>

#define CPU_AHB_APB0_DEFAULT 		\
	 CPU_CLK_SRC_OSC24M	 	\
	 | APB0_DIV_1			\
	 | AHB_DIV_2			\
	 | AXI_DIV_1

#define GPH_STATUS_LEDS			((1 << 7) | (1 << 11) | (1 << 20) | (1 << 21))
#define GPH_LED1_PIN_NO			21
#define GPH_LED2_PIN_NO			20
#define GPH_LED3_PIN_NO			11
#define GPH_LED4_PIN_NO			7

#define GPB_UART0_FUNC			2
#define GPB_UART0_PINS			((1 << 22) | (1 << 23))

#define GPC_NAND_FUNC			5
#define GPC_NAND_PINS			0x0100ffff /* PC0 thru PC15 and PC24 */

#define GPF_SD0_FUNC			2
#define GPF_SD0_PINS			0x3f	/* PF0 thru PF5 */
#define GPH1_SD0_DET_FUNC		5

static void cubieboard_set_sys_clock(void)
{
	u32 reg32;
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	/* Switch CPU clock to main oscillator */
	write32(CPU_AHB_APB0_DEFAULT, &ccm->cpu_ahb_apb0_cfg);

	/* Configure the PLL1. The value is the same one used by u-boot
	 * P = 1, N = 16, K = 1, M = 1 --> Output = 384 MHz
	 */
	write32(0xa1005000, &ccm->pll1_cfg);

	/* FIXME: Delay to wait for PLL to lock */
	u32 wait = 1000;
	while (--wait);

	/* Switch CPU to PLL clock */
	reg32 = read32(&ccm->cpu_ahb_apb0_cfg);
	reg32 &= ~CPU_CLK_SRC_MASK;
	reg32 |= CPU_CLK_SRC_PLL1;
	write32(reg32, &ccm->cpu_ahb_apb0_cfg);

        setbits_le32(&ccm->ahb_gate0, AHB_GATE_DMA);
        write32(0xa1009911, &ccm->pll6_cfg);
        /* config AHCI */
        setbits_le32(&ccm->ahb_gate0, AHB_GATE_SATA);
        setbits_le32(&ccm->pll6_cfg, PLL6_SATA_CLK_EN);
}

static void cubieboard_setup_clocks(void)
{
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	cubieboard_set_sys_clock();
	/* Configure the clock source for APB1. This drives our UART */
	write32(APB1_CLK_SRC_OSC24M | APB1_RAT_N(0) | APB1_RAT_M(0),
		&ccm->apb1_clk_div_cfg);

	/* Configure the clock for SD0 */
	write32(SDx_CLK_GATE | SDx_CLK_SRC_OSC24M | SDx_RAT_EXP_N(0)
		| SDx_RAT_M(1), &ccm->sd0_clk_cfg);

	/* Enable clock to SD0 */
	a1x_periph_clock_enable(A1X_CLKEN_MMC0);

}

static void cubieboard_setup_gpios(void)
{
	/* Mux Status LED pins */
	gpio_set_multipin_func(GPH, GPH_STATUS_LEDS, GPIO_PIN_FUNC_OUTPUT);
	/* Turn on LED2 to let user know we're executing coreboot code */
	gpio_set(GPH, GPH_LED2_PIN_NO);

	/* Mux UART pins */
	gpio_set_multipin_func(GPB, GPB_UART0_PINS, GPB_UART0_FUNC);

	/* Mux SD pins */
	gpio_set_multipin_func(GPF, GPF_SD0_PINS, GPF_SD0_FUNC);
	gpio_set_pin_func(GPH, 1, GPH1_SD0_DET_FUNC);

	/* Mux NAND pins */
	gpio_set_multipin_func(GPC, GPC_NAND_PINS, GPC_NAND_FUNC);
}

static void cubieboard_enable_uart(void)
{
	a1x_periph_clock_enable(A1X_CLKEN_UART0);
}

static void cubietruck_raminit(void)
{
	/* parameters copied from u-boot-sunxi */
	static struct dram_para dram_para = {
            .clock = 432,
            .type = 3,
            .rank_num = 1,
            .density = 8192,
            .io_width = 16,
            .bus_width = 32,
            .cas = 9,
            .zq = 0x7f,
            .odt_en = 0,
            .size = 2048,
            .tpr0 = 0x42d899b7,
            .tpr1 = 0xa090,
            .tpr2 = 0x22a00,
            .tpr3 = 0x0,
            .tpr4 = 0x1,
            .tpr5 = 0x0,
            .emr1 = 0x4,
            .emr2 = 0x10,
            .emr3 = 0x0,
        };

	dramc_init(&dram_para);

	/* FIXME: ram_check does not compile for ARM,
	 * and we didn't init console yet
	 */
	////void *const test_base = (void *)A1X_DRAM_BASE;
	////ram_check((u32)test_base, (u32)test_base + 0x1000);
}

void bootblock_mainboard_init(void)
{
	/* A10 Timer init uses the 24MHz clock, not PLLs, so we can init it very
	 * early on to get udelay, which is used almost everywhere else.
	 */
	init_timer();

	cubieboard_setup_clocks();
	cubieboard_setup_gpios();
	cubieboard_enable_uart();

	cubietruck_raminit();
	/* turn on LED3 to show bootblock_mainboard_init() finished */
	gpio_set(GPH, GPH_LED3_PIN_NO);
}