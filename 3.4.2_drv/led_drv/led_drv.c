#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>

#include <linux/io.h>		//ioremap
#include <linux/uaccess.h> 	//copy_from_user


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

static struct class* led_drv_class;
static struct device* led_drv_dev;

typedef struct _LED_GPIO
{
	unsigned long GPF_CON;
	unsigned long GPF_DAT;
	unsigned long GPF_UP;
}LED_GPIO_t;

volatile unsigned long *gpf_con;
volatile unsigned long *gpf_dat;

static volatile LED_GPIO_t* pLedGpio;

#define GPF_ADDR (0x56000010)//(0x56000050)
#define GPF_LEN  (16)
#define LED_NUM  (4)



int led_open (struct inode * pnode, struct file * pfile)
{
	//pLedGpio->GPF_CON &=~( (0x3<<(5*2))|(0x3<<(6*2))|(0x3<<(7*2))|(0x3<<(8*2)));
	//pLedGpio->GPF_CON |= ( (0x01<<(5*2))|(0x01<<(6*2))|(0x01<<(7*2))|(0x01<<(8*2)));

	//pLedGpio->GPF_DAT|=( (0x01<<5)|(0x01<<6)|(0x01<<7)|(0x01<<8));
	*gpf_con &=~( (0x3<<(5*2))|(0x3<<(6*2))|(0x3<<(7*2))|(0x3<<(8*2)));
	*gpf_con |= ( (0x01<<(5*2))|(0x01<<(6*2))|(0x01<<(7*2))|(0x01<<(8*2)));
	*gpf_dat|=( (0x01<<5)|(0x01<<6)|(0x01<<7)|(0x01<<8));
	
	printk("This is led_open!\n");
	return 0;
}


ssize_t led_read (struct file *pfile, char __user * pbuf, size_t n, loff_t * pret)
{
	printk("This is led_read!\n");
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
	//pLedGpio->GPF_DAT&=~( (0x01<<5)|(0x01<<6)|(0x01<<7)|(0x01<<8));
	//pLedGpio->GPF_DAT|= led_val;
	
	*gpf_dat&=~( (0x01<<5)|(0x01<<6)|(0x01<<7)|(0x01<<8));
	*gpf_dat|= (led_val<<5);
	return 0;
}

int led_release (struct inode *pnode, struct file *pfile)
{

	
	printk("This is led_release!\n");
	return 0;
}



//===========================================
int major;
static struct file_operations led_dev_ops=
{
	.owner 	= THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
	.open	= led_open, 
	.read	= led_read, 
	.write	= led_write,
	
};

static int led_drv_init(void)
{
	
	major = register_chrdev(0, "led_drv", &led_dev_ops); // 注册, 告诉内核
	led_drv_class = class_create(THIS_MODULE,"led_drv_class");
	led_drv_dev  = device_create(led_drv_class,NULL,MKDEV(major,0),NULL,"led_drv_dev");

	//pLedGpio = (volatile LED_GPIO_t*)ioremap(GPF_ADDR,GPF_LEN);
	gpf_con = (volatile unsigned long*)ioremap(GPF_ADDR,GPF_LEN);
	gpf_dat = gpf_con+1;
	printk("This is led_drv_init! maj:%d\n",major);
	return 0;
}

static int led_drv_exit(void)
{
	unregister_chrdev(major, "led_drv"); // 卸载
	
	device_destroy(led_drv_class,MKDEV(major,0));
	class_destroy(led_drv_class);

	iounmap(pLedGpio);
	
	printk("This is led_drv_exit!\n");
	return 0;
}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");

