#ifndef __CONFIG_MT6572_H
#define __CONFIG_MT6572_H

#include <linux/sizes.h>

#define CFG_SYS_SDRAM_BASE		0x80000000
#define CFG_SYS_SDRAM_SIZE    0x20000000

#define CFG_EXTRA_ENV_SETTINGS "stdin=serial,button-kbd\0" \
	"stdout=vidconsole,serial\0" \
	"stderr=vidconsole,serial\0" \
	"bootdelay=-1"

#endif /* ifdef __CONFIG_MT6572_H */
