CC = /home/shunya/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc

subsystem:
	cd ptm2human && $(MAKE)

all: subsystem 
	$(CC) -c detector.c -o main.o
	$(CC) main.o ptm2human/etb_format.o ptm2human/etmv4.o ptm2human/ptm.o ptm2human/stream.o ptm2human/tracer-etmv4.o ptm2human/tracer-ptm.o ptm2human/ptm2human.o -o detector.o 

clean:
	rm *.o