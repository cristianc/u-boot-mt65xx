#include <config.h>
#include <init.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <vsprintf.h>
#include <linux/delay.h>
#include <linux/sizes.h>
#include <fdt_support.h>
#include <linux/bitops.h>

DECLARE_GLOBAL_DATA_PTR;

static void set_armpll(void)
{
  u32 val;

  val = readl(0x10001000);
  val = 0;
  writel(val, 0x10001000);
  writel(0x8009a000, 0x10205104);

  udelay(100);
  
  val = readl(0x10001000);
  val |= 0x4;
  writel(val, 0x10001000);
}

static void uart1_gpio_init(void)
{
  /* 
	 * HACK: At some point, the preloader (our previous stage)
   * switches the aux mode of URXD1, UTXD1 pins to "MD debug"; 
   * LK switches them back early before printing the first ever log line.
   * We replicate what LK does to be able to see logs.
	 */
  u32 val;

	/* Pin 103 (URXD1) */
  val = readl(0x100053c0);
  val &= ~(0x7 << 28);
  val |=  (0x1 << 28);
  writel(val, 0x100053c0);
	/* Pin 104 (UTXD1) */
  val = readl(0x100053d0);
  val &= ~(0x7 << 0);
  val |=  (0x1 << 0); 
  writel(val, 0x100053d0);
}

static void mtk_gpt_early_init(void)
{
  /* 
   * For whatever reason, lib/time.c gives
   * "Could not initialize timer" with -EAGAIN without 
   * TIMER_EARLY functions, hence we implement init here and 
   * timer_early_get_{rate,count}() functions in mtk_timer.c
   */
  writel(GENMASK(5, 4) | BIT(1) | BIT(0), 0x10008000 + 0x40 + 0x0);
  writel(0, 0x10008000 + 0x40 + 0x4);
}

void board_debug_uart_init(void)
{
  /* This is called right before switching the mode of UART1 pins */ 
  set_armpll();
  uart1_gpio_init();

  mtk_gpt_early_init();
}

int dram_init(void)
{
  /* 
   * 512MB seems to be the norm across MT6572 devices,
   * hence that size is hardcoded in mt6572-generic.dts;
   * 1GB is the maximum DRAM size for the devices that we have supported. 
   */
  gd->ram_size = get_ram_size((long*)CFG_SYS_SDRAM_BASE, SZ_1G);
  fdt_fixup_memory((void*)gd->fdt_blob, CFG_SYS_SDRAM_BASE, gd->ram_size);
  return 0;
}

int print_cpuinfo(void)
{
	printf("SoC:   MediaTek MT6572\n");
	return 0;
}