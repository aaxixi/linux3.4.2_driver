#KERN_DIR = /home/cx/work/source/kernel/linux-2.6.30.4
KERN_DIR = /home/cx/work/source/kernel/linux-3.4.2
all:
	make -C $(KERN_DIR) M=`pwd` modules 
	rm -rf modules.order
	rm -rf *.cmd *.o *.symvers
	rm -rf IRQ_Test.mod.c

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= IRQ_Test.o
