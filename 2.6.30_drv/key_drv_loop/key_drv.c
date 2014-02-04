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

static struct class* key_drv_class;
static struct device* key_drv_dev;

static volatile unsigned int *gpf_con;
static volatile unsigned int *gpf_dat;

static volatile unsigned int *gpb_con;
static volatile unsigned int *gpb_dat;

#define GPF_ADR (0x56000050)
#define GPB_ADR (0x56000010)

int key_open (struct inode * pnode, struct file * pfile)
{

	printk("This is key_open!\n");
	return 0;
}

ssize_t key_read (struct file *pfile, const char __user *pbuf, size_t n, loff_t *pret)
{
	unsigned int key_val[4]; 
	unsigned int temp;

	//if(n!=4)
		//return -EINVAL;

	temp= *gpf_dat;

	key_val[0] = (temp&(1<<0))?0:1;
	key_val[1] = (temp&(1<<1))?0:1;
	key_val[2] = (temp&(1<<2))?0:1;
	key_val[3] = (temp&(1<<4))?0:1;

	copy_to_user(pbuf,key_val,sizeof(key_val));


	//printk("This is key_read: val[0]:0x%x val[1]:0x%x val[2]:0x%x val[3]:0x%x!\n",key_val[0],key_val[1],key_val[2],key_val[3]);

	return (sizeof(key_val));
}

ssize_t key_write (struct file *pfile, const char __user *pbuf, size_t n, loff_t *pret)
{
	unsigned int led_val; 
	unsigned int gpio_temp;
	copy_from_user(&led_val,pbuf,n);
;
	printk("This is key_write: val:0x%x gpio_temp:0x%x!\n",led_val,gpio_temp);

	return 0;
}




//===========================================
int major;
static struct file_operations key_dev_ops=
{
	.owner 	= THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
	.open	= key_open, 
	.read 	= key_read,
	.write	= key_write,
	
};

static int __init key_drv_init(void)
{
	int ret=0;
	major = register_chrdev(0, "key_drv", &key_dev_ops); // 注册, 告诉内核
	key_drv_class = class_create(THIS_MODULE,"key_drv_class");
	key_drv_dev  = device_create(key_drv_class,NULL,MKDEV(major,0),NULL,"key_drv_dev");

	gpf_con = (volatile unsigned int*)ioremap(GPF_ADR,16);
	gpf_dat = gpf_con+1;
	*gpf_con &= ~( (0x3<<(0*2)) | (0x3<<(1*2)) | (0x3<<(2*2)) | (0x3<<(4*2)) );
	
	gpb_con = (volatile unsigned int*)ioremap(GPB_ADR,16);
	gpb_dat = gpb_con+1;
	
	*gpb_con &= ~( (0x3<<(2*5)) | (0x3<<(2*6)) | (0x3<<(2*7)) | (0x3<<(2*8)) );
	*gpb_con |=  ( (0x1<<(2*5)) | (0x1<<(2*6)) | (0x1<<(2*7)) | (0x1<<(2*8)) );
	
	*gpb_dat &= ~( (0x1<<(5)) | (0x1<<(6)) | (0x1<<(7)) | (0x1<<(8)) );
	*gpb_dat |=  ( (0x1<<(5)) | (0x0<<(6)) | (0x1<<(7)) | (0x0<<(8)) );

	printk("This is key_drv_init! maj:%d\n",major);
	return 0;
}

static int __exit key_drv_exit(void)
{
	unregister_chrdev(major, "key_drv"); // 卸载
	device_destroy(key_drv_dev,MKDEV(major,0));
	class_destroy(key_drv_class);
	
	*gpb_dat |=  ( (0x1<<(5)) | (0x1<<(6)) | (0x1<<(7)) | (0x1<<(8)) );

	iounmap(gpf_con);
	iounmap(gpb_con);
	
	printk("This is key_drv_exit!\n");
	return 0;
}


module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.embedsky.net");
MODULE_DESCRIPTION("GPIO control for EmbedSky SKY2440/TQ2440 Board");


