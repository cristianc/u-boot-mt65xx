#include <dm.h>
#include <clk.h>
#include <errno.h>
#include <led.h>
#include <asm/io.h>

#define PERIOD_MAX 0x1000
#define LEVEL_MAX 0x2000

#define DISP_PWM_EN_OFF 0x0
#define DISP_PWM_COMMIT_OFF 0x8
#define DISP_PWM_CON_0_OFF 0x10
#define DISP_PWM_CON_1_OFF 0x14

struct led_mtk_pwm_priv {
	void __iomem *regs;
	struct clk_bulk clks;
	u32 period;
	u32 level;
};

static enum led_state_t led_mtk_pwm_get_state(struct udevice *dev)
{
	struct led_mtk_pwm_priv *priv = dev_get_priv(dev);
	
	return readl(priv->regs + DISP_PWM_EN_OFF) & 1 ? LEDST_ON : LEDST_OFF;
}

static int led_mtk_pwm_set_state(struct udevice *dev, enum led_state_t state)
{
	struct led_mtk_pwm_priv *priv = dev_get_priv(dev);

	writel(0, priv->regs + DISP_PWM_COMMIT_OFF);

	if(state == LEDST_TOGGLE)
		state = !led_mtk_pwm_get_state(dev);

	switch (state) {
		case LEDST_OFF:
		case LEDST_ON:
			state = (state == LEDST_ON) ? 1 : 0;
			break;
		default:
			return -ENOSYS;
	}

	writel(state, priv->regs + DISP_PWM_EN_OFF);

	writel(1, priv->regs + DISP_PWM_COMMIT_OFF);
	writel(0, priv->regs + DISP_PWM_COMMIT_OFF);

	return 0;
}


static int led_mtk_pwm_probe(struct udevice *dev)
{
	struct led_mtk_pwm_priv *priv = dev_get_priv(dev);
	int err;

	priv->regs = dev_read_addr_ptr(dev);
	if(!priv->regs)
		return -EINVAL;

	priv->period = dev_read_u32_default(dev, "period", 255);
	if (priv->period >= PERIOD_MAX)
		return -EINVAL;

	priv->level = dev_read_u32_default(dev, "level", 255);
	if (priv->level >= LEVEL_MAX)
		return -EINVAL;

	err = clk_get_bulk(dev, &priv->clks);
	if (err)
		return err;

	err = clk_enable_bulk(&priv->clks);
	if (err)
		return err;

	writel(0, priv->regs + DISP_PWM_COMMIT_OFF);

	writel(0 << 16, priv->regs + DISP_PWM_CON_0_OFF);
	writel((priv->level << 16) | priv->period, priv->regs + DISP_PWM_CON_1_OFF);

	writel(0, priv->regs + DISP_PWM_EN_OFF);

	writel(1, priv->regs + DISP_PWM_COMMIT_OFF);
	writel(0, priv->regs + DISP_PWM_COMMIT_OFF);

	return 0;
}

static const struct led_ops led_mtk_pwm_ops = {
	.set_state	= led_mtk_pwm_set_state,
	.get_state	= led_mtk_pwm_get_state,
};

static const struct udevice_id led_mtk_pwm_ids[] = {
	{ .compatible = "mediatek,display_pwm" },
	{ }
};

U_BOOT_DRIVER(led_mtk_pwm) = {
	.name	= "led_mtk_pwm",
	.id	= UCLASS_LED,
	.ops	= &led_mtk_pwm_ops,
	.of_match = led_mtk_pwm_ids,
	.priv_auto	= sizeof(struct led_mtk_pwm_priv),
	.probe	= led_mtk_pwm_probe,
};
