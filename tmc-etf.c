#include "tmc-etf.h"

void tmc_enable_etf_sink(void *base, struct etf_config con)
{
	//printk(KERN_INFO "tmc_enable_etf_sink start");
    CS_UNLOCK(base);
    tmc_wait_for_tmcready(base);
    iowrite32(TMC_MODE_CIRCULAR_BUFFER, base + TMC_MODE);
    iowrite32(TMC_FFCR_EN_FMT | TMC_FFCR_EN_TI |
		       TMC_FFCR_FON_FLIN | TMC_FFCR_FON_TRIG_EVT |
		       TMC_FFCR_TRIGON_TRIGIN,
		       base + TMC_FFCR);
    iowrite32(con.trigger_cntr, base + TMC_TRG);
    tmc_enable(base);
    CS_LOCK(base);
	//printk(KERN_INFO "tmc_enable_etf_sink finish");
}


void tmc_disable_etf_sink(void *base)
{
    CS_UNLOCK(base);
	printk(KERN_INFO "disable etf.");
    tmc_flush_and_stop(base);
	printk(KERN_INFO "flush and stop.");

	tmc_dump_etf_sink(base);
	printk(KERN_INFO "dump sink");

    tmc_disable(base);
    CS_LOCK(base);
}

void tmc_continue_etf_sink(void *base)
{
	CS_UNLOCK(base);
    tmc_enable(base);
    CS_LOCK(base);
}

void tmc_enable_etf_link(void *base)
{
	printk(KERN_INFO "tmc_enable_etf_link start");
    CS_UNLOCK(base);

	/* Wait for TMCSReady bit to be set */
	tmc_wait_for_tmcready(base);

	iowrite32(TMC_MODE_HARDWARE_FIFO, base + TMC_MODE);
	iowrite32(TMC_FFCR_EN_FMT | TMC_FFCR_EN_TI,
		       base + TMC_FFCR);
	iowrite32(0x0, base + TMC_BUFWM);
	tmc_enable(base);

	CS_LOCK(base);
	printk(KERN_INFO "tmc_enable_etf_link finish");
}

void tmc_disable_etf_link(void *base)
{
    CS_UNLOCK(base);

	tmc_flush_and_stop(base);
	tmc_disable(base);

	CS_LOCK(base);
}


void tmc_dump_etf_sink(void *base)
{
	uint32_t result;
	unsigned long flags;
	char* c = (char*)(&result);
	int data_count = 0;

	result = ioread32(base + TMC_RRD);		// RRD一次读取32位
    spin_lock_irqsave(&tbc_lock, flags);
    while(result != 0xffffffff)
    {
		// write to /dev/cs_device
		iowrite32(result, write_point);
		write_point += 4;
        result = ioread32(base + TMC_RRD);
		data_count++;
    }
	iowrite32(result, write_point);
	write_point += 4;
	data_count++;
	spin_unlock_irqrestore(&tbc_lock, flags);

	printk(KERN_INFO "etr dump finish, %d bytes data is read.", data_count * 4);
}

void tmc_info_etf(void* base)
{
	// etf RAM depth
	uint32_t tmc_rsz = ioread32(base + TMC_RSZ);
	printk(KERN_INFO "TMC_RSZ: %x\n", tmc_rsz);
}