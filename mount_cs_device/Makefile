obj-m += mount_cs_device.o

KDIR=/home/shunya/linux
PWD=$(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) CC=/home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc ARCH=arm64 modules

clean:
	make -C $(KDIR) M=$(PWD) CC=/home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc ARCH=arm64 clean
