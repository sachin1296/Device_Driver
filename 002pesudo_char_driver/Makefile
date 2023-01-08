obj-m := main.o

ARCH=arm 
CROSS_COMPILE=arm-linux-gnueabihf- 
#KDIR=/home/sachin/Device_Driver/Device_Driver/ldd/source/linux-5.10
KDIR=/lib/modules/5.15.0-43-generic/build/


all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules
host:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
