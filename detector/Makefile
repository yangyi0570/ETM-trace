CC = /home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc

subsystem:
	cd ptm2human && $(MAKE)

all: subsystem 
	$(CC) -c detector.c -o main.o
	$(CC) main.o ptm2human.so -o detector.o 