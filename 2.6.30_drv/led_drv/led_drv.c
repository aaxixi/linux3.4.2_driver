


//==============================drv

/*

struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
}


*/

#if 1
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <linux/io.h>

static struct class* led_drv_class;
static struct device* led_drv_dev;
volatile unsigned long *gpf_con;
volatile unsigned long *gpf_dat;

#define GPF_ADDR (0x56000010)
#define GPF_LEN  (16)
#define LED_NUM  (4)
#define GPBOUT  (1<<(5*2)) |(1<<(6*2)) | (1<<(7*2)) | (1<<(8*2));               //设置GPB5/6/7/8为输出  

int led_open (struct inode * pnode, struct file * pfile)
{
	*gpf_con = GPBOUT;
	*gpf_dat = 0x0;
	printk("This is led_open!\n");
	return 0;
}

ssize_t led_write (struct file *pfile, const char __user *pbuf, size_t n, loff_t *pret)
{
	unsigned int led_val; 
	unsigned int gpio_temp;
	copy_from_user(&led_val,pbuf,n);
	led_val = led_val&0xff;
	gpio_temp = *gpf_dat ;
	printk("This is led_write: val:0x%x gpio_temp:0x%x!\n",led_val,gpio_temp);

	*gpf_dat = (led_val<<5);
	return 0;
}



//===========================================
int major;
static struct file_operations led_dev_ops=
{
	.owner 	= THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
	.open	= led_open, 
	.write	= led_write,
	
};

static int __init led_drv_init(void)
{
	int ret=0;
	major = register_chrdev(0, "led_drv", &led_dev_ops); // 注册, 告诉内核
	led_drv_class = class_create(THIS_MODULE,"led_drv_class");
	led_drv_dev  = device_create(led_drv_class,NULL,MKDEV(major,0),NULL,"led_drv_dev");
	ret = request_mem_region(GPF_ADDR, GPF_LEN, "led_drv_dev");
	if(ret)
	gpf_con = (volatile unsigned long*)ioremap(GPF_ADDR,GPF_LEN);
	gpf_dat = gpf_con+1;
	printk("This is led_drv_init! maj:%d\n",major);
	return 0;
}

static int __exit led_drv_exit(void)
{
	unregister_chrdev(major, "led_drv"); // 卸载
	device_destroy(led_drv_class,MKDEV(major,0));
	class_destroy(led_drv_class);
	iounmap(gpf_con);
	printk("This is led_drv_exit!\n");
	return 0;
}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.embedsky.net");
MODULE_DESCRIPTION("GPIO control for EmbedSky SKY2440/TQ2440 Board");

#else

/*************************************

NAME:EmbedSky_hello.c
COPYRIGHT:www.embedsky.net

*************************************/

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>


#define DEVICE_NAME "GPIO-Control"

/* 应用程序执行ioctl(fd, cmd, arg)时的第2个参数 */
#define IOCTL_GPIO_ON	1
#define IOCTL_GPIO_OFF	0


static struct class* led_drv_class;
static struct device* led_drv_dev;
int major;

/* 用来指定LED所用的GPIO引脚 */
static unsigned long gpio_table [] =
{
	S3C2410_GPB5,
	S3C2410_GPB6,
	S3C2410_GPB7,
	S3C2410_GPB8,
};

/* 用来指定GPIO引脚的功能：输出 */
static unsigned int gpio_cfg_table [] =
{
	S3C2410_GPB5_OUTP,
	S3C2410_GPB6_OUTP,
	S3C2410_GPB7_OUTP,
	S3C2410_GPB8_OUTP,
};

static int tq2440_gpio_ioctl(
	struct inode *inode, 
	struct file *file, 
	unsigned int cmd, 
	unsigned long arg)
{
	if (arg > 4)
	{
		return -EINVAL;
	}

	switch(cmd)
	{
		case IOCTL_GPIO_ON:
			// 设置指定引脚的输出电平为0
			s3c2410_gpio_setpin(gpio_table[arg], 0);
			return 0;

		case IOCTL_GPIO_OFF:
			// 设置指定引脚的输出电平为1
			s3c2410_gpio_setpin(gpio_table[arg], 1);
			return 0;

		default:
			return -EINVAL;
	}
}

static struct file_operations led_dev_ops = {
	.owner	=	THIS_MODULE,
	.ioctl	=	tq2440_gpio_ioctl,
};


static int __init dev_init(void)
{
	int ret;

	int i;
	
	for (i = 0; i < 4; i++)
	{
		s3c2410_gpio_cfgpin(gpio_table[i], gpio_cfg_table[i]);
		s3c2410_gpio_setpin(gpio_table[i], 0);
	}

	major = register_chrdev(0, "led_drv", &led_dev_ops); // 注册, 告诉内核
	led_drv_class = class_create(THIS_MODULE,"led_drv_class");
	led_drv_dev  = device_create(led_drv_class,NULL,MKDEV(major,0),NULL,"led_drv_dev");

	printk (DEVICE_NAME" initialized\n");

	return ret;
}

static void __exit dev_exit(void)
{
	unregister_chrdev(major, "led_drv"); // 卸载
	
	device_destroy(led_drv_class,MKDEV(major,0));
	class_destroy(led_drv_class);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.embedsky.net");
MODULE_DESCRIPTION("GPIO control for EmbedSky SKY2440/TQ2440 Board");



#endif

