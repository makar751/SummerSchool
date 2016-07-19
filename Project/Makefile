obj-m += smsc95xx.o
KDIR:=/home/allex/ForRaspberry/linux
all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:
	make -C $(KDIR) M=$(shell pwd) clean
