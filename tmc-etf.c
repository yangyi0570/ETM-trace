#include "tmc-etf.h"

void tmc_enable_etf_sink(void *base, struct etf_config con)
{
	printk(KERN_INFO "tmc_enable_etf_sink start");
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
	printk(KERN_INFO "tmc_enable_etf_sink finish");
}


void tmc_disable_etf_sink(void *base)
{
    CS_UNLOCK(base);
    tmc_flush_and_stop(base);

	tmc_dump_etf_sink(base);

    tmc_disable(base);
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
	struct file* fp;
    mm_segment_t fs;
    loff_t pos;
	uint32_t result;
	char* c = (char*)(&result);

	fp = filp_open("/dev/cs_device", O_RDWR, 0644);
	fs = get_fs();
    set_fs(KERNEL_DS);	// allow write using kernel memory

	result = ioread32(base + TMC_RRD);		// RRD一次读取32位
	pos = 0;

    while(result != 0xffffffff)
    {
		// write to /dev/cs_device

		vfs_write(fp, c, 4, &pos);
        result = ioread32(base + TMC_RRD);
		//printk(KERN_INFO "result:%x",result);
    }

	vfs_write(fp, c, 4, &pos);

	// restore
	filp_close(fp, NULL);
    set_fs(fs);

	printk(KERN_INFO "etr dump finish");
}

void tmc_info_etf(void* base){
	// etf RAM depth
	uint32_t tmc_rsz = ioread32(base + TMC_RSZ);
	printk(KERN_INFO "TMC_RSZ: %x\n", tmc_rsz);
}