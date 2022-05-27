#include "etm.h"

/*
    unlock OS lock via TRCOSLAR.
    Set OSLK bit in TRCOSLAR to 0.
*/
void unlock_OS(void *base)
{
    iowrite32(0x0, base + TRCOSLAR);
}

/*
    lock OS lock via TRCOSLAR.
    Set OSLK bit in TRCOSLAR to 1.
*/
void lock_OS(void *base)
{
    iowrite32(0x1, base + TRCOSLAR); // Note: This setting disables the trace unit.
}

/*
    Set EN bit in TRCPRGCTLR to 0.
*/
void disable_unit(void *base)
{
    iowrite32(0x0, base+ TRCPRGCTLR);
}

/*
    Set EN bit in TRCPRGCTLR to 1.
*/
void enable_unit(void *base)
{
    iowrite32(0x1, base+ TRCPRGCTLR);
}
/*
    after unlock os and software,
    disable the trace unit to enable configure it.
*/
// void etm_enable_configure(void *base)
// {
//     unlock_software(base);
//     unlock_OS(base);
//     disable_unit(base);
// }

/*
    enable the trace unit to start tracing.
*/
// void etm_disable_configure(void *base)
// {
//     // lock_OS(base);
//     enable_unit(base);
//     // lock_software(base);
// }


/*void set_start_stop_address(void *base,)
{
    
}*/

/**
 * 此处只设置一个counter，实际上最多可以设置4个
 */
/*void configure_counter(void *base, uint16_t reload_value, )
{   
    uint32_t trcidr5 = ioread32(base + TRCIDR5);
    if(trcidr5 & 0x80000000)    //如果实现了递减计数器，则n=0是不可用的
    {
        
    }
    else
    {   // A53中递减计数器没实现
        iowrite32(base + TRCCNTRLDVRn(0), reload_value);
        //CNTCHAIN<n>为1，则不会递减
        iowrite32(base + TRCCNTCTLRn(0), );
    }
    
}*/

void set_trace_exception_level(void *base, uint32_t secure_level, uint32_t non_secure_level)
{
    uint32_t trcvictlr = ioread32(base + TRCVICTLR);
    uint32_t temp = (secure_level << 16) | (non_secure_level << 20);
    trcvictlr &= 0xFF00FFFF;
    trcvictlr |= temp;
}

void etm_status_show(void *base)
{
    
    uint32_t software_lock = ioread32(base + TRCLSR);
    uint32_t os_lock = ioread32(base + TRCOSLSR);
    uint32_t enable_reg = ioread32(base + TRCPRGCTLR);
    printk(KERN_INFO "software->0x%x\n", software_lock);            //0x1 unlock, 0x3 lock
    printk(KERN_INFO "os->0x%x\n", os_lock);                        //0x8 unlock, 0xA lock
    printk(KERN_INFO "enable->0x%x\n", enable_reg);                 //0x0 disable, 0x1 enable

    printk(KERN_INFO "***registers show***");
    printk(KERN_INFO "TRCPRGCTLR->0x%x", ioread32(base + TRCPRGCTLR));
    printk(KERN_INFO "TRCPROCSELR->0x%x", ioread32(base + TRCPROCSELR));
    printk(KERN_INFO "TRCSTATR->0x%x", ioread32(base + TRCSTATR));
    printk(KERN_INFO "TRCCONFIGR->0x%x", ioread32(base + TRCCONFIGR));
    printk(KERN_INFO "TRCAUXCTLR->0x%x", ioread32(base + TRCAUXCTLR));
    printk(KERN_INFO "TRCEVENTCTL0R->0x%x", ioread32(base + TRCEVENTCTL0R));
    printk(KERN_INFO "TRCEVENTCTL1R->0x%x", ioread32(base + TRCEVENTCTL1R));
    printk(KERN_INFO "TRCSTALLCTLR->0x%x", ioread32(base + TRCSTALLCTLR));
    printk(KERN_INFO "TRCTSCTLR->0x%x", ioread32(base + TRCTSCTLR));
    printk(KERN_INFO "TRCSYNCPR->0x%x", ioread32(base + TRCSYNCPR));
    printk(KERN_INFO "TRCCCCTLR->0x%x", ioread32(base + TRCCCCTLR));
    printk(KERN_INFO "TRCBBCTLR->0x%x", ioread32(base + TRCBBCTLR));
    printk(KERN_INFO "TRCTRACEIDR->0x%x", ioread32(base + TRCTRACEIDR));
    printk(KERN_INFO "TRCQCTLR->0x%x", ioread32(base + TRCQCTLR));

    printk(KERN_INFO "TRCVICTLR->0x%x", ioread32(base + TRCVICTLR));
    printk(KERN_INFO "TRCVIIECTLR->0x%x", ioread32(base + TRCVIIECTLR));
    printk(KERN_INFO "TRCVISSCTLR->0x%x", ioread32(base + TRCVISSCTLR));
    printk(KERN_INFO "TRCVIPCSSCTLR->0x%x", ioread32(base + TRCVIPCSSCTLR));
    printk(KERN_INFO "TRCVDCTLR->0x%x", ioread32(base + TRCVDCTLR));
    printk(KERN_INFO "TRCVDSACCTLR->0x%x", ioread32(base + TRCVDSACCTLR));
    printk(KERN_INFO "TRCVDARCCTLR->0x%x", ioread32(base + TRCVDARCCTLR));
    printk(KERN_INFO "TRCACVR0->0x%x", ioread32(base + 0x400));
    printk(KERN_INFO "TRCACVR1->0x%x", ioread32(base + 0x404));

    // printk(KERN_INFO "TRCIDR0->0x%x", ioread32(base + TRCIDR0));
    // printk(KERN_INFO "TRCIDR1->0x%x", ioread32(base + TRCIDR1));
    // printk(KERN_INFO "TRCIDR2->0x%x", ioread32(base + TRCIDR2));
    // printk(KERN_INFO "TRCIDR3->0x%x", ioread32(base + TRCIDR3));
    // printk(KERN_INFO "TRCIDR4->0x%x", ioread32(base + TRCIDR4));
    // printk(KERN_INFO "TRCIDR5->0x%x", ioread32(base + TRCIDR5));
    

}

