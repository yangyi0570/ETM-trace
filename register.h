#ifndef __REGISTER_H
#define __REGISTER_H

/**
 * @brief refer to device tree and technical manual.
 * 
 */

/* junor2 coresight component base address */
#ifdef JUNOR2

/* base address of etm */
#define ETM0_BASE_ADD 		0x22040000
#define ETM1_BASE_ADD 		0x22140000
#define ETM2_BASE_ADD 		0x23040000
#define ETM3_BASE_ADD 		0x23140000
#define ETM4_BASE_ADD 		0x23240000
#define ETM5_BASE_ADD 		0x23340000

/* base address of funnel */
#define CLUSTER0_FUNNEL_BASE_ADD        0x220C0000
#define CLUSTER1_FUNNEL_BASE_ADD        0x230C0000
#define MAIN_FUNNEL_BASE_ADD            0x20040000
#define CSSYS1_BASE_ADD                 0x20130000
#define CSSYS2_BASE_ADD                 0x20150000           

/* base address of tmc */
#define ETF0_BASE_ADD		0x20010000
#define ETF1_BASE_ADD		0x20140000
#define ETR_BASE_ADD		0x20070000

/* base address of PMU */
#define PMU0_BASE_ADD 		0x22030000
#define PMU1_BASE_ADD 		0x22130000
#define PMU2_BASE_ADD 		0x23030000
#define PMU3_BASE_ADD 		0x23130000
#define PMU4_BASE_ADD 		0x23230000
#define PMU5_BASE_ADD 		0x23330000

#endif




/* hikey970 coresight component base address */
#ifdef HIKEY970

/* base address of etm */
#define ETM0_BASE_ADD 		0xECC40000
#define ETM1_BASE_ADD 		0xECD40000
#define ETM2_BASE_ADD 		0xECE40000
#define ETM3_BASE_ADD 		0xECF40000
#define ETM4_BASE_ADD 		0xED440000
#define ETM5_BASE_ADD 		0xED540000
#define ETM6_BASE_ADD 		0xED640000
#define ETM7_BASE_ADD 		0xED740000

/* base address of funnel */
#define FUNNEL0_BASE_ADD            0xEC801000
#define FUNNEL1_BASE_ADD            0xED001000
#define FUNNEL2_BASE_ADD            0xEC031000

/* base address of tmc */
#define ETB0_BASE_ADD		0xEC802000
#define ETB1_BASE_ADD		0xED002000
#define ETB2_BASE_ADD		0xEC036000
#define ETR0_BASE_ADD		0xEC033000

/* base address of PMU */
#define PMU0_BASE_ADD 		0xECC30000
#define PMU1_BASE_ADD 		0xECD30000
#define PMU2_BASE_ADD 		0xECE30000
#define PMU3_BASE_ADD 		0xECF30000
#define PMU4_BASE_ADD 		0xED430000
#define PMU5_BASE_ADD 		0xED530000
#define PMU6_BASE_ADD 		0xED630000
#define PMU7_BASE_ADD 		0xED730000 


#endif

/* registers size*/
#define ETM_REGISTERS_SIZE	    0x1000
#define FUNNEL_REGISTERS_SIZE   0x1000
#define TMC_REGISTERS_SIZE	    0x1000
#define PMU_REGISTERS_SIZE      0x1000



#endif