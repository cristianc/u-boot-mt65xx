#include <config.h>
#include <init.h>
#include <asm/global_data.h>
//#include <linux/io.h>
#include <linux/sizes.h>
#include <asm/arch/misc.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)CFG_SYS_SDRAM_BASE, SZ_1G);
	return 0;
}

int print_cpuinfo(void)
{
	printf("SoC:   MediaTek MT6580\n");
	return 0;
}
