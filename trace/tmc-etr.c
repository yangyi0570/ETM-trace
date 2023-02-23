#include "tmc-etr.h"


void tmc_enable_etr_sink(void *base, struct etr_config con)
{
	uint32_t axictl;
	printk(KERN_INFO "tmc_enable_etr_sink start");
    
    CS_UNLOCK(base);

    //todo: Linux中对con.vaddr进行memset

	/* Wait for TMCSReady bit to be set */
	tmc_wait_for_tmcready(base);

	iowrite32(con.size / 4, base + TMC_RSZ);
	iowrite32(TMC_MODE_CIRCULAR_BUFFER, base + TMC_MODE);

    // WrBurstLen:0xF
    // ScatterGatherMode:0
    // ProtCtrlBit0:0 
    // ProtCtrlBit1:1
	axictl = ioread32(base + TMC_AXICTL);
	axictl |= TMC_AXICTL_WR_BURST_16;
	axictl &= ~TMC_AXICTL_SCT_GAT_MODE;
	axictl = (axictl &
		  ~(TMC_AXICTL_PROT_CTL_B0 | TMC_AXICTL_PROT_CTL_B1)) |
		  TMC_AXICTL_PROT_CTL_B1;
	iowrite32(axictl, base + TMC_AXICTL);

	// iowrite32(con.paddr, base + TMC_DBALO);		// todo:加入
	iowrite32(0x0, base + TMC_DBAHI);
	iowrite32(TMC_FFCR_EN_FMT | TMC_FFCR_EN_TI |
		       TMC_FFCR_FON_FLIN | TMC_FFCR_FON_TRIG_EVT |
		       TMC_FFCR_TRIGON_TRIGIN,
		       base + TMC_FFCR);
	iowrite32(con.trigger_cntr, base + TMC_TRG);
	tmc_enable(base);

	CS_LOCK(base);
	printk(KERN_INFO "tmc_enable_etr_sink finish");
}

void tmc_disable_etr_sink(void *base, struct etr_config con)
{
    CS_UNLOCK(base);

	tmc_flush_and_stop(base);
	/*
	 * the content of the buffer needs to be
	 * read before the TMC is disabled.
	 */
	tmc_dump_etr_sink(base, con);
	tmc_disable(base);

	CS_LOCK(base);
}


// todo:读取可能存在问题，需要考虑不同Mode下的读取开始位置
void tmc_dump_etr_sink(void *base, struct etr_config con)
{
	// struct file* fp;
    // mm_segment_t fs;
    // loff_t pos;
	uint32_t result;
	// char* c = (char*)(&result);

	// fp = filp_open("/dev/cs_device", O_RDWR, 0644);
	// fs = get_fs();
    // set_fs(KERNEL_DS);	// allow write using kernel memory

	result = ioread32(base + TMC_RRD);		// RRD一次读取32位
	// pos = 0;

    while(result != 0xffffffff)
    {
		// write to /dev/cs_device

		//vfs_write(fp, c, 4, &pos);
        result = ioread32(base + TMC_RRD);
		//printk(KERN_INFO "result:%x",result);
    }

	//vfs_write(fp, c, 4, &pos);

	// restore
	// filp_close(fp, NULL);
    // set_fs(fs);

	printk(KERN_INFO "etr dump finish");
}