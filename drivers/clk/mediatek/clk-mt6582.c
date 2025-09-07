#include <dm.h>
#include <log.h>
#include <asm/arch-mediatek/reset.h>
#include <asm/io.h>
#include <dt-bindings/clock/mt6582-clk.h>
#include <linux/bitops.h>

#include "clk-mtk.h"

#define MT6582_PLL_FMAX			(1800UL * MHZ)
#define MT6582_CON0_RST_BAR		BIT(24)

#define PLL(_id, _reg, _pwr_reg, _en_mask, _flags, _pcwbits, _pd_reg,	\
	    _pd_shift, _pcw_reg, _pcw_shift) {				\
		.id = _id,						\
		.reg = _reg,						\
		.pwr_reg = _pwr_reg,					\
		.en_mask = _en_mask,					\
		.rst_bar_mask = MT6582_CON0_RST_BAR,			\
		.fmax = MT6582_PLL_FMAX,				\
		.flags = _flags,					\
		.pcwbits = _pcwbits,					\
		.pd_reg = _pd_reg,					\
		.pd_shift = _pd_shift,					\
		.pcw_reg = _pcw_reg,					\
		.pcw_shift = _pcw_shift,				\
	}

static const struct mtk_pll_data apmixed_plls[] = {
	PLL(CLK_APMIXED_ARMPLL, 0x200, 0x20c, BIT(0), 0,
			21, 0x204, 24, 0x204, 0),
	PLL(CLK_APMIXED_MAINPLL, 0x210, 0x21c, 0x78000001, HAVE_RST_BAR,
			21, 0x210, 4, 0x214, 0),
	PLL(CLK_APMIXED_UNIVPLL, 0x220, 0x22c, 0xFC000001, HAVE_RST_BAR,
			7, 0x224, 24, 0x224, 0),
	PLL(CLK_APMIXED_MMPLL, 0x230, 0x23c, BIT(0), 0,
			21, 0x230, 4, 0x234, 0),
	PLL(CLK_APMIXED_MSDCPLL, 0x240, 0x24c, BIT(0), 0,
			21, 0x240, 4, 0x244, 0),
};

static const struct mtk_fixed_clk top_fixed_clks[] = {
	FIXED_CLK(CLK_TOP_APXGPT, CLK_XTAL, 13000000),
};

#define FACTOR0(_id, _parent, _mult, _div)			\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_APMIXED)

#define FACTOR1(_id, _parent, _mult, _div)			\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_TOPCKGEN)

static const struct mtk_fixed_factor top_fixed_divs[] = {
	FACTOR0(CLK_TOP_MPLL, CLK_APMIXED_MAINPLL, 1, 1),
	FACTOR1(CLK_TOP_MPLL_D4, CLK_TOP_MPLL, 1, 4),
	FACTOR1(CLK_TOP_MPLL_D6, CLK_TOP_MPLL, 1, 6),
	FACTOR1(CLK_TOP_MPLL_D7, CLK_TOP_MPLL, 1, 7),
	FACTOR1(CLK_TOP_MPLL_D8, CLK_TOP_MPLL, 1, 8),
	FACTOR1(CLK_TOP_MPLL_D10, CLK_TOP_MPLL, 1, 10),
	FACTOR1(CLK_TOP_MPLL_D12, CLK_TOP_MPLL, 1, 12),
	FACTOR1(CLK_TOP_MPLL2, CLK_TOP_MPLL, 2, 1),
	FACTOR1(CLK_TOP_MPLL2_D2, CLK_TOP_MPLL2, 1, 2),

	FACTOR0(CLK_TOP_UPLL, CLK_APMIXED_UNIVPLL, 1, 1),
	FACTOR1(CLK_TOP_UPLL_D4, CLK_TOP_UPLL, 1, 4),
	FACTOR1(CLK_TOP_UPLL_D6, CLK_TOP_UPLL, 1, 6),
	FACTOR1(CLK_TOP_UPLL_D7, CLK_TOP_UPLL, 1, 7),
	FACTOR1(CLK_TOP_UPLL_D12, CLK_TOP_UPLL, 1, 12),
	FACTOR1(CLK_TOP_UPLL_D24, CLK_TOP_UPLL, 1, 24),
	FACTOR1(CLK_TOP_UPLL2, CLK_TOP_UPLL, 2, 1),
	FACTOR1(CLK_TOP_UPLL2_D4, CLK_TOP_UPLL2, 1, 4),
	FACTOR1(CLK_TOP_UPLL2_D8, CLK_TOP_UPLL2, 1, 8),

	FACTOR0(CLK_TOP_MSDCPLL, CLK_APMIXED_MSDCPLL, 1, 1),
	FACTOR1(CLK_TOP_MSDCPLL_D2, CLK_TOP_MSDCPLL, 1, 2),
};

