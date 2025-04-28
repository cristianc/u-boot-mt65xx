#include <asm/io.h>
#include <errno.h>
#include <dm.h>
#include <linux/delay.h>
#include <power/pmic.h>
#include <power/mtk_pwrap.h>

struct mtk_pwrap_priv {
	void __iomem *base;
};

static int mtk_pwrap_wacs2_wait_for_fsm(struct udevice *dev, u32 fsm)
{
	struct mtk_pwrap_priv *priv = dev_get_priv(dev);
	u32 counter = 0;
	u32 wacs2_fsm;

	while(counter <= 100){
		wacs2_fsm = (readl(priv->base + MTK_PWRAP_REG_WACS2_RDATA) >> 16) & 0x7;

		if(wacs2_fsm == fsm)
			return 0;

		if(wacs2_fsm == PWRAP_FSM_WAIT_FOR_VALID_CLR)
			writel(1, priv->base + MTK_PWRAP_REG_WACS2_VLDCLR);

		mdelay(100);
		counter++;
	}

	return -EINVAL;
}

static int mtk_pwrap_wacs2(struct udevice *dev, u8 write, u16 addr, u16 wdata)
{
	struct mtk_pwrap_priv *priv = dev_get_priv(dev);
	int ret;

	if (write & ~(0x01))
		return -EINVAL;

	ret = mtk_pwrap_wacs2_wait_for_fsm(dev, PWRAP_FSM_WAIT_FOR_IDLE);
	if(ret)
		return ret;

	u32 wacs_cmd = (write << 31) | ((addr >> 1) << 16) | wdata;
	writel(wacs_cmd, priv->base + MTK_PWRAP_REG_WACS2_CMD);

	return 0;
}

static int mtk_pwrap_reg_count(struct udevice *dev)
{
	return 0xffff;
}

static int mtk_pwrap_read(struct udevice *dev, uint reg, u8 *buffer,
			      int len)
{
	struct mtk_pwrap_priv *priv = dev_get_priv(dev);
	int ret = 0;
	
	if (len != 2)
		return -EINVAL;

	ret = mtk_pwrap_wacs2(dev, 0, reg, 0);
	if (ret)
		return ret;

	ret = mtk_pwrap_wacs2_wait_for_fsm(dev, PWRAP_FSM_WAIT_FOR_VALID_CLR);
	if (ret)
		return ret;

	u16 rdata = readl(priv->base + MTK_PWRAP_REG_WACS2_RDATA) & 0xffff;
	*(u16 *)buffer = (u16)rdata;

	ret = mtk_pwrap_wacs2_wait_for_fsm(dev, PWRAP_FSM_WAIT_FOR_IDLE);
	if (ret)
		return ret;

	return 0;
}

static int mtk_pwrap_write(struct udevice *dev, uint reg, const u8 *buffer,
			       int len)
{
	int ret = 0;
	
	if (len != 2)
		return -EINVAL;

	ret = mtk_pwrap_wacs2(dev, 1, reg, *(u16 *)buffer);
	if (ret)
		return ret;

	return 0;
}

static int mtk_pwrap_probe(struct udevice *dev)
{
	struct mtk_pwrap_priv *priv = dev_get_priv(dev);
	struct uc_pmic_priv *pmic_priv = dev_get_uclass_priv(dev);

	pmic_priv->trans_len = 2;

	priv->base = dev_read_addr_ptr(dev);
	if(!priv->base)
		return -EINVAL;

	if((readl(priv->base + MTK_PWRAP_REG_INIT_DONE2) & 1) == 0)
		return -EINVAL;

	return 0;
}

static struct dm_pmic_ops mtk_pwrap_ops = {
	.reg_count = mtk_pwrap_reg_count,
	.read = mtk_pwrap_read,
	.write = mtk_pwrap_write,
};

static const struct udevice_id mtk_pwrap_match[] = {
	{ .compatible = "mediatek,pwrap" },
	{ .compatible = "mediatek,mt6572-pwrap" },
	{ .compatible = "mediatek,mt6580-pwrap" },
	{ }
};

U_BOOT_DRIVER(mtk_pwrap) = {
	.name = "mtk_pwrap",
	.id = UCLASS_PMIC,
	.of_match = mtk_pwrap_match,
	.bind = dm_scan_fdt_dev,
	.priv_auto = sizeof(struct mtk_pwrap_priv),
	.probe = mtk_pwrap_probe,
	.ops = &mtk_pwrap_ops,
};
