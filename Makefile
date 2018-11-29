EXTRA_CFLAGS += $(CFLAGS_EXTRA) -fno-pie
obj-m += chardev.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

