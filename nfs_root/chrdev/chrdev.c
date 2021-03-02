#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>

#define CHR_DEV_NAME "chr_dev"  // device file name
#define CHR_DEV_MAJOR 240 // major number

int chr_open(struct inode *inode, struct file *filp){
	printk("Vitual Chr Dev: open\n");
	return 0;
}

ssize_t chr_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos){
	printk("Vitual Chr Dev: write Data \n");
	return count;
}


ssize_t chr_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	printk("Vitual Chr Dev: read Data - *  * \n");
	return count;
}


int chr_release(struct inode *inode, struct file *filp){
	printk("Vitual Chr Dev: release! \n");
	return 0;
}

struct file_operations chr_fops = {
	.owner = THIS_MODULE,
	.write = chr_write,
	.read = chr_read,
	.open = chr_open,
	.release = chr_release,
};

int init_chrdev(void){
	int registration;
	registration = register_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME, &chr_fops);
	
	if(registration <0){
		printk("registration is failed \n");
		return registration;
	
	}
	printk("registration is success!! \n");
	return 0;
}

void cleanup_chrdev(void){
	printk("module is unregisterd! \n"); // space not available!!
	unregister_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME);
}

module_init(init_chrdev);
module_exit(cleanup_chrdev);
MODULE_LICENSE("GPL");

