VERSION = $(shell uname -r)
KDIR = /lib/modules/$(VERSION)/build
PWD = $(shell pwd)
TARGET = vfs
obj-m := $(TARGET).o
default: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions
disclean: clean
	@rm -f *.ko *.symvers *.mod
