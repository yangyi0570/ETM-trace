#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/init.h>
#include <linux/slab.h> // kmalloc()
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/uaccess.h> // copy_to_user()

#include "register.h"
#include "connect.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang Yi");
MODULE_DESCRIPTION("tracing a core");

/* 寄存器信息，硬件相关 */
struct connect_param tcparam;
struct connect_param *cparam;

/* 内核模块参数，记录调试核 */
// int debugger = 6;

static int __init trace_init(void)
{
    printk(KERN_INFO "--------------------------------------");
    /* map the debug and cross trigger registers into virtual memory space */
    cparam = kmalloc(sizeof(tcparam), GFP_KERNEL);
    cparam->etm1_register = ioremap(ETM1_BASE_ADD, ETM_REGISTERS_SIZE);
    cparam->cluster0_funnel_register = ioremap(CLUSTER0_FUNNEL_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->main_funnel_register = ioremap(MAIN_FUNNEL_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->etf0_register = ioremap(ETF0_BASE_ADD, TMC_REGISTERS_SIZE);
    cparam->pmu1_register = ioremap(PMU1_BASE_ADD, PMU_REGISTERS_SIZE);

    // 尝试将trace配置在指定核上执行
    // smp_call_function_single(debugger, trace_program_flow, (void*)cparam, 1);
    enable_trace(cparam);

    /* print kernel module info */
    printk(KERN_INFO "module: the trace configuration is installed\n");
    return 0;
}

static void __exit trace_exit(void)
{
    disable_trace(cparam);

    /* unmap physical memory */
    iounmap(cparam->etm1_register);
    iounmap(cparam->cluster0_funnel_register);
    iounmap(cparam->main_funnel_register);
    iounmap(cparam->etf0_register);
    iounmap(cparam->pmu1_register);
    kfree(cparam);

    /* print kernel module info */
    printk(KERN_INFO "module: the trace configuration is removed\n");
    printk(KERN_INFO "--------------------------------------------");
}

module_init(trace_init);
module_exit(trace_exit);