static const int uart_parents[] = {
	CLK_XTAL,
	CLK_TOP_UPLL2_D8
};

static const int msdc_parents[] = {
	CLK_XTAL,
	CLK_TOP_MSDCPLL_D2,
	CLK_TOP_MPLL2_D2,
	CLK_TOP_MPLL_D4,
	CLK_TOP_UPLL_D4,
	CLK_TOP_UPLL2_D4,
};

/*static const int mmsys_pwm_parents[] = {
	CLK_XTAL,
	CLK_TOP_UPLL_D12,
};*/

static const struct mtk_composite top_muxes[] = {
	MUX(CLK_TOP_UART_SEL, uart_parents, 0x60, 0, 1),
	MUX(CLK_TOP_MSDC0_SEL, msdc_parents, 0x60, 24, 3),
	MUX(CLK_TOP_MSDC1_SEL, msdc_parents, 0x70, 0, 3),
	MUX(CLK_TOP_MSDC2_SEL, msdc_parents, 0x70, 8, 3),
	//MUX(CLK_TOP_MMSYS_PWM_SEL, mmsys_pwm_parents, 0x0, 18, 1),
	// TODO: add more muxes
};

/*static const struct mtk_gate_regs top1_cg_regs = {
	.sta_ofs = 0x50,
	.set_ofs = 0x54,
	.clr_ofs = 0x58,
};*/

static const struct mtk_gate_regs top2_cg_regs = {
	.sta_ofs = 0x60,
	.set_ofs = 0x64,
	.clr_ofs = 0x68,
};

static const struct mtk_gate_regs top3_cg_regs = {
	.sta_ofs = 0x70,
	.set_ofs = 0x74,
	.clr_ofs = 0x78,
};

#define GATE_TOPx_FLAGS(_id, _parent, _shift, _flags, _regs) {		\
		.id = _id,					\
		.parent = _parent,				\
		.regs = _regs,				\
		.shift = _shift,				\
		.flags = _flags,				\
	}

/*#define GATE_TOP0(_id, _parent, _shift) \
	GATE_TOPx_FLAGS(_id, _parent, _shift, CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN, \
			&top0_cg_regs)*/

#define GATE_TOP2(_id, _parent, _shift) \
	GATE_TOPx_FLAGS(_id, _parent, _shift, CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN, \
			&top2_cg_regs)

#define GATE_TOP3(_id, _parent, _shift) \
	GATE_TOPx_FLAGS(_id, _parent, _shift, CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN, \
			&top3_cg_regs)

static const struct mtk_gate top_cgs[] = {
	//GATE_TOP0(CLK_TOP_MMSYS_PWM, CLK_TOP_MMSYS_PWM_SEL, 0),

	GATE_TOP2(CLK_TOP_UART, CLK_TOP_UART_SEL, 7),
	GATE_TOP2(CLK_TOP_MSDC0, CLK_TOP_MSDC0_SEL, 31),
	GATE_TOP3(CLK_TOP_MSDC1, CLK_TOP_MSDC1_SEL, 7),
	GATE_TOP3(CLK_TOP_MSDC2, CLK_TOP_MSDC2_SEL, 15),
	// TODO: add more gates
};

