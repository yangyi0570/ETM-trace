obj-m += demo.o
demo-y := trace.o connect.o etm.o tmc.o funnel.o tmc-etf.o tmc-etr.o pmu.o

KDIR=/home/shunya/linux
# for hikey
#KDIR=/usr/src/linux
# for juno r2, cross platform compile
CC=/home/shunya/toolchain/gcc-arm-7.5-x86_64-aarch64/bin/aarch64-linux-gnu-gcc

PWD=$(shell pwd)

all:
# for juno r2
	make -C $(KDIR) M=$(PWD) CC=$(CC) ARCH=arm64 modules	
# for hikey 970
#   make -C $(KDIR) M=$(PWD) modules
	rm -f *.o *.mod.c Module.symvers modules.order 
	find ./ -name "*.cmd" | xargs rm -f

clean:
	make -C $(KDIR) M=$(PWD) CC=$(CC) ARCH=arm64 clean
# for hikey 970
# make -C $(KDIR) M=$(PWD) clean
