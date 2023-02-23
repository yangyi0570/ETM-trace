#ifndef __PMU_H
#define __PMU_H

#include <linux/kernel.h>
#include <asm/io.h>
#include "register.h"


/*-------------------Performance Monitors registers-------------------*/
#define PMEVCNTR0_EL0               0x000
#define PMEVCNTR1_EL0               0x008
#define PMEVCNTR2_EL0               0x010
#define PMCCNTR_EL0                 0x0F8
#define PMEVTYPER0_EL0              0x400
#define PMEVTYPER1_EL0              0x404
#define PMCR_EL0                    0xE04
#define PMCCFILTR_EL0               0x47C
#define PMLAR                       0xFB0
#define PMCNTENSET_EL0              0xC00
#define PMCNTENCLR_EL0              0xC20
#define PMINTENSET_EL1              0xC40
#define PMINTENCLR_EL1              0xC60

// bits in PMCR_EL0
#define PMCR_X                      (1 << 4)
#define PMCR_D                      (1 << 3)
#define PMCR_C                      (1 << 2)
#define PMCR_P                      (1 << 1)
#define PMCR_E                      1

// bits in PMEVTYPER<n>_EL0
#define PMEVTYPER_P                 (1 << 31)
#define PMEVTYPER_U                 (1 << 30)    
#define PMEVTYPER_NSK               (1 << 29)
#define PMEVTYPER_NSU               (1 << 28)
#define PMEVTYPER_NSH               (1 << 27)
#define PMEVTYPER_M                 (1 << 26)
#define PMEVTYPER_MT                (1 << 25)
#define PMEVTYPER_SH                (1 << 24)

void set_pmu_export(void *base);

#endif