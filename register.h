#ifndef __REGISTER_H
#define __REGISTER_H


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








/*---------------------------------ETM-----------------------------------*/

// ETM registers' offset

// #define  TRCPRGCTLR		            0x004
// #define  TRCSTATR		            0x00C
// #define  TRCCONFIGR		            0x010
// #define  TRCAUXCTLR		            0x018
// #define  TRCEVENTCTL0R		        0x020
// #define  TRCEVENTCTL1R		        0x024
// #define  TRCSTALLCTLR               0x02C
// #define  TRCTSCTLR		            0x030
// #define  TRCSYNCPR		            0x034
// #define  TRCCCCTLR		            0x038
// #define  TRCBBCTLR		            0x03C
// #define  TRCTRACEIDR		        0x040
// #define  TRCVICTLR		            0x080
// #define  TRCVIIECTLR		        0x084
// #define  TRCVISSCTLR		        0x088
// #define  TRCSEQEVR0		            0x100
// #define  TRCSEQEVR1		            0x104
// #define  TRCSEQEVR2		            0x108
// #define  TRCSEQRSTEVR		        0x118
// #define  TRCSEQSTR		            0x11C
// #define  TRCEXTINSELR		        0x120
// #define  TRCCNTRLDVR0		        0x140
// #define  TRCCNTRLDVR1		        0x144
// #define  TRCCNTCTLR0		        0x150
// #define  TRCCNTCTLR1		        0x154
// #define  TRCCNTVR0		            0x160
// #define  TRCCNTVR1		            0x164
// #define  TRCIDR8		            0x180
// #define  TRCIDR9		            0x184
// #define  TRCIDR10		            0x188
// #define  TRCIDR11		            0x18C
// #define  TRCIDR12		            0x190
// #define  TRCIDR13		            0x194
// #define  TRCIMSPEC0		            0x1C0
// #define  TRCIDR0		            0x1E0
// #define  TRCIDR1		            0x1E4
// #define  TRCIDR2		            0x1E8
// #define  TRCIDR3		            0x1EC
// #define  TRCIDR4		            0x1F0
// #define  TRCIDR5		            0x1F4
// #define  TRCRSCTLR2		            0x208
// #define  TRCSSCCR0		            0x280
// #define  TRCSSCSR0		            0x2A0
// #define  TRCOSLAR		            0x300
// #define  TRCOSLSR		            0x304
// #define  TRCPDCR		            0x310
// #define  TRCPDSR		            0x314
// #define  TRCACVRn		            0x400
// #define  TRCACATRn		            0x480
// #define  TRCCIDCVR0		            0x600
// #define  TRCVMIDCVR0		        0x640
// #define  TRCCIDCCTLR0		        0x680
// #define  TRCVMIDCCTRL0		        0x688
// #define  TRCITMISCOUT		        0xEDC
// #define  TRCITMISCIN		        0xEE0
// #define  TRCITATBDATA0		        0xEEC
// #define  TRCITATBCTR2		        0xEF0
// #define  TRCITATBCTR1		        0xEF4
// #define  TRCITATBCTR0		        0xEF8
// #define  TRCITCTRL		            0xF00
// #define  TRCCLAIMSET		        0xFA0
// #define  TRCCLAIMCLR		        0xFA4
// #define  TRCDEVAFF0		            0xFA8
// #define  TRCDEVAFF1		            0xFAC
// #define  TRCLAR			            0xFB0
// #define  TRCLSR			            0xFB4
// #define  TRCAUTHSTATUS		        0xFB8
// #define  TRCDEVARCH		            0xFBC
// #define  TRCDEVID		            0xFC8
// #define  TRCDEVTYPE		            0xFCC
// #define  TRCPIDR0		            0xFE0
// #define  TRCPIDR1		            0xFE4
// #define  TRCPIDR2		            0xFE8
// #define  TRCPIDR3		            0xFEC
// #define  TRCPIDR4		            0xFD0
// //#define  TRCPIDRn		None
// #define  TRCCIDR0		            0xFF0
// #define  TRCCIDR1		            0xFF4
// #define  TRCCIDR2		            0xFF8
// #define  TRCCIDR3		            0xFFC



#endif