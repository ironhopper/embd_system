#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int hello_init(void)
{
	printk("hello module \n");
	return 0;
}

void hello_exit(void)
{
	printk("bye module \n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
