#ifndef __REGISTER_H
#define __REGISTER_H

/**
 * @brief refer to device tree and technical manual.
 * 
 */

/* base address of etm */
#define ETM0_BASE_ADD 		0x22040000
#define ETM1_BASE_ADD 		0x22140000
#define ETM2_BASE_ADD 		0x23040000
#define ETM3_BASE_ADD 		0x23140000
#define ETM4_BASE_ADD 		0x23240000
#define ETM5_BASE_ADD 		0x23340000

#define ETM_REGISTERS_SIZE	0x1000

/* base address of funnel */
#define CLUSTER0_FUNNEL_BASE_ADD        0x220C0000
#define CLUSTER1_FUNNEL_BASE_ADD        0x230C0000
#define MAIN_FUNNEL_BASE_ADD            0x20040000
#define CSSYS1_BASE_ADD                 0x20130000
#define CSSYS2_BASE_ADD                 0x20150000           

#define FUNNEL_REGISTERS_SIZE       0x1000


/* base address of tmc */
#define ETF0_BASE_ADD		0x20010000
#define ETF1_BASE_ADD		0x20140000
#define ETR_BASE_ADD		0x20070000

#define TMC_REGISTERS_SIZE	0x1000

/* base address of PMU */
#define PMU0_BASE_ADD 0x22030000
#define PMU1_BASE_ADD 0x22130000

#define PMU_REGISTERS_SIZE 0x1000

#endif