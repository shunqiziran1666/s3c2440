#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>
#include <linux/device.h>
//#include <mach/gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-samsung.h>
#include <plat/gpio-cfg.h>

#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#define NAME_LENGTH 10

struct pin_desc {
        unsigned int pin_num;
        unsigned int irq_num;
        unsigned int key_value;
};

struct pin_desc jz2440_pins_desc[3];

static irqreturn_t buttons_irq(int irq, void *dev)
{
        dev_info(dev, "enter irq handle! irq num%d\n", irq);

        return IRQ_RETVAL(IRQ_HANDLED);;
}

int juo_pinctrl_probe(struct platform_device *pdev)
{
        struct resource *res;
        struct device_node *dnode = pdev->dev.of_node;
        int i, ret;
        char *string;
        dev_info(&pdev->dev, "juo pinctrl probe!\n");
        for (i = 0; i < sizeof(jz2440_pins_desc) / sizeof(jz2440_pins_desc[0]); i++) {
                res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
                if (res) {
                        jz2440_pins_desc[i].irq_num = res->start;
                        dev_info(&pdev->dev, "get irq num = %d\n",
                                 jz2440_pins_desc[i].irq_num);
                }
                else {
                        dev_err(&pdev->dev, "can not get irq num!\n");
                        return -EINVAL;
                }

                string = devm_kmalloc(&pdev->dev, NAME_LENGTH, GFP_KERNEL);
                sprintf(string, "EINT-%d", i);
                ret = devm_request_irq(&pdev->dev, jz2440_pins_desc[i].irq_num,
                                       buttons_irq,
                                       0, string, &pdev->dev);
                if (ret) {
                        dev_err(&pdev->dev, "request irq for %s fail!\n", string);
                }

                jz2440_pins_desc[i].pin_num = of_get_named_gpio(dnode, "eint-pins", i);
                if (jz2440_pins_desc[i].pin_num < 0) {
                        dev_err(&pdev->dev, "get gpio num fail!\n");
                        return -EINVAL;
                }
                else
                        dev_info(&pdev->dev, "get gpio num %d\n", jz2440_pins_desc[i].pin_num);
        }

        return 0;
}

static int juo_pinctrl_remove(struct platform_device *pdev)
{
        dev_info(&pdev->dev, "juo pinctrl remove, releasing resource!\n");

        return 0;
}

const struct of_device_id of_match_buttons[] = {
        {
                //.name = "jz2440-pinctrl-device", //该字段添加了也不能probe。。。
                //.type = "na",  //这个字段添加了不能被probe，原因有待考察
                .compatible = "jz2440-pinctrl", //通过该字段与dts节点进行匹配
                .data = NULL
        },
        {
                /*select*/
        }
};

struct platform_driver juo_pinctrl_drv = {
        .probe = juo_pinctrl_probe,
        .remove = juo_pinctrl_remove,
        .driver = {
                .name = "juo-pinctrl-driver",
                .of_match_table = of_match_buttons,
        }


};

static int pinctrl_init(void)
{
        platform_driver_register(&juo_pinctrl_drv);
        printk("platform driver register\n");
        return 0;
}

static void pinctrl_exit(void)
{
        printk("platform driver unregister\n");
        platform_driver_unregister(&juo_pinctrl_drv);
}

module_init(pinctrl_init);
module_exit(pinctrl_exit);
MODULE_LICENSE("GPL");