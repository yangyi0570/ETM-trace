#include "etm.h"

/**
 * @brief unlock OS lock via TRCOSLAR. Set OSLK bit in TRCOSLAR to 0.
 * 
 * @param base 
 */
void unlock_OS(void *base)
{
    iowrite32(0x0, base + TRCOSLAR);
}

/**
 * @brief lock OS lock via TRCOSLAR. Set OSLK bit in TRCOSLAR to 1.
 * 
 * @param base 
 */
void lock_OS(void *base)
{
    iowrite32(0x1, base + TRCOSLAR); // Note: This setting disables the trace unit.
}

/**
 * @brief Set EN bit in TRCPRGCTLR to 0.
 * 
 * @param base 
 */
void disable_unit(void *base)
{
    iowrite32(0x0, base+ TRCPRGCTLR);
}

/**
 * @brief Set EN bit in TRCPRGCTLR to 1.
 * 
 * @param base 
 */
void enable_unit(void *base)
{
    iowrite32(0x1, base+ TRCPRGCTLR);
}

void set_trace_exception_level(void *base, uint32_t secure_level, uint32_t non_secure_level)
{
    uint32_t trcvictlr = ioread32(base + TRCVICTLR);
    uint32_t temp = (secure_level << 16) | (non_secure_level << 20);
    trcvictlr &= 0xFF00FFFF;
    trcvictlr |= temp;
}

/**
 * @brief Set the wanted PMU event export to trace flow.
 * 
 * @param base 
 */
void etm_accept_pmu_events(void *base)
{

    //0x14 事件对应于cycle
    //17号事件为分支预测 17+4=21对应于实际写入etm中的事件号
    //16号事件为周期 16+4=20
    iowrite32(0x00001415, base + TRCEXTINSELR);
    //trcrsctlr2 = ioread32(base + TRCRSCTLRn(2));
    //iowrite32(trcrsctlr2 & 0xffff0000, base + TRCRSCTLRn(2)); //将SELECT域置为0
    iowrite32(0x1, base + TRCRSCTLRn(2));   //将group设为0，select设为0号外部事件
    iowrite32(0x2, base + TRCRSCTLRn(4));   //将group设为0，select设为1号外部事件
    iowrite32(0x00000402, base + TRCEVENTCTL0R);   //将2号资源作为事件0，将4号资源作为事件1
    //trceventctl1r = ioread32(base + TRCEVENTCTL1R);
    iowrite32(0x00000003, base + TRCEVENTCTL1R);    //允许事件0和事件1产生追踪元素
}

/**
 * @brief Enable etm to trace program flow.
 * 
 * @param base 
 */
void etm_enable_trace_program_flow(void *base)
{
    printk(KERN_INFO "etm_enable_trace_program_flow start");
    CS_UNLOCK(base);
    unlock_OS(base);
    disable_unit(base);
    
    //etm_status_show(base);
    iowrite32(ETM_MODE_DEFAULT, base + TRCCONFIGR);
    iowrite32(0x0, base + TRCSTALLCTLR);
    iowrite32(0xC, base + TRCSYNCPR);
    iowrite32(0x6, base + TRCTRACEIDR); //trace id
    iowrite32(0x0, base + TRCTSCTLR);
    iowrite32(ETM_VI_DEFAULT | ETM_EXLEVEL_S_EL0 |
            ETM_EXLEVEL_S_EL1 | ETM_EXLEVEL_S_EL2 |
            ETM_EXLEVEL_S_EL3 | ETM_EXLEVEL_NS_EL1 |
            ETM_EXLEVEL_NS_EL2, base + TRCVICTLR); // only trace non-secure EL0
    iowrite32(0x0, base + TRCVIIECTLR);    // no address filtering
    iowrite32(0x0, base + TRCVISSCTLR);
    iowrite32(0x0, base + TRCBBCTLR);

    //pmu使能相关
    etm_accept_pmu_events(base);

    //在etm单元使能前需要保证pmu已经使能完成
    enable_unit(base);
    etm_status_show(base);
    lock_OS(base);
    CS_LOCK(base);
    printk(KERN_INFO "etm_enable_trace_program_flow finish");
}

/**
 * @brief Enable etm to trace program flow and data flow.
 * 
 * @param base 
 */
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

/**
 * @brief disable etm.
 * 
 * @param base 
 */
void etm_disable(void *base)
{
    CS_UNLOCK(base);
    unlock_OS(base);

    disable_unit(base);

    lock_OS(base);
    CS_LOCK(base);
}

/**
 * @brief Show the registers value of etm.
 * 
 * @param base 
 */
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

    printk(KERN_INFO "TRCIDR0->0x%x", ioread32(base + TRCIDR0));
    printk(KERN_INFO "TRCIDR1->0x%x", ioread32(base + TRCIDR1));
    printk(KERN_INFO "TRCIDR2->0x%x", ioread32(base + TRCIDR2));
    printk(KERN_INFO "TRCIDR3->0x%x", ioread32(base + TRCIDR3));
    printk(KERN_INFO "TRCIDR4->0x%x", ioread32(base + TRCIDR4));
    printk(KERN_INFO "TRCIDR5->0x%x", ioread32(base + TRCIDR5));
}
