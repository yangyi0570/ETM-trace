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
#include <linux/export.h>  // EXPORT_SYMBOL()

#include "register.h"
#include "connect.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang Yi");
MODULE_DESCRIPTION("tracing a core");

/* 寄存器信息，硬件相关 */
struct connect_param tcparam;
struct connect_param *cparam;

/* define spinlock of trace buffer character */
DEFINE_SPINLOCK(tbc_lock);

/* parameter about character device*/
int device_buffer_size = 0x10000; //0x10000 = 64KB
dev_t dev = 0;
static struct class *dev_class;
static struct cdev cs_cdev;
void __iomem *paddr;
void __iomem* write_point;

/* character device operations */
static ssize_t cs_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = cs_read,
};

static ssize_t cs_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    void* read_point = paddr;
    int bytes_read = 0;
    uint32_t val;
    if(write_point == paddr)    return 0;   // empty buffer.

    spin_lock(&tbc_lock);
    while(read_point != write_point){
        val = ioread32(read_point);
        if(copy_to_user(buf, &val, sizeof(val))){
            spin_unlock(&tbc_lock);
            return -EFAULT;
        }
        read_point += 4;
        bytes_read += 4;
    }
    write_point = paddr;
    memset(paddr, 0, device_buffer_size);
    spin_unlock(&tbc_lock);

    return bytes_read;
}

/* create /dev/cs_device file */
static int create_trace_buffer_device(void)
{
    /*Allocating Major number*/
    if ((alloc_chrdev_region(&dev, 0, 1, "cs_Dev")) < 0)
    {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
    /*Creating cdev structure*/
    cdev_init(&cs_cdev, &fops);
    /*Adding character device to the system*/
    if ((cdev_add(&cs_cdev, dev, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }
    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "cs_class")) == NULL)
    {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
    /*Creating device*/
    if ((device_create(dev_class, NULL, dev, NULL, "cs_trace_buffer")) == NULL)
    {
        pr_err("Cannot create the trace buffer device.\n");
        goto r_device;
    }
    pr_info("cs_trace_buffer created!!!\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

int cs_trace_continue(void){
    printk(KERN_INFO "cs_trace_continue is called.");
    continue_junor2_trace(cparam);
    return 0;
}
EXPORT_SYMBOL_GPL(cs_trace_continue);

int cs_trace_pause(void){
    printk(KERN_INFO "cs_trace_pause is called.");
    pause_junor2_trace(cparam);
    return 0;
}
EXPORT_SYMBOL_GPL(cs_trace_pause);

static int __init trace_init(void)
{
    printk(KERN_INFO "--------------------------------------");

    cparam = kmalloc(sizeof(tcparam), GFP_KERNEL);

    /* create a character device, kernel write to address directly, user read by device.*/
    int retval = create_trace_buffer_device();
    if (retval) return retval;
    paddr = kmalloc(device_buffer_size, GFP_KERNEL); // 64KB,kmalloc()最大可申请内存空间
    memset(paddr, 0, device_buffer_size);
    write_point = paddr;

#ifdef JUNOR2
    /* map the debug and cross trigger registers into virtual memory space */
    cparam->etm1_register = ioremap(ETM1_BASE_ADD, ETM_REGISTERS_SIZE);
    cparam->cluster0_funnel_register = ioremap(CLUSTER0_FUNNEL_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->main_funnel_register = ioremap(MAIN_FUNNEL_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->etf0_register = ioremap(ETF0_BASE_ADD, TMC_REGISTERS_SIZE);
    cparam->pmu1_register = ioremap(PMU1_BASE_ADD, PMU_REGISTERS_SIZE);

    // 尝试将trace配置在指定核上执行
    // smp_call_function_single(debugger, trace_program_flow, (void*)cparam, 1);
    enable_junor2_trace(cparam);
#endif

#ifdef HIKEY970
    /* map the debug and cross trigger registers into virtual memory space */
    cparam->etm7_register = ioremap(ETM7_BASE_ADD, ETM_REGISTERS_SIZE);
    cparam->etb1_register = ioremap(ETB1_BASE_ADD, TMC_REGISTERS_SIZE);
    cparam->etb2_register = ioremap(ETB2_BASE_ADD, TMC_REGISTERS_SIZE);
    cparam->funnel1_register = ioremap(FUNNEL1_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->funnel2_register = ioremap(FUNNEL2_BASE_ADD, FUNNEL_REGISTERS_SIZE);
    cparam->etr0_register = ioremap(ETR0_BASE_ADD, TMC_REGISTERS_SIZE);
    cparam->pmu7_register = ioremap(PMU7_BASE_ADD, PMU_REGISTERS_SIZE);

    enable_hikey970_trace(cparam);
#endif

    /* print kernel module info */
    printk(KERN_INFO "module: the trace configuration is installed\n");
    return 0;
}

static void __exit trace_exit(void)
{
#ifdef JUNOR2
    disable_junor2_trace(cparam);

    /* unmap physical memory */
    iounmap(cparam->etm1_register);
    iounmap(cparam->cluster0_funnel_register);
    iounmap(cparam->main_funnel_register);
    iounmap(cparam->etf0_register);
    iounmap(cparam->pmu1_register);
    kfree(cparam);
#endif

#ifdef HIKEY970
    disable_hikey970_trace(cparam);

    /* unmap physical memory */
    iounmap(cparam->etm7_register);
    iounmap(cparam->etb1_register);
    iounmap(cparam->etb2_register);
    iounmap(cparam->funnel1_register);
    iounmap(cparam->funnel2_register);
    iounmap(cparam->etr0_register);
    iounmap(cparam->pmu7_register);
    kfree(cparam);
#endif

    /* remove character device */
    kfree(paddr);
    /* unmount character device */
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&cs_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("cs_trace_buffer removed!!!\n");


    /* print kernel module info */
    printk(KERN_INFO "module: the trace configuration is removed\n");
    printk(KERN_INFO "--------------------------------------------");
}

module_init(trace_init);
module_exit(trace_exit);
