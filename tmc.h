#ifndef __TMC_H
#define __TMC_H

#include <linux/kernel.h>
#include <asm/io.h>
#include "register.h"
#include "coresight_priv.h"


/*-----------------register---------------------*/
// ETB@1 base address


#define TMC_RSZ		                0x004
#define TMC_STS		                0x00C
#define TMC_RRD		                0x010
#define TMC_RRP		                0x014
#define TMC_RWP		                0x018
#define TMC_TRG		                0x01C
#define TMC_CTL		                0x020
#define TMC_RWD		                0x024
#define TMC_MODE                    0x028
#define TMC_LBUFLEVEL				0x02c
#define TMC_CBUFLEVEL				0x030
#define TMC_BUFWM					0x034
#define TMC_RRPHI					0x038
#define TMC_RWPHI					0x03c
#define TMC_AXICTL					0x110
#define TMC_DBALO					0x118
#define TMC_DBAHI					0x11c
#define TMC_FFSR		            0x300
#define TMC_FFCR		            0x304
#define TMC_ITMISCOP0		        0xEE0
#define TMC_ITTRFLINACK		        0xEE4
#define TMC_ITTRFLIN		        0xEE8
#define TMC_ITATBDATA0		        0xEEC
#define TMC_ITATBCTR2		        0xEF0
#define TMC_ITATBCTR1		        0xEF4
#define TMC_ITATBCTR0		        0xEF8
#define TMC_LAR                     0xFB0
#define TMC_LSR                     0xFB4


// Bits in FFCR
#define TMC_FFCR_EN_FMT		1
#define TMC_FFCR_EN_TI		(1 << 1)
#define TMC_FFCR_FON_FLIN	(1 << 4)
#define TMC_FFCR_FON_TRIG_EVT	(1 << 5)
#define TMC_FFCR_FLUSHMAN       (1 << 6)
#define TMC_FFCR_TRIGON_TRIGIN	(1 << 8)
#define TMC_FFCR_STOP_ON_FLUSH	(1 << 12)

// Bits in CTL
#define TMC_CTL_CAPT_EN		                1
// Bits in STS
#define TMC_STS_FULL                        1
#define TMC_STS_TRIGGERED                   (1 << 1)
#define TMC_STS_TMCREADY                    (1 << 2)

/* TMC_AXICTL - 0x110 */
#define TMC_AXICTL_PROT_CTL_B0				1
#define TMC_AXICTL_PROT_CTL_B1				(1 << 1)
#define TMC_AXICTL_SCT_GAT_MODE				(1 << 7)
#define TMC_AXICTL_WR_BURST_16				0xF00
/*------------------------------------------------*/


enum tmc_mode {
	TMC_MODE_CIRCULAR_BUFFER,
	TMC_MODE_SOFTWARE_FIFO,
	TMC_MODE_HARDWARE_FIFO,
};

// void etf_circular_buffer(void *base);
// void generate_flush(void *base);
// void check_tmc_staus(void *base);
// void reset_tmc(void *base);

void tmc_enable(void *base);
void tmc_disable(void *base);
void tmc_wait_for_tmcready(void *base);
void tmc_flush_and_stop(void *base);

#endif 