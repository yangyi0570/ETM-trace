#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/smp.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang Yi");
MODULE_DESCRIPTION("junor2 pmi to read trace data temporarily.");

#define PMI_A72_core0 36
#define PMI_A72_core1 37
#define PMI_A53_core0 38
#define PMI_A53_core1 39
#define PMI_A53_core2 40
#define PMI_A53_core3 41

static const unsigned int pmu_irqs[]
    = { PMI_A72_core0,
          PMI_A72_core1,
          PMI_A53_core0,
          PMI_A53_core1,
          PMI_A53_core2,
          PMI_A53_core3 };

extern int cs_trace_pause(void);
extern int cs_trace_continue(void);


irqreturn_t pmu_irq_handler(int irq, void* dev_id)
{
    // FIRST, disable ETM & ETB
    // SECOND, retrieve trace

    // THIRD, enable ETB & ETM
    cs_trace_pause();
    cs_trace_continue();
    
    /* Set cycle counter to a specific value. */
	asm volatile("msr pmccntr_el0, %0" : : "r" (0xFFFFFFFFF0000000));

    printk("[PMU]: PMU INTERRUPT %u CAPTURED\n", irq);

    return IRQ_HANDLED;
}

void init_pmccntr(void* data){
    asm volatile("msr pmccntr_el0, %0" : : "r" (0xFFFFFFFFF0000000));
}

int register_inst_count_handler(void)
{
    int error;
    int irq_i;

    // enable PMU interrupt
    //asm volatile("msr PMINTENSET_EL1, %0" ::"r"((__u32)1 << 31));

    // for (irq_i = 0; irq_i < 6; irq_i++) {
    //     //error = request_irq(pmu_irqs[irq_i], pmu_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT | IRQF_SHARED, "pmi_handler", (void*)pmu_irq_handler);
    //     error = request_irq(pmu_irqs[irq_i], pmu_irq_handler, 0x10c84, "pmi_handler", (void*)pmu_irq_handler);
    //     if (error) {
    //         printk(KERN_ERR "[PMU]: Request_irq for interrupt %d failed %d\n", pmu_irqs[irq_i], error);
    //         return -1;
    //     } else {
    //         printk("[PMU]: successfully request_irq for interrupt %d\n", pmu_irqs[irq_i]);
    //     }
    // }

    error = request_irq(pmu_irqs[1], pmu_irq_handler, 0x10c84, "pmi_handler", (void*)pmu_irq_handler);
    if(error){
        printk(KERN_ERR "[PMU]: request irq failed.");
    }

    return 0;
}

static int __init
init(void)
{
    printk(KERN_INFO "[pmi_handler] initialized\n");
    on_each_cpu(init_pmccntr, NULL, 1);
    register_inst_count_handler();
    printk("[PMU]: Interrupt Handler Registed\n");
    return 0;
}

static void __exit
fini(void)
{
    printk(KERN_INFO "[pmi_handler] unloaded");
}

module_init(init);
module_exit(fini);
