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

struct connect_param{                    // trace相关部件基址
    void __iomem *etm1_register;
    void __iomem *cluster0_funnel_register;
    void __iomem *main_funnel_register;
    void __iomem *etf0_register;     // todo：在trace加入ioremap
    void __iomem *pmu1_register;
};

void enable_trace(void* debug);
void disable_trace(void* debug);
void trace_pmu_event(void* debug);

#endif
