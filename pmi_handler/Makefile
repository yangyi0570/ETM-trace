#CONFIG_MODULE_SIG=n
obj-m	:= pmi_handler.o
KDIR	:= /home/shunya/linux
CC		:= /home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
PWD	:= $(shell pwd)

cflags-y += -O2

# etm_trace module export symbols, pmi needs to use it.
KBUILD_EXTRA_SYMBOLS += /home/shunya/etm-trace/ETM-trace/Module.symvers

all:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) ARCH=arm64 modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) ARCH=arm64 clean