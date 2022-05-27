#ifndef _CACHEUTILS_H_
#define _CACHEUTILS_H_


#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/utsname.h>

static inline void flush(void *addr){
	asm volatile ("DC CIVAC, %0" ::"r"(addr));
	asm volatile ("DSB ISH");
	asm volatile("ISB");
}

void mfence(){ asm volatile ("dmb ish"); }

uint64_t rdtscp(){
	uint64_t res = 0;

	asm volatile("DSB SY");
	asm volatile("ISB");
	asm volatile("MRS %0, PMCCNTR_EL0":"=r"(res));
	asm volatile("DSB SY");
	asm volatile("ISB");

	return res;
}

void maccess(void *p){
	volatile uint32_t val;
	asm volatile("ldr %0, [%1]\n\t":"=r"(val):"r"(p));
	asm volatile("dsb ish");
	asm volatile("isb");
}

static inline uint64_t flush_reload(void *addr){
	uint64_t time1, time2, read_data;

	asm volatile (
		"dsb sy \n\t"
		"isb \n\t"
		"mrs %0, PMCCNTR_EL0 \n\t"
		"isb \n\t"
		"ldr %2, [%3] \n\t"
		"dsb ish \n\t"
		"isb \n\t"
		"mrs %1, PMCCNTR_EL0 \n\t"
		"isb \n\t"
		"dsb sy \n\t"
		"dc civac, %3\n\t"
		"dsb sy \n\t"
		: "=&r"(time1), "=&r"(time2), "=&r"(read_data)
		: "r"(addr)
	);
	return time2-time1;
}


#endif
