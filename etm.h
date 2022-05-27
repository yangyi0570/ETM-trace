#ifndef __ETM_H
#define __ETM_H

#include <linux/kernel.h>
#include <asm/io.h>
#include "register.h"
#include "coresight_priv.h"
#include "pmu.h"


/*
 * Device registers:
 * 0x000 - 0x2FC: Trace		registers
 * 0x300 - 0x314: Management	registers
 * 0x318 - 0xEFC: Trace		registers
 * 0xF00: Management		registers
 * 0xFA0 - 0xFA4: Trace		registers
 * 0xFA8 - 0xFFC: Management	registers
 */
/* Trace registers (0x000-0x2FC) */
/* Main control and configuration registers */
#define TRCPRGCTLR			0x004
#define TRCPROCSELR			0x008
#define TRCSTATR			0x00C
#define TRCCONFIGR			0x010
#define TRCAUXCTLR			0x018
#define TRCEVENTCTL0R			0x020
#define TRCEVENTCTL1R			0x024
#define TRCSTALLCTLR			0x02C
#define TRCTSCTLR			0x030
#define TRCSYNCPR			0x034
#define TRCCCCTLR			0x038
#define TRCBBCTLR			0x03C
#define TRCTRACEIDR			0x040
#define TRCQCTLR			0x044
/* Filtering control registers */
#define TRCVICTLR			0x080
#define TRCVIIECTLR			0x084
#define TRCVISSCTLR			0x088
#define TRCVIPCSSCTLR			0x08C
#define TRCVDCTLR			0x0A0
#define TRCVDSACCTLR			0x0A4
#define TRCVDARCCTLR			0x0A8
/* Derived resources registers */
#define TRCSEQEVRn(n)			(0x100 + (n * 4))
#define TRCSEQRSTEVR			0x118
#define TRCSEQSTR			0x11C
#define TRCEXTINSELR			0x120
#define TRCCNTRLDVRn(n)			(0x140 + (n * 4))
#define TRCCNTCTLRn(n)			(0x150 + (n * 4))
#define TRCCNTVRn(n)			(0x160 + (n * 4))
/* ID registers */
#define TRCIDR8				0x180
#define TRCIDR9				0x184
#define TRCIDR10			0x188
#define TRCIDR11			0x18C
#define TRCIDR12			0x190
#define TRCIDR13			0x194
#define TRCIMSPEC0			0x1C0
#define TRCIMSPECn(n)			(0x1C0 + (n * 4))
#define TRCIDR0				0x1E0
#define TRCIDR1				0x1E4
#define TRCIDR2				0x1E8
#define TRCIDR3				0x1EC
#define TRCIDR4				0x1F0
#define TRCIDR5				0x1F4
#define TRCIDR6				0x1F8
#define TRCIDR7				0x1FC
/* Resource selection registers */
#define TRCRSCTLRn(n)			(0x200 + (n * 4))
/* Single-shot comparator registers */
#define TRCSSCCRn(n)			(0x280 + (n * 4))
#define TRCSSCSRn(n)			(0x2A0 + (n * 4))
#define TRCSSPCICRn(n)			(0x2C0 + (n * 4))
/* Management registers (0x300-0x314) */
#define TRCOSLAR			0x300
#define TRCOSLSR			0x304
#define TRCPDCR				0x310
#define TRCPDSR				0x314
/* Trace registers (0x318-0xEFC) */
/* Comparator registers */
#define TRCACVRn(n)			(0x400 + (n * 8))
#define TRCACATRn(n)			(0x480 + (n * 8))
#define TRCDVCVRn(n)			(0x500 + (n * 16))
#define TRCDVCMRn(n)			(0x580 + (n * 16))
#define TRCCIDCVRn(n)			(0x600 + (n * 8))
#define TRCVMIDCVRn(n)			(0x640 + (n * 8))
#define TRCCIDCCTLR0			0x680
#define TRCCIDCCTLR1			0x684
#define TRCVMIDCCTLR0			0x688
#define TRCVMIDCCTLR1			0x68C
/* Management register (0xF00) */
/* Integration control registers */
#define TRCITCTRL			0xF00
/* Trace registers (0xFA0-0xFA4) */
/* Claim tag registers */
#define TRCCLAIMSET			0xFA0
#define TRCCLAIMCLR			0xFA4
/* Management registers (0xFA8-0xFFC) */
#define TRCDEVAFF0			0xFA8
#define TRCDEVAFF1			0xFAC
#define TRCLAR				0xFB0
#define TRCLSR				0xFB4
#define TRCAUTHSTATUS			0xFB8
#define TRCDEVARCH			0xFBC
#define TRCDEVID			0xFC8
#define TRCDEVTYPE			0xFCC
#define TRCPIDR4			0xFD0
#define TRCPIDR5			0xFD4
#define TRCPIDR6			0xFD8
#define TRCPIDR7			0xFDC
#define TRCPIDR0			0xFE0
#define TRCPIDR1			0xFE4
#define TRCPIDR2			0xFE8
#define TRCPIDR3			0xFEC
#define TRCCIDR0			0xFF0
#define TRCCIDR1			0xFF4
#define TRCCIDR2			0xFF8
#define TRCCIDR3			0xFFC

