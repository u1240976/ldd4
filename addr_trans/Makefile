obj-m = dev_addr.o
dev_addr-y = dev_addr_trans.o lookup_paddr.o

KERNELDIR := /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
