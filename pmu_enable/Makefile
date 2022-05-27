obj-m += pmu_enable.o

PWD := $(shell pwd)
KDIR := /usr/src/linux

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

