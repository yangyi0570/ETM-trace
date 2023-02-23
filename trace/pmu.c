#include "pmu.h"

void set_pmu_export(void *base)
{
    uint32_t pmcr_el0;
    // unlock pmu register
    iowrite32(0xC5ACCE55, base + PMLAR);

    pmcr_el0 = ioread32(base + PMCR_EL0);
    iowrite32(pmcr_el0 | PMCR_X, base + PMCR_EL0);  //设置PMU输出事件

    pmcr_el0 = ioread32(base + PMCR_EL0);
    printk(KERN_INFO "pmu:pmcr_el0:0x%x",pmcr_el0);

    iowrite32(0x0, base + PMLAR);
}
