#include <dm.h>
#include <log.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/mtk_pmic.h>

struct mtk_regulator_priv {
  struct udevice *pwrap;
  struct mtk_pmic_regulator regulator;
};

static int mtk_regulator_probe(struct udevice *dev)
{
  struct udevice *pmic;
  struct dm_regulator_uclass_plat *uc_pdata;
  struct mtk_pmic_pdata *pmic_pdata;
  struct mtk_regulator_priv *priv;
  
  uc_pdata = dev_get_uclass_plat(dev);
  priv = dev_get_priv(dev);
  
  pmic = dev_get_parent(dev);
  pmic_pdata = (void*)dev_get_driver_data(pmic);
  priv->pwrap = dev_get_parent(pmic);

  for(int i = 0; 1; i++){
    // end of list
    if(pmic_pdata->regulators[i].name == NULL)
      return -EINVAL;

    if(!strcmp(pmic_pdata->regulators[i].name, uc_pdata->name)){
      priv->regulator = pmic_pdata->regulators[i];
      break;
    }
  }

  return 0;
}

static int mtk_regulator_get_enable(struct udevice *dev)
{
  struct mtk_regulator_priv *priv = dev_get_priv(dev);
  struct mtk_pmic_regulator regulator = priv->regulator;
  struct mtk_pmic_reg en_reg = regulator.en;

  int ret = pmic_reg_read(priv->pwrap, en_reg.reg);

  if(ret < 0)
    return ret;

  return ((ret >> en_reg.shift) & en_reg.mask) == en_reg.mask;
}

static int mtk_regulator_set_enable(struct udevice *dev, bool enable)
{
  struct mtk_regulator_priv *priv = dev_get_priv(dev);
  struct mtk_pmic_regulator regulator = priv->regulator;
  struct mtk_pmic_reg en_reg = regulator.en;

  return pmic_clrsetbits(priv->pwrap, en_reg.reg,
    en_reg.mask << en_reg.shift,
    enable ? (en_reg.mask << en_reg.shift) : 0);
}

int mtk_regulator_get_value(struct udevice *dev)
{
  struct mtk_regulator_priv *priv = dev_get_priv(dev);
  struct mtk_pmic_regulator regulator = priv->regulator;
  struct mtk_pmic_reg vosel_reg = regulator.vosel;

  int ret = pmic_reg_read(priv->pwrap, vosel_reg.reg);

  if (ret < 0)
    return ret;

  ret = (ret >> vosel_reg.shift) & vosel_reg.mask;
  if (ret > regulator.voltages_count)
    return -EINVAL;

  return regulator.voltages[ret];
}

int mtk_regulator_set_value(struct udevice *dev, int uV)
{
  struct mtk_regulator_priv *priv = dev_get_priv(dev);
  struct mtk_pmic_regulator regulator = priv->regulator;
  struct mtk_pmic_reg vosel_reg = regulator.vosel;

  for(int i = 0; i < regulator.voltages_count; i++)
  {
    if(regulator.voltages[i] == uV){
      return pmic_clrsetbits(priv->pwrap, vosel_reg.reg,
        vosel_reg.mask << vosel_reg.shift,
        i << vosel_reg.shift);
    }
  }
  return -EINVAL;
}

struct dm_regulator_ops mtk_regulator_ops = {
  .get_enable = mtk_regulator_get_enable,
  .set_enable = mtk_regulator_set_enable,
  
  .get_value = mtk_regulator_get_value,
	.set_value = mtk_regulator_set_value,
};

U_BOOT_DRIVER(mtk_regulator) = {
  .name = "mtk_regulator",
  .id = UCLASS_REGULATOR,
  .ops = &mtk_regulator_ops,
  .probe = mtk_regulator_probe,
  .priv_auto = sizeof(struct mtk_regulator_priv),
};
