#ifndef __MTK_PMIC_H__
#define __MTK_PMIC_H__

#include <linux/types.h>

#define MTK_PMIC_REG(_reg, _mask, _shift) \
  ((struct mtk_pmic_reg) {.reg = _reg, .mask = _mask, .shift = _shift})

struct mtk_pmic_reg {
  u32 reg;
  u32 mask;
  u32 shift;
};

struct mtk_pmic_regulator {
  char* name;
  struct mtk_pmic_reg en;
  struct mtk_pmic_reg vosel;
  u32 *voltages;
  u32 voltages_count;
};

struct mtk_pmic_regs {
  
};

struct mtk_pmic_pdata {
  struct mtk_pmic_regulator *regulators;
  struct mtk_pmic_regs regs;
};

#endif

