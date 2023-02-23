CC = /home/shunya/toolchain/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc

all:
	$(CC) -c etb_format.c -o etb_format.o  
	$(CC) -c etmv4.c -o etmv4.o    
	$(CC) -c ptm.c -o ptm.o  
	$(CC) -c stream.c -o stream.o  
	$(CC) -c tracer-etmv4.c -o tracer-etmv4.o  
	$(CC) -c tracer-ptm.c -o tracer-ptm.o
	$(CC) -c ptm2human.c -o ptm2human1.o
	$(CC) -fPIC -shared etb_format.o etmv4.o ptm.o stream.o tracer-etmv4.o tracer-ptm.o ptm2human1.o -o libptm2human.so
clean:
	rm *.o *.so