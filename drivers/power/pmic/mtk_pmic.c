#include <dm.h>
#include <power/pmic.h>
#include <power/mtk_pmic.h>

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "", .driver = "mtk_regulator" },
	{ },
};

static int mtk_pmic_bind(struct udevice *dev)
{
	ofnode regulators_node;
	int children;

	// TODO: sysreset

	regulators_node = dev_read_subnode(dev, "regulators");
	if (!ofnode_valid(regulators_node)) {
		debug("%s regulators subnode not found\n", dev->name);
		return -EINVAL;
	}

	children = pmic_bind_children(dev, regulators_node, pmic_children_info);
	if (!children)
		debug("%s has no children (regulators)\n", dev->name);

	return 0;
}

static u32 mt6350_vmc_voltages[] = {
	1800000,
	3300000
};

static u32 mt6350_vmch_voltages[] = {
	3000000,
	3300000
};

static struct mtk_pmic_regulator mt6350_regulators[] = {
	{
		.name = "vmc",
		.en = MTK_PMIC_REG(0x504, 1, 12),
		.vosel = MTK_PMIC_REG(0x52a, 1, 4),
		.voltages_count = 2,
		.voltages = mt6350_vmc_voltages,
	},
	{
		.name = "vmch",
		.en = MTK_PMIC_REG(0x506, 1, 14),
		.vosel = MTK_PMIC_REG(0x52c, 1, 7),
		.voltages_count = 2,
		.voltages = mt6350_vmch_voltages,
	},
	{ },
};

static struct mtk_pmic_pdata mt6350_data = {
	.regulators = mt6350_regulators,
	.regs = { },
};

static const struct udevice_id mtk_pmic_match[] = {
	/* vmc and vmch voltages are same as mt6350 */
	{ .compatible = "mediatek,mt6323", .data = (ulong)&mt6350_data },
	{ .compatible = "mediatek,mt6350", .data = (ulong)&mt6350_data },
	{ }
};

U_BOOT_DRIVER(mtk_pmic) = {
	.name = "mtk_pmic",
	.id = UCLASS_NOP,
	.of_match = mtk_pmic_match,
	.bind = mtk_pmic_bind,
};
