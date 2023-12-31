/*
 * Copyright (C) 2020 UNISOC Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "sprd_dsi.h"

static struct clk *clk_ap_ahb_dsi_eb;

static struct dsi_glb_context {
	unsigned int ctrl_reg;
	unsigned int ctrl_mask;

	struct regmap *regmap;
} ctx_reset;

static int dsi_glb_parse_dt(struct dsi_context *ctx,
				struct device_node *np)
{
	unsigned int syscon_args[2];

	clk_ap_ahb_dsi_eb =
		of_clk_get_by_name(np, "clk_ap_ahb_dsi_eb");
	if (IS_ERR(clk_ap_ahb_dsi_eb)) {
		pr_warn("read clk_ap_ahb_dsi_eb failed\n");
		clk_ap_ahb_dsi_eb = NULL;
	}

	ctx_reset.regmap = syscon_regmap_lookup_by_phandle_args(np,
			"reset-syscon", 2, syscon_args);
	if (IS_ERR(ctx_reset.regmap)) {
		pr_warn("failed to map dsi glb reg\n");
		return PTR_ERR(ctx_reset.regmap);
	} else {
		ctx_reset.ctrl_reg = syscon_args[0];
		ctx_reset.ctrl_mask = syscon_args[1];
	}

	return 0;
}

static void dsi_glb_enable(struct dsi_context *ctx)
{
	int ret;

	ret = clk_prepare_enable(clk_ap_ahb_dsi_eb);
	if (ret)
		pr_err("enable clk_ap_ahb_dsi_eb failed!\n");
}

static void dsi_glb_disable(struct dsi_context *ctx)
{
	clk_disable_unprepare(clk_ap_ahb_dsi_eb);
}

static void dsi_reset(struct dsi_context *ctx)
{
	regmap_update_bits(ctx_reset.regmap,
			ctx_reset.ctrl_reg,
			ctx_reset.ctrl_mask,
			ctx_reset.ctrl_mask);
	udelay(10);
	regmap_update_bits(ctx_reset.regmap,
			ctx_reset.ctrl_reg,
			ctx_reset.ctrl_mask,
			(unsigned int)(~ctx_reset.ctrl_mask));
}

const struct dsi_glb_ops qogirl6_dsi_glb_ops = {
	.parse_dt = dsi_glb_parse_dt,
	.reset = dsi_reset,
	.enable = dsi_glb_enable,
	.disable = dsi_glb_disable,
};

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Pony.Wu@unisoc.com");
MODULE_DESCRIPTION("sprd qogirl6 dsi global APB regs low-level config");
