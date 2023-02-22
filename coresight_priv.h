#ifndef _CORESIGHT_PRIV_H
#define _CORESIGHT_PRIV_H

#include "register.h"
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/spinlock.h>

extern spinlock_t tbc_lock;
extern void __iomem* write_point;


/*--------------------------- DEBUG REGISTERS ------------------------*/

// Offsets of debug registers
#define DBGDTRRX_OFFSET                 0x80
#define EDITR_OFFSET                    0x84
#define EDSCR_OFFSET                    0x88
#define DBGDTRTX_OFFSET                 0x8C
#define EDRCR_OFFSET                    0x90
#define OSLAR_OFFSET                    0x300
#define EDLAR_OFFSET                    0xFB0

// Bits in EDSCR
#define STATUS                          (0x3f)
#define EL                              (0x300)
#define ERR                             (1 <<  6)
#define HDE				                (1 << 14)
#define SSD                             (1 << 16)
#define ITE                             (1 << 24)
#define TXfull                          (1 << 29)
#define RXfull                          (1 << 30)

// Bits in EDRCR
#define CSE                             (1 << 2)

// Values of EDSCR.STATUS
#define NON_DEBUG                       0x2
#define HLT_BY_DEBUG_REQUEST            0x13

/*--------------------------- CTI REGISTERS ------------------------*/

// Offsets of cross trigger registers
#define CTICONTROL_OFFSET               0x0
#define CTIINTACK_OFFSET                0x10
#define CTIAPPPULSE_OFFSET              0x1C
#define CTIOUTEN0_OFFSET                0xA0
#define CTIOUTEN1_OFFSET                0xA4
#define CTITRIGOUTSTATUS_OFFSET         0x134
#define CTIGATE_OFFSET                  0x140
#define CTIDEVID_OFFSET                 0xFC8

// Bits in CTICONTROL
#define GLBEN                           (1 << 0)

// Bits in CTIDEVID
#define NUMCHAN 0x3f0000
#define NUMTRIG 0x3f00

// Bits in CTIINTACK
#define ACK0                            (1 <<  0)
#define ACK1                            (1 <<  1)

// Bits in CTIAPPPULSE
#define APPPULSE0                       (1 <<  0)
#define APPPULSE1                       (1 <<  1)

// Bits in CTIOUTEN<n>
#define OUTEN0                          (1 <<  0)
#define OUTEN1                          (1 <<  1)

// Bits in CTITRIGOUTSTATUS
#define TROUT0                          (1 <<  0)
#define TROUT1                          (1 <<  1)

// Bits in CTIGATE
#define GATE0                           (1 <<  0)
#define GATE1                           (1 <<  1)


#define CORESIGHT_ITCTRL	0xf00
#define CORESIGHT_CLAIMSET	0xfa0
#define CORESIGHT_CLAIMCLR	0xfa4
#define CORESIGHT_LAR		0xfb0
#define CORESIGHT_LSR		0xfb4
#define CORESIGHT_AUTHSTATUS	0xfb8
#define CORESIGHT_DEVID		0xfc8
#define CORESIGHT_DEVTYPE	0xfcc



static inline void CS_LOCK(void __iomem *base)
{
	do {
		/* Wait for things to settle */
		mb();	//内存屏障
		iowrite32(0x0, base + CORESIGHT_LAR);
	} while (0);
	//iowrite32(0x0, base + CORESIGHT_LAR);
}

static inline void CS_UNLOCK(void __iomem *base)
{
	do {
		iowrite32(0xC5ACCE55, base + CORESIGHT_LAR);
		/* Make sure everyone has seen this */
		mb();
	} while (0);	

	//iowrite32(0xC5ACCE55, base + CORESIGHT_LAR);
}


#endif