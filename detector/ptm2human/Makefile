CC = /home/shunya/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc

all:
	$(CC) -c etb_format.c -o etb_format.o  
	$(CC) -c etmv4.c -o etmv4.o    
	$(CC) -c ptm.c -o ptm.o  
	$(CC) -c stream.c -o stream.o  
	$(CC) -c tracer-etmv4.c -o tracer-etmv4.o  
	$(CC) -c tracer-ptm.c -o tracer-ptm.o
	$(CC) -c ptm2human.c -o ptm2human.o
clean:
	rm *.o