void etm_disable(void *base)
{
    CS_UNLOCK(base);
    unlock_OS(base);

    disable_unit(base);
    
    lock_OS(base);
    CS_LOCK(base);
}

void etm_accept_pmu_events(void *base)
{
    uint32_t trcrsctlr2;
    //uint32_t trceventctl1r;
    //TODO: 输出TRCIDR5.NUMEXTINSEL确定能支持的外部事件数
    iowrite32(0x60, base + TRCEXTINSELR);  //TRCIDR5.NUMEXTINSEL决定了能支持多少外部事件
    trcrsctlr2 = ioread32(base + TRCRSCTLRn(2));
    iowrite32(trcrsctlr2 & 0xffff0000, base + TRCRSCTLRn(2)); //将SELECT域置为0
    iowrite32(0x2, base + TRCEVENTCTL0R);   //将2号资源作为事件0
    //trceventctl1r = ioread32(base + TRCEVENTCTL1R);
    iowrite32(0xfffffff1, base + TRCEVENTCTL1R);    //允许事件0产生追踪元素
}


void etm_enable_trace_program_flow(void *base)
{
    printk(KERN_INFO "etm_enable_trace_program_flow start");
    CS_UNLOCK(base);
    unlock_OS(base);
    disable_unit(base);
    
    //etm_status_show(base);
    iowrite32(0x1, base + TRCCONFIGR);
    iowrite32(0x0, base + TRCEVENTCTL0R);
    iowrite32(0x0, base + TRCEVENTCTL1R);
    iowrite32(0x0, base + TRCSTALLCTLR);
    iowrite32(0xC, base + TRCSYNCPR);
    iowrite32(0x6, base + TRCTRACEIDR); //trace id
    iowrite32(0x0, base + TRCTSCTLR);
    iowrite32(0x201, base + TRCVICTLR);
    iowrite32(0x1, base + TRCVIIECTLR);
    iowrite32(0x0, base + TRCVISSCTLR);
    iowrite32(0x0, base + TRCBBCTLR);

    //pmu使能相关
    //etm_accept_pmu_events(base);

    //在etm单元使能前需要保证pmu已经使能完成
    enable_unit(base);
    etm_status_show(base);
    lock_OS(base);
    CS_LOCK(base);
    printk(KERN_INFO "etm_enable_trace_program_flow finish");
}


void etm_enable_trace_instruction_and_data(void* base)
{
    CS_UNLOCK(base);
    unlock_OS(base);
    disable_unit(base);

    etm_status_show(base);
    iowrite32(0x00031FC7, base + TRCCONFIGR);
    iowrite32(0x0, base + TRCEVENTCTL0R);
    iowrite32(0x0, base + TRCEVENTCTL1R);
    iowrite32(0x0, base + TRCSTALLCTLR);
    iowrite32(0xC, base + TRCSYNCPR);
    iowrite32(0x6, base + TRCTRACEIDR); //trace id
    iowrite32(0x0, base + TRCTSCTLR);

    iowrite32(0x0, base + TRCVDARCCTLR);
    iowrite32(0x1, base + TRCVDCTLR);
    iowrite32(0x0, base + TRCVDSACCTLR);

    iowrite32(0x201, base + TRCVICTLR);
    iowrite32(0x0, base + TRCVIIECTLR);
    iowrite32(0x0, base + TRCVISSCTLR);

    enable_unit(base);
    lock_OS(base);
    CS_LOCK(base);
}
