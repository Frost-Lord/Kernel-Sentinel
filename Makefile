obj-m += ksentinel.o
obj-m += driver/driver.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
BUILDDIR := $(PWD)/build

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	mkdir -p $(BUILDDIR)
	cp *.ko $(BUILDDIR)
	cp driver/*.ko $(BUILDDIR)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf $(BUILDDIR)
