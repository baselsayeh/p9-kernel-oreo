/*
 * Power off or Reboot the hi3xxx device.
 *
 * Copyright (C) 2014 Hisilicon, Young Choi <cuiyong1@hisilicon.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <asm/cacheflush.h>

#include "pmic_interface.h"

#include <linux/kmsg_dump.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/mfd/hisi_pmic.h>
#include <linux/mfd/hisi_pmic_mntn.h>
#include <soc_gpio_interface.h>
#include <soc_sctrl_interface.h>
static void __iomem *powerhold_gpio_base;
static int g_powerhold_gpio_offset = 0;

#include <linux/hisi/rdr_hisi_platform.h>
#include <rdr_inner.h>

static void __iomem *sysctrl_base;
extern void (*pm_power_off)(void);
extern void (*arm_pm_restart)(char str, const char *cmd);
extern int gps_get_lhd_status(void);
extern struct cmdword reboot_reason_map[];

void hisi_pm_system_off(void)
{
	int out_dir;

	set_reboot_reason(AP_S_COLDBOOT);
	while(1) {

		if (powerhold_gpio_base != NULL) {
			printk(KERN_INFO "system power off now\n");
			/*set direction*/
			out_dir = readl(SOC_GPIO_GPIODIR_ADDR(powerhold_gpio_base));
			out_dir |= (1 << g_powerhold_gpio_offset);
			writel(out_dir, SOC_GPIO_GPIODIR_ADDR(powerhold_gpio_base));
			writel(0, (void *)powerhold_gpio_base + (1 << (g_powerhold_gpio_offset + 2)));

			mdelay(1000);
			machine_restart("chargereboot");
		}
	}

}

/*******************************************************************************
Function:       hisi_pm_system_reset
Description: 	注册到machine_restart函数中的回调，当系统复位时，调用。
Input:		    cmd  复位类型
Output:		    NA
Return:		    NA
********************************************************************************/
void hisi_pm_system_reset(char mode, const char *cmd)
{
    unsigned int i;
    unsigned int curr_reboot_type = UNKNOWN;

    // 0 gps lhd status normal
    // 1 gpa lhd status error
    if(1 == gps_get_lhd_status())
    {
        printk("gps error happened, need reset pmu status\n");
        hisi_pmic_set_cold_reset();
    }


    if (cmd == NULL || *cmd == '\0') {
        printk(KERN_ERR "hisi_pm_system_reset cmd is null\n");
        cmd = "coldboot";
    } else {
        printk(KERN_ERR "hisi_pm_system_reset cmd is %s\n", cmd);
    }
    for (i=0; i < get_reboot_reason_map_size(); i++) {
        if (!strncmp((char *)reboot_reason_map[i].name, cmd, sizeof(reboot_reason_map[i].name))) {
            curr_reboot_type = reboot_reason_map[i].num;
            break;
        }
    }
    if (UNKNOWN == curr_reboot_type) {
        curr_reboot_type = COLDBOOT;
        console_verbose();
        dump_stack();
    }
	set_reboot_reason(curr_reboot_type);
	hisiap_nmi_notify_lpm3();
	printk(KERN_ERR "ap send nmi to lpm3, then goto dead loop.\n");
	while(1);
}
static int hi3xxx_reset_probe(struct platform_device *pdev)
{
	struct device_node *np = NULL;
	int offset = 0;
	int ret = 0;

	np = of_find_compatible_node(NULL, NULL, "hisi,powerhold");
	/* get powerhold gpio */
	if (!np) {
		printk(KERN_ERR "get powerhold np error !\n");
	} else {
		powerhold_gpio_base = of_iomap(np, 0);
		printk(KERN_INFO "powerhold_gpio_base = %pK !\n", powerhold_gpio_base);
		if (!powerhold_gpio_base) {
			pr_err("%s: powerhold_gpio_base is NULL\n", __func__);
		}
		ret = of_property_read_u32(np, "offset", (u32 *)&offset);
		if (ret) {
			printk(KERN_ERR "get offset error !\n");
			powerhold_gpio_base = 0;
		}
		else {
			printk(KERN_INFO "offset = 0x%x !\n", (unsigned int)offset);
			g_powerhold_gpio_offset = offset;
		}
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	if (!np) {
		pr_err("%s: hisilicon,sysctrl No compatible node found\n", __func__);
		return -ENODEV;
	}

	sysctrl_base = of_iomap(np, 0);
	if (!sysctrl_base) {
		pr_err("%s: hisilicon,sysctrl_base is NULL\n", __func__);
		return -ENODEV;
	}

	pm_power_off = hisi_pm_system_off;
	arm_pm_restart = hisi_pm_system_reset;

	return 0;
}

static int hi3xxx_reset_remove(struct platform_device *pdev)
{
	if (pm_power_off == hisi_pm_system_off)
		pm_power_off = NULL;

	if (arm_pm_restart == hisi_pm_system_reset)
		arm_pm_restart = NULL;

	return 0;
}

static const struct of_device_id of_hi3xxx_reset_match[] = {
	{ .compatible = "hisilicon,hi3xxx-reset", },
	{},
};

static struct platform_driver hi3xxx_reset_driver = {
	.probe = hi3xxx_reset_probe,
	.remove = hi3xxx_reset_remove,
	.driver = {
		.name = "hi3xxx-reset",
		.owner = THIS_MODULE,
		.of_match_table = of_hi3xxx_reset_match,
	},
};

static int __init hi3xxx_reset_init(void)
{
	return platform_driver_register(&hi3xxx_reset_driver);
}
module_init(hi3xxx_reset_init);

static void __exit hi3xxx_reset_exit(void)
{
	platform_driver_unregister(&hi3xxx_reset_driver);
}
module_exit(hi3xxx_reset_exit);

MODULE_AUTHOR("Choi Young <cuiyong1@hisilicon.com>");
MODULE_DESCRIPTION("hi3xxx reset driver");
MODULE_LICENSE("GPL v2");
