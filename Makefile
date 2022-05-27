obj-m += demo.o
demo-y := trace.o connect.o etm.o tmc.o funnel.o tmc-etf.o tmc-etr.o pmu.o

KDIR=/home/shunya/linux
PWD=$(shell pwd)

all:	
	make -C $(KDIR) M=$(PWD) CC=/home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc ARCH=arm64 modules
	rm *.o .*.o.cmd

clean:
	make -C $(KDIR) M=$(PWD) CC=/home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc ARCH=arm64 clean