/* ETMv4 resources */
#define ETM_MAX_NR_PE			8
#define ETMv4_MAX_CNTR			4
#define ETM_MAX_SEQ_STATES		4
#define ETM_MAX_EXT_INP_SEL		4
#define ETM_MAX_EXT_INP			256
#define ETM_MAX_EXT_OUT			4
#define ETM_MAX_SINGLE_ADDR_CMP		16
#define ETM_MAX_ADDR_RANGE_CMP		(ETM_MAX_SINGLE_ADDR_CMP / 2)
#define ETM_MAX_DATA_VAL_CMP		8
#define ETMv4_MAX_CTXID_CMP		8
#define ETM_MAX_VMID_CMP		8
#define ETM_MAX_PE_CMP			8
#define ETM_MAX_RES_SEL			16
#define ETM_MAX_SS_CMP			8

#define ETM_ARCH_V4			0x40
#define ETMv4_SYNC_MASK			0x1F
#define ETM_CYC_THRESHOLD_MASK		0xFFF
#define ETMv4_EVENT_MASK		0xFF
#define ETM_CNTR_MAX_VAL		0xFFFF
#define ETM_TRACEID_MASK		0x3f

/* ETMv4 programming modes */
#define ETM_MODE_EXCLUDE		(1 << 0)
#define ETM_MODE_LOAD			(1 << 1)
#define ETM_MODE_STORE			(1 << 2)
#define ETM_MODE_LOAD_STORE		(1 << 3)
#define ETM_MODE_BB			(1 << 4)
#define ETMv4_MODE_CYCACC		(1 << 5)
#define ETMv4_MODE_CTXID		(1 << 6)
#define ETM_MODE_VMID			(1 << 7)
#define ETM_MODE_COND(val)		BMVAL(val, 8, 10)
#define ETMv4_MODE_TIMESTAMP		(1 << 11)
#define ETM_MODE_RETURNSTACK		(1 << 12)
#define ETM_MODE_QELEM(val)		BMVAL(val, 13, 14)
#define ETM_MODE_DATA_TRACE_ADDR	(1 << 15)
#define ETM_MODE_DATA_TRACE_VAL		(1 << 16)
#define ETM_MODE_ISTALL			(1 << 17)
#define ETM_MODE_DSTALL			(1 << 18)
#define ETM_MODE_ATB_TRIGGER		(1 << 19)
#define ETM_MODE_LPOVERRIDE		(1 << 20)
#define ETM_MODE_ISTALL_EN		(1 << 21)
#define ETM_MODE_DSTALL_EN		(1 << 22)
#define ETM_MODE_INSTPRIO		(1 << 23)
#define ETM_MODE_NOOVERFLOW		(1 << 24)
#define ETM_MODE_TRACE_RESET		(1 << 25)
#define ETM_MODE_TRACE_ERR		(1 << 26)
#define ETM_MODE_VIEWINST_STARTSTOP	(1 << 27)
#define ETMv4_MODE_ALL			(GENMASK(27, 0) | \
					 ETM_MODE_EXCL_KERN | \
					 ETM_MODE_EXCL_USER)

#define TRCSTATR_IDLE_BIT		0
#define ETM_DEFAULT_ADDR_COMP		0

/* PowerDown Control Register bits */
#define TRCPDCR_PU			(1 << 3)

/* secure state access levels */
#define ETM_EXLEVEL_S_APP		(1 << 8)
#define ETM_EXLEVEL_S_OS		(1 << 9)
#define ETM_EXLEVEL_S_NA		(1 << 10)
#define ETM_EXLEVEL_S_HYP		(1 << 11)
/* non-secure state access levels */
#define ETM_EXLEVEL_NS_APP		(1 << 12)
#define ETM_EXLEVEL_NS_OS		(1 << 13)
#define ETM_EXLEVEL_NS_HYP		(1 << 14)
#define ETM_EXLEVEL_NS_NA		(1 << 15)

/* Address comparator access types */
enum etm_addr_acctype {
	ETM_INSTR_ADDR,
	ETM_DATA_LOAD_ADDR,
	ETM_DATA_STORE_ADDR,
	ETM_DATA_LOAD_STORE_ADDR,
};

/* Address comparator context types */
enum etm_addr_ctxtype {
	ETM_CTX_NONE,
	ETM_CTX_CTXID,
	ETM_CTX_VMID,
	ETM_CTX_CTXID_VMID,
};


//void etm4_config_trace_mode(struct etmv4_config *config);


// void unlock_OS(void *base);
// void lock_OS(void *base);
// void unlock_software(void *base);
// void lock_software(void *base);
// void disable_unit(void *base);
// void enable_unit(void *base);
// void init(void *base);
// void finish(void *base);
// void etm_status_show(void *base);

void etm_enable_trace_program_flow(void *base);
void etm_enable_trace_instruction_and_data(void* base);
void etm_disable(void *base);


#endif