#include "pmu.h"

// void configure_pmu(void *base)
// {
    
//     // unlock pmu register
//     iowrite32(0xC5ACCE55, base + PMLAR);

//     // enable cycle counter and event counters
//     iowrite32(0x80000003, base + PMCNTENSET_EL0);

//     // disable counter overflow interrupt
//     iowrite32(0x0, base + PMINTENCLR_EL1);

//     // 配置事件
//     uint32_t pmevtyper0 = ioread32(base + PMEVTYPER0_EL0);
//     printk(KERN_INFO "pmevtyper0:0x%x\n",pmevtyper0);
//     pmevtyper0 = ((pmevtyper0 >> 16) << 16) | 0x000D;
//     //设置事件追踪的异常等级
//     pmevtyper0 |= PMEVTYPER_P;
//     pmevtyper0 |= PMEVTYPER_U;
//     pmevtyper0 |= PMEVTYPER_NSK;
//     pmevtyper0 |= PMEVTYPER_NSU;
//     pmevtyper0 &= (~PMEVTYPER_NSH);
//     pmevtyper0 &= (~PMEVTYPER_M);
//     pmevtyper0 |= PMEVTYPER_MT;         // 该位无法置位
//     pmevtyper0 &= (~PMEVTYPER_SH);
//     // printk(KERN_INFO "temp pmevtyper0:0x%x\n",pmevtyper0);
//     iowrite32(pmevtyper0, base + PMEVTYPER0_EL0);
//     pmevtyper0 = ioread32(base + PMEVTYPER0_EL0);
//     printk(KERN_INFO "pmevtyper0:0x%x\n",pmevtyper0);

//     // set x bit PMCR_EL0
//     uint32_t pmcr_el0 = ioread32(base + PMCR_EL0);
//     printk(KERN_INFO "pmcr_el0:0x%x\n",pmcr_el0);
//     pmcr_el0 = pmcr_el0 | PMCR_X;
//     pmcr_el0 = pmcr_el0 | PMCR_D;
//     pmcr_el0 = pmcr_el0 | PMCR_C;
//     pmcr_el0 = pmcr_el0 | PMCR_P;
//     pmcr_el0 = pmcr_el0 | PMCR_E;
//     iowrite32(pmcr_el0, base + PMCR_EL0);
//     pmcr_el0 = ioread32(base + PMCR_EL0);
//     printk(KERN_INFO "pmcr_el0:0x%x\n",pmcr_el0);

//     //读取事件计数
//     uint64_t pmevcntr0_el0 = ioread64(base + PMEVCNTR0_EL0);
//     printk(KERN_INFO "Event cnt:0x%lx\n", pmevcntr0_el0);
//     pmevcntr0_el0 = ioread64(base + PMEVCNTR0_EL0);
//     printk(KERN_INFO "Event cnt:0x%lx\n", pmevcntr0_el0);
//     pmevcntr0_el0 = ioread64(base + PMEVCNTR0_EL0);
//     printk(KERN_INFO "Event cnt:0x%lx\n", pmevcntr0_el0);
// }

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
