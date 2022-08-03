#include "connect.h"

void enable_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    struct etf_config etf_con = {0x100};
    struct funnel_config main_funnel_con = {0, 0xFAC688};
    struct funnel_config cluster0_funnel_con = {1, 0xFAC688}; 

    tmc_enable_etf_sink(param->etf0_register, etf_con);
    funnel_enable(param->main_funnel_register, main_funnel_con);
    funnel_enable(param->cluster0_funnel_register, cluster0_funnel_con);
    
    set_pmu_export(param->pmu1_register);
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