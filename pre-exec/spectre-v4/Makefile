.PHONY: clean all
all:
	aarch64-linux-gnu-gcc -march=armv8-a -D_ARM_ARCH_8A_ -static -Os spectre_v4.c -o v4_tmp -I../../
clean:
	@rm spectreV4
