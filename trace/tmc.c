#include "tmc.h"


// void etf_circular_buffer(void *base)
// {
//     iowrite32(0xC5ACCE55, base + TMC_LAR);
//     uint32_t lsr = ioread32(base + TMC_LSR);
//     printk(KERN_INFO "lsr:0x%x\n",lsr);

//     // 1. Wait until TMCReady is equal to one.

//     // 2. Program the MODE Register for Circular Buffer mode.
//     iowrite32(0x0, base + TMC_MODE); // set MODE to Circular Buffer mode

//     /*
//         3. Program the FFCR Register. ARM recommends that you set the TrigOnTrigIn,
//         FOnTrigEvt, StopOnFl, EnTI, and EnFt bits. This enables formatting, inserting a trigger
//         when a trigger is observed on TRIGIN, and following a delay corresponding to the value
//         of the TRG Register, flushing and then stopping the TMC.
//     */
//     uint32_t ffcr = ioread32(base + TMC_FFCR);
//     ffcr = ffcr | TrigOnTrigIn | FOnTrigEvt | StopOnFl | EnTI | EnFt;
//     iowrite32(ffcr, base + TMC_FFCR);
//     ffcr = ioread32(base + TMC_FFCR);
//     printk(KERN_INFO "FFCR: 0x%x", ffcr);
//     /*
//         4. Program the TRG Register, to control the amount of buffer to be dedicated to the period
//         after a trigger is observed.
//     */
//     //uint32_t rsz = ioread32(base + TMC_RSZ);
//     //printk(KERN_INFO "RSZ:0x%x",rsz);    RSZ:0x1000  16KB, width of register is log2(MEMSIZE) = 12 bit
//     iowrite32(0x100, base + TMC_TRG);

//     /*
//         5. Set the TraceCaptEn bit in the CTL Register. This starts the trace session.
//     */
//     iowrite32(0x1, base + TMC_CTL);
//     uint32_t ctl = ioread32(base + TMC_CTL);
//     printk(KERN_INFO "ctl:0x%x\n",ctl);
// }



void generate_flush(void *base)
{
    uint32_t ffcr = ioread32(base + TMC_FFCR);
    ffcr = ffcr | TMC_FFCR_STOP_ON_FLUSH | TMC_FFCR_FLUSHMAN;
    iowrite32(ffcr, base + TMC_FFCR);
}


void reset_tmc(void *base)
{
    iowrite32(0x0, base + TMC_CTL);
}


void check_tmc_staus(void *base)
{
    uint32_t trace_capt_en = ioread32(base + TMC_CTL);          // 
    uint32_t tmc_ready = ioread32(base + TMC_STS) & (1 << 2);
    uint32_t ffcr = ioread32(base + TMC_FFCR);
    printk(KERN_INFO "TraceCaptEn: 0x%x, TMCReady: 0x%x, ffcr: 0x%x", trace_capt_en, tmc_ready, ffcr);
}



void tmc_enable(void *base)
{
    iowrite32(TMC_CTL_CAPT_EN, base + TMC_CTL);
}

void tmc_disable(void *base)
{
    iowrite32(0x0, base + TMC_CTL);
}

void tmc_wait_for_tmcready(void *base)
{
    uint32_t val = ioread32(base + TMC_STS);
    val &= TMC_STS_TMCREADY;
    while(val != (1 << 2))
    {
        val = ioread32(base + TMC_STS);
        val &= TMC_STS_TMCREADY;
    }
}

void tmc_flush_and_stop(void *base)
{
    uint32_t ffcr;
    uint32_t val;

    ffcr = ioread32(base + TMC_FFCR);
	ffcr |= TMC_FFCR_STOP_ON_FLUSH;
	iowrite32(ffcr, base + TMC_FFCR);
	ffcr |= TMC_FFCR_FLUSHMAN;
	iowrite32(ffcr, base + TMC_FFCR);
	/* Ensure flush completes */
    val = ioread32(base + TMC_FFCR);
    val &= TMC_FFCR_FLUSHMAN;
	while(val !=0 )
    {
        val = ioread32(base + TMC_FFCR);
        val &= TMC_FFCR_FLUSHMAN;
    }

	tmc_wait_for_tmcready(base);
}

