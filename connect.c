#include "connect.h"

#ifdef JUNOR2
void enable_junor2_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    /* funnel and tmc configuration */
    struct etf_config etf_con = {0x100};
    struct funnel_config main_funnel_con = {0, 0xFAC688};
    struct funnel_config cluster0_funnel_con = {1, 0xFAC688}; 

    tmc_enable_etf_sink(param->etf0_register, etf_con);
    funnel_enable(param->main_funnel_register, main_funnel_con);
    funnel_enable(param->cluster0_funnel_register, cluster0_funnel_con);
    
    set_pmu_export(param->pmu1_register);
    etm_enable_trace_program_flow(param->etm1_register);
}

void disable_junor2_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;

    etm_disable(param->etm1_register);
    funnel_disable(param->cluster0_funnel_register);
    funnel_disable(param->main_funnel_register);
    tmc_disable_etf_sink(param->etf0_register);

    printk(KERN_INFO "disable trace and reset!");
}

void pause_junor2_trace(void* debug){
    struct connect_param *param = (struct connect_param *)debug;
    etm_disable(param->etm1_register);
    funnel_disable(param->cluster0_funnel_register);
    funnel_disable(param->main_funnel_register);
    tmc_disable_etf_sink(param->etf0_register);
}

void continue_junor2_trace(void* debug){
    struct connect_param *param = (struct connect_param *)debug;
    struct etf_config etf_con = {0x100};
    struct funnel_config main_funnel_con = {0, 0xFAC688};
    struct funnel_config cluster0_funnel_con = {1, 0xFAC688}; 
    tmc_enable_etf_sink(param->etf0_register, etf_con);
    funnel_enable(param->main_funnel_register, main_funnel_con);
    funnel_enable(param->cluster0_funnel_register, cluster0_funnel_con);
    etm_continue(param->etm1_register);
}

// void dump_junor2_trace(void* debug){
//     //todo
// }

#endif


#ifdef HIKEY970
void enable_hikey970_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    /* funnel and tmc configuration */
    struct etr_config etr_con = {0x00001000, 0x10086, 0x100};
    struct funnel_config funnel1_con = {3, 0xFAC688};   // note: be cautious for configuration
    struct funnel_config funnel2_con = {1, 0xFAC688};

    tmc_enable_etr_sink(param->etr0_register, etr_con);
    tmc_enable_etf_link(param->etb2_register);
    funnel_enable(param->funnel2_register, funnel2_con);
    tmc_enable_etf_link(param->etb1_register);
    funnel_enable(param->funnel1_register, funnel1_con);
    etm_enable_trace_program_flow(param->etm7_register);
}

void disable_hikey970_trace(void* debug)
{
    struct connect_param *param = (struct connect_param *)debug;
    struct etr_config etr_con = {0x00001000, 0x10086, 0x100};

    etm_disable(param->etm7_register);
    funnel_disable(param->funnel1_register);
    tmc_disable_etf_link(param->etb1_register);
    funnel_disable(param->funnel2_register);
    tmc_disable_etf_link(param->etb2_register);
    tmc_disable_etr_sink(param->etr0_register, etr_con);

    printk(KERN_INFO "disable trace and reset!");
}
#endif
