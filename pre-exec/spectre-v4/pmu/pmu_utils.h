#ifndef PMU_UTILS_H
#define PMU_UTILS_H

#include <stdio.h>
#include <string.h>

enum bool
{
    false = 0,
    true
};

enum Type
{
    SW_INCR = 0,
    L1I_CACHE_REFILL,
    L1I_TLB_REFILL,
    L1D_CACHE_REFILL,
    L1D_CACHE,
    L1D_TLB_REFILL,
    LD_RETIRED,
    ST_RETIRED,
    INST_RETIRED,
    EXC_TAKEN,
    EXC_RETURN,
    CID_WRITE_RETIRED,
    PC_WRITE_RETIRED,
    BR_IMMED_RETIRED,
    BR_RETURN_RETIRED,
    UNALIGNED_LDST_RETIRED,
    BR_MIS_PRED,
    CPU_CYCLES,
    BR_PRED,
    MEM_ACCESS,
    L1I_CACHE,
    L1D_CACHE_WB,
    L2D_CACHE,
    L2D_CACHE_REFILL,
    L2D_CACHE_WB,
    BUS_ACCESS,
    MEMORY_ERROR,
    INST_SPEC,
    TTBR_WRITE_RETIRED,
    BUS_CYCLES,
    NOP1,
    L1D_CACHE_ALLOCATE = 31,
    L2D_CACHE_ALLOCATE = 32,
    NOP2,
    NOP3,
    NOP4,
    NOP5,
    L1D_TLB = 37,
    L1I_TLB = 38,
    L2I_CACHE = 39,
    L2I_CACHE_REFILL = 40,
    NOP6,
    NOP7,
    NOP8,
    NOP9,
    NOP10,
    NOP11,
    NOP12,
    NOP13,
    NOP14,
    NOP15,
    NOP16,
    DTLB_WALK = 52,
    ITLB_WALK = 53,
    NOP17,
    NOP18,
    NOP19,
    L1D_CACHE_LMISS_RD = 57,
    OP_RETIRED = 58,
    OP_SPEC = 59,
    STALL = 60,
	INVALID
}evType;

enum bool isValidType(enum Type ttype){
    if(ttype==30||(33<=ttype&&ttype<=36)||(41<=ttype&&ttype<=51)||(54<=ttype&&ttype<=56)){
        return false;
    }
    return true;
}

#define ARMV8_PMEVTYPER_P              (1 << 31) /* EL1 modes filtering bit */
#define ARMV8_PMEVTYPER_U              (1 << 30) /* EL0 filtering bit */
#define ARMV8_PMEVTYPER_NSK            (1 << 29) /* Non-secure EL1 (kernel) modes filtering bit */
#define ARMV8_PMEVTYPER_NSU            (1 << 28) /* Non-secure User mode filtering bit */
#define ARMV8_PMEVTYPER_NSH            (1 << 27) /* Non-secure Hyp modes filtering bit */
#define ARMV8_PMEVTYPER_M              (1 << 26) /* Secure EL3 filtering bit */
#define ARMV8_PMEVTYPER_MT             (1 << 25) /* Multithreading */
#define ARMV8_PMEVTYPER_EVTCOUNT_MASK  0x3ff



static inline void
enable_pmu(uint32_t evtCount)
{
#if defined(__GNUC__) && defined __aarch64__
	evtCount &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
	asm volatile("isb");
	/* Just use counter 0 */
	asm volatile("msr pmevtyper0_el0, %0" : : "r" (evtCount));
	/*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1 enable */
	uint32_t r = 0;

	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r|1));
#else
#error Unsupported architecture/compiler!
#endif
}

static inline uint32_t
read_pmu(void)
{
#if defined(__GNUC__) && defined __aarch64__
        uint32_t r = 0;
	asm volatile("mrs %0, pmevcntr0_el0" : "=r" (r)); 
	return r;
#else
#error Unsupported architecture/compiler!
#endif
}

static inline void
disable_pmu(uint32_t evtCount)
{
#if defined(__GNUC__) && defined __aarch64__
	/*   Performance Monitors Count Enable Set register: clear bit 0 */
	uint32_t r = 0;

	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r&&0xfffffffe));
#else
#error Unsupported architecture/compiler!
#endif
}

static uint32_t evtCnt[70], cntBegin, cntEnd;

void reset(){
	evType = INVALID;
    memset(evtCnt, 0, sizeof evtCnt);
    cntBegin = 0, cntEnd = 0;
}

void pmu_begin(enum Type type){
	if(isValidType(type)==false){
		printf("invalid event type\n");
		return;
	}
	
    evType = type;
    enable_pmu(evType);
    cntBegin = read_pmu();
}

void pmu_end(void){
	if(evType == INVALID){
		printf("appoint a valid event type first!\n");
		return;
	}
    cntEnd = read_pmu();
    disable_pmu(evType);
    if(0 < cntEnd - cntBegin){
        evtCnt[evType]++;
    }
}

uint32_t pmu_get_score(){
	if(evType == INVALID){
		printf("appoint a valid event type first!\n");
		return 0;
	}
    uint32_t sum = 0;
    for (uint32_t i = 0; i <= 60; i++){
        sum += evtCnt[i];
    }
    return sum;
}


#endif /* PMU_UTILS_H */