/*static const struct mtk_gate_regs mmsys1_cg_regs = {
	.sta_ofs = 0x110,
	.set_ofs = 0x114,
	.clr_ofs = 0x118,
};*/

#define GATE_MMSYS1_FLAGS(_id, _parent, _shift, _flags) {		\
		.id = _id,					\
		.parent = _parent,				\
		.regs = &mmsys1_cg_regs,				\
		.shift = _shift,				\
		.flags = _flags,				\
	}

#define GATE_MMSYS1_XTAL(_id, _parent, _shift) \
	GATE_MMSYS1_FLAGS(_id, _parent, _shift, CLK_GATE_SETCLR | CLK_PARENT_XTAL)

/*static const struct mtk_gate mmsys_cgs[] = {
	GATE_MMSYS1_XTAL(CLK_MMSYS_PWM_MM, CLK_XTAL, 0),
	GATE_MMSYS1_XTAL(CLK_MMSYS_PWM_26M, CLK_XTAL, 1),
};*/

static const struct mtk_clk_tree mt6582_apmixed_clk_tree = {
	.xtal2_rate = 26 * MHZ,
	.plls = apmixed_plls,
};

static const struct mtk_clk_tree mt6582_topckgen_clk_tree = {
	.xtal_rate = 26 * MHZ,
	.fdivs_offs = CLK_TOP_MPLL,
	.muxes_offs = CLK_TOP_UART_SEL,
	.fclks = top_fixed_clks,
	.fdivs = top_fixed_divs,
	.muxes = top_muxes,
};

static int mt6582_apmixedsys_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt6582_apmixed_clk_tree);
}

static int mt6582_topckgen_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt6582_topckgen_clk_tree);
}

static int mt6582_topckgen_cg_probe(struct udevice *dev)
{
	return mtk_common_clk_gate_init(dev, &mt6582_topckgen_clk_tree, top_cgs);
}

/*static int mt6572_mmsys_cg_probe(struct udevice *dev)
{
	return mtk_common_clk_gate_init(dev, &mt6572_clk_tree, mmsys_cgs);
}*/

static const struct udevice_id mt6582_apmixed_compat[] = {
	{ .compatible = "mediatek,mt6582-apmixedsys" },
	{ }
};

static const struct udevice_id mt6582_topckgen_compat[] = {
	{ .compatible = "mediatek,mt6582-topckgen" },
	{ }
};

static const struct udevice_id mt6582_topckgen_cg_compat[] = {
	{ .compatible = "mediatek,mt6582-topckgen-cg" },
	{ }
};

/*static const struct udevice_id mt6572_mmsys_cg_compat[] = {
	{ .compatible = "mediatek,mt6572-mmsys-cg" },
	{ }
};*/

U_BOOT_DRIVER(mtk_clk_apmixedsys) = {
	.name = "mt6582-clock-apmixedsys",
	.id = UCLASS_CLK,
	.of_match = mt6582_apmixed_compat,
	.probe = mt6582_apmixedsys_probe,
	.priv_auto	= sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_apmixedsys_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_topckgen) = {
	.name = "mt6582-clock-topckgen",
	.id = UCLASS_CLK,
	.of_match = mt6582_topckgen_compat,
	.probe = mt6582_topckgen_probe,
	.priv_auto	= sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_topckgen_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_topckgen_cg) = {
	.name = "mt6582-topckgen-cg",
	.id = UCLASS_CLK,
	.of_match = mt6582_topckgen_cg_compat,
	.probe = mt6582_topckgen_cg_probe,
	.priv_auto	= sizeof(struct mtk_cg_priv),
	.ops = &mtk_clk_gate_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

/*U_BOOT_DRIVER(mtk_clk_mmsys_cg) = {
	.name = "mt6572-mmsys-cg",
	.id = UCLASS_CLK,
	.of_match = mt6572_mmsys_cg_compat,
	.probe = mt6572_mmsys_cg_probe,
	.priv_auto	= sizeof(struct mtk_cg_priv),
	.ops = &mtk_clk_gate_ops,
	.flags = DM_FLAG_PRE_RELOC,
};*/
