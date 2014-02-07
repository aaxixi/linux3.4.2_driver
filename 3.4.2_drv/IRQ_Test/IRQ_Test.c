#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/regs-gpio.h>
#include <asm/poll.h>


volatile unsigned long irq_cnt=0;

static struct class *key_class;
static struct fasync_struct *key_async_queue;

#if 0
static irqreturn_t key_handler(int irq, void *dev_id)
{
 int val;
 struct irq_desc *key_desc = (struct irq_desc *)dev_id;
 val = s3c2410_gpio_getpin(key_desc->pin);
 val=val>>key_desc->num;
 if(val==0)
  {
   key_val=key_desc->key;
  }
 else if(val==1)
  {
   key_val=key_desc->key | 0x80;
  }
 
  kill_fasync(&key_async_queue, SIGIO, POLL_IN);//add
 
 return 0;
}
#endif

struct irq_desc {
 unsigned long pin;
 int num;
 char *name;
};
static struct irq_desc dev_id[1]=
{
 {S3C2410_GPF(1) ,1,"key1"},
};

static irqreturn_t key_handler(int irq, void *dev_id)
{
 int val;
 struct irq_desc *key_desc = (struct irq_desc *)dev_id;

 //s3c2410_gpio_getpin并不是单单获得某个位的状态，而是这一位在整个寄存器中的值
 val=s3c2410_gpio_getpin(key_desc->pin);
 val=val>>key_desc->num;
 
 irq_cnt++;
 kill_fasync(&key_async_queue, SIGIO, POLL_IN);//add
 #if 0
 printk("cnt:%ld",irq_cnt);
 if(val==0)
  {
   printk("%s is down\n" ,key_desc->name);
  }
 else if(val==1)
  {
   printk("%s is up\n" ,key_desc->name);
  }
 #endif
 return 0;
}

static int key_open (struct inode *inode, struct file *file)
{
    s3c2410_gpio_cfgpin(S3C2410_GPF(1), S3C2410_GPIO_IRQ);
	request_irq(IRQ_EINT1, key_handler,IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,"key1",&dev_id[0]);
    return 0;
}
static ssize_t key_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	copy_to_user(buf, &irq_cnt, sizeof(unsigned long));
    return sizeof(unsigned long);
}

//当设备关闭的时候会调用release函数
int key_release(struct inode *inode, struct file *file)
{
 free_irq(IRQ_EINT1 ,&dev_id[0]);

 return 0;
}

static int key_fasync(int fd, struct file *file, int on)
{ 
    //主要是用于设置key_async_queue，在kill_fasync会用到它
 return fasync_helper(fd, file, on, &key_async_queue);
}

static struct file_operations key_operation = {
    .owner   =  THIS_MODULE,
    .open    =  key_open,
    .read    =  key_read,
    .release =  key_release,
	.fasync   = key_fasync,
};

static int major;
static int minor;
int key_devinit(void)
{
	irq_cnt=0;
    major=register_chrdev(0, "irqTest", &key_operation);
    key_class=class_create(THIS_MODULE, "irqTest_class");
    device_create(key_class, NULL, MKDEV(major , 0), NULL, "irqTest");

    return 0;
}

void key_exit(void)
{
    unregister_chrdev(major , "irqTest");
    device_destroy(key_class, MKDEV(major , 0));
    class_destroy(key_class);
}

module_init(key_devinit);
module_exit(key_exit);
MODULE_LICENSE("GPL");