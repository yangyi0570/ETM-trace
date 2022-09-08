#ifndef __CONNECT_FUNC_H
#define __CONNECT_FUNC_H

#include "etm.h"
#include "register.h"
#include "tmc.h"
#include "tmc-etf.h"
#include "tmc-etr.h"
#include "funnel.h"
#include "pmu.h"
#include "coresight_priv.h"

/* trace base address struct*/
#ifdef JUNOR2
struct connect_param{
    void __iomem *etm0_register;
    void __iomem *etm1_register;
    void __iomem *etm2_register;
    void __iomem *etm3_register;
    void __iomem *etm4_register;
    void __iomem *etm5_register;
    void __iomem *cluster0_funnel_register;
    void __iomem *cluster1_funnel_register;
    void __iomem *main_funnel_register;
    void __iomem *cssys1_register;
    void __iomem *cssys2_register;
    void __iomem *etf0_register;
    void __iomem *etf1_register;
    void __iomem *etr_register;
    void __iomem *pmu0_register;
    void __iomem *pmu1_register;
    void __iomem *pmu2_register;
    void __iomem *pmu3_register;
    void __iomem *pmu4_register;
    void __iomem *pmu5_register;
};

void enable_junor2_trace(void* debug);
void disable_junor2_trace(void* debug);
#endif

#ifdef HIKEY970
struct connect_param{
    void __iomem *etm0_register;
    void __iomem *etm1_register;
    void __iomem *etm2_register;
    void __iomem *etm3_register;
    void __iomem *etm4_register;
    void __iomem *etm5_register;
    void __iomem *etm6_register;
    void __iomem *etm7_register;
    void __iomem *funnel0_register;
    void __iomem *funnel1_register;
    void __iomem *funnel2_register;
    void __iomem *etb0_register;
    void __iomem *etb1_register;
    void __iomem *etb2_register;
    void __iomem *etr0_register;
    void __iomem *pmu0_register;
    void __iomem *pmu1_register;
    void __iomem *pmu2_register;
    void __iomem *pmu3_register;
    void __iomem *pmu4_register;
    void __iomem *pmu5_register;
    void __iomem *pmu6_register;
    void __iomem *pmu7_register;
};

void enable_hikey970_trace(void* debug);
void disable_hikey970_trace(void* debug);
#endif


#endif
