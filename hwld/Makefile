obj-m := main.o

ARCH=arm 
CROSS_COMPILE=arm-linux-gnueabihf- 
KDIR=/home/sachin/Device_Driver/Device_Driver/ldd/source/linux-5.10


all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules
