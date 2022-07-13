#include "connect.h"


// void trace_pmu_event(void *debug)
// {
//     struct connect_param *param = (struct connect_param *)debug;
//     uint32_t extinselr = ioread32(param->etm_register + TRCEXTINSELR);
//     extinselr = extinselr & 0xFFFFFF00;
//     extinselr = extinselr | 0x0D;       //系统调用为0x0082  p8006
//     iowrite32(extinselr, param->etm_register + TRCEXTINSELR);
    
//     // select the external input 0 as tracing resource 2
//     uint32_t trcrsctlr2 = ioread32(param->etm_register + TRCRSCTLR2);
//     trcrsctlr2 = trcrsctlr2 & 0xFFFF0000;  // SELECT设置为0
//     iowrite32(trcrsctlr2, param->etm_register + TRCRSCTLR2);
    
//     // select the resource 2 as event 0
//     uint32_t trceventctl0r = ioread32(param->etm_register + TRCEVENTCTL0R);
//     trceventctl0r = trceventctl0r & 0xFFFFFF00;
//     trceventctl0r = trceventctl0r | 0x2;
//     iowrite32(trceventctl0r, param->etm_register + TRCEVENTCTL0R);

//     // enable event 0
//     uint32_t trceventctl1r = ioread32(param->etm_register + TRCEVENTCTL1R);
//     trceventctl1r = trceventctl1r & 0xFFFFFFF0;
//     trceventctl1r = trceventctl1r | 0x1;
//     iowrite32(trceventctl1r, param->etm_register + TRCEVENTCTL1R);

//     configure_pmu(param->pmu_register);
    
// }

void enable_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    struct etf_config etf_con = {0x100};
    struct funnel_config main_funnel_con = {0, 0xFAC688};
    struct funnel_config cluster0_funnel_con = {1, 0xFAC688}; 
    //struct funnel_config cssys2_con = {0, 0xFAC688};

    //is_Q_element_implement(param->etm0_register);
    tmc_enable_etf_sink(param->etf0_register, etf_con);
    funnel_enable(param->main_funnel_register, main_funnel_con);   // todo:struct funnel_config con需要配置
    funnel_enable(param->cluster0_funnel_register, cluster0_funnel_con);
    
    set_pmu_export(param->pmu1_register);
    //调用不同的etm使能函数来确定是否实现数据跟踪（但实际上hikey970上没有数据跟踪）
    etm_enable_trace_program_flow(param->etm1_register);
}

void disable_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    //struct etr_config etr_con = {0x00001000, 0x10086, 0x100};

    etm_disable(param->etm1_register);
    funnel_disable(param->cluster0_funnel_register);
    funnel_disable(param->main_funnel_register);
    tmc_disable_etf_sink(param->etf0_register);


    printk(KERN_INFO "disable trace and reset!");

}


// todo: dump the data









    // uint32_t software_lock;
    // uint32_t os_lock;
    // uint32_t enable_reg;
    // software_lock = ioread32(param->etm_register + TRCLSR);
    // os_lock = ioread32(param->etm_register + TRCOSLSR);
    // enable_reg = ioread32(param->etm_register + TRCPRGCTLR);
    // printk(KERN_INFO "lock: software->0x%x\n", software_lock);
    // printk(KERN_INFO "lock: os->0x%x\n", os_lock);
    // printk(KERN_INFO "lock: enable->0x%x\n", enable_reg);
