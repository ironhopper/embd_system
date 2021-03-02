#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <asm/uaccess.h>
#define DEV_NAME "periled"
#define MAX_LED_NUM 8
static unsigned int gpio_led[MAX_LED_NUM];

static struct platform_device_id periled_devtype[] = {
	{
		.name = DEV_NAME,
		.driver_data = 0,
	}, {
		/* sentinel */
	}
};
static const struct of_device_id periled_dt_ids[] = {
	{
		.compatible = "cndi,periled",
		.data = &periled_devtype[0],
	},		{ /* sentinel */ }
};

static ssize_t periled_write(struct file *file, const char __user *data, size_t len, loff_t *ppos)
{
	char msg;
	long res;
	int i;
	res = copy_from_user(&msg, data, 1);
	if (res)
		return -EFAULT;
	for(i = 0; i < MAX_LED_NUM ; i++ )
	{
	if ( msg & (0x01 << i))
		gpio_set_value(gpio_led[i], 1);
	else
		gpio_set_value(gpio_led[i], 0);
	}
		return len;
}

static int setup_periled_gpio(void)
{
	int i, ret;
	char name[20];
	for (i=0; i<MAX_LED_NUM; i++)
	{
		sprintf(name, "%s %d", "periled", i);
		ret = gpio_request(gpio_led[i], name);
		if (ret) {
			printk(KERN_ERR "request %s %d  fail.%d \n", name,gpio_led[i],ret);
			return -1;
		}
		else {
			printk("request %s %d success %d.\n",name,gpio_led[i],ret);
			gpio_direction_output(gpio_led[i], 0);
		}
	}
	return 0;
}

static void release_periled_gpio(void)
{
	int i ;
	for(i = 0; i < MAX_LED_NUM ; i++){
		gpio_free(gpio_led[i]);
	}
}


static int periled_probe(struct platform_device *pdev)
{
	int i;
	struct device_node *np;
	struct pinctrl *pin;
	char property[20];
	np = pdev->dev.of_node;
	printk("%s : enter +++ (JEHA KIM,embedded system)\n", __func__);
	if (!np)
		return -ENODEV;
	for (i = 0; i < MAX_LED_NUM; i++) {
		sprintf(property, "%s%d", "periled-gpio-", i);
		gpio_led[i] = of_get_named_gpio(np, property, 0);
		printk("%d gpio_led \n",gpio_led[i]);
	}
	pin = devm_pinctrl_get_select(&pdev->dev, "default");
	if (IS_ERR(pin)){
		printk("pin config error: %d \n",!pin);	
		return -1;
	}
	release_periled_gpio();
	if (setup_periled_gpio()){
		printk("Failed to allocate gpio setup.\n");
		return -1;
	}
	printk("%s enter +++++ 2\n",__func__);		
	return 0;
}


static int periled_remove(struct platform_device *pdev)

{
	printk("%s : close --- (JEHA KIM,embedded system)\n",__func__);
	release_periled_gpio();
	return 0;
}
static const struct file_operations periled_fops = {
	.owner = THIS_MODULE,
	.write = periled_write,
};
static struct platform_driver periled_driver = {
	.driver = {
		.name = DEV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = periled_dt_ids,
	},
	.id_table = periled_devtype,
	.probe = periled_probe,
	.remove = periled_remove,
};


static int __init periled_driver_init(void)
{
	int retval;
	platform_driver_register(&periled_driver);
	retval = register_chrdev(240, DEV_NAME, &periled_fops);
	if (retval < 0)
	{
		// platform_driver_unregister(&periled_driver);
		printk("Failed to register the char dev \n");
		return retval;
	}
	return retval;
}

static void __exit periled_driver_exit(void)
{
	unregister_chrdev(240, DEV_NAME);
	platform_driver_unregister(&periled_driver);
	printk("exit led driver success!\n");
	return;
}

module_init(periled_driver_init);
module_exit(periled_driver_exit);
MODULE_LICENSE("GPL");
