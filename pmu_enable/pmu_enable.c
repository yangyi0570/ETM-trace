#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LC");
MODULE_DESCRIPTION("Enable PMU on CPU7.");

// 0x40030000 is the base address of the debug registers on Core 7
#define DEBUG_REGISTER_ADDR_CPU7             0xed710000
#define DEBUG_REGISTER_SIZE_CPU7             0x1000

// 0x40030000 is the base address of the cross trigger interface registers on Core 7
#define CTI_REGISTER_ADDR_CPU7               0xed720000
#define CTI_REGISTER_SIZE_CPU7               0x1000

// Offsets of debug registers
#define DBGDTRRX_OFFSET                 0x80
#define EDITR_OFFSET                    0x84
#define EDSCR_OFFSET                    0x88
#define DBGDTRTX_OFFSET                 0x8C
#define EDRCR_OFFSET                    0x90
#define OSLAR_OFFSET                    0x300
#define EDLAR_OFFSET                    0xFB0

// Bits in EDSCR
#define STATUS                          (0x3f)
#define EL                              (0x300)
#define ERR                             (1 <<  6)
#define HDE				                      (1 << 14)
#define SSD                             (1 << 16)
#define ITE                             (1 << 24)
#define TXfull                          (1 << 29)
#define RXfull                          (1 << 30)

// Bits in EDRCR
#define CSE                             (1 <<  2)

// Offsets of cross trigger registers
#define CTICONTROL_OFFSET               0x0
#define CTIINTACK_OFFSET                0x10
#define CTIAPPPULSE_OFFSET              0x1C
#define CTIOUTEN0_OFFSET                0xA0
#define CTIOUTEN1_OFFSET                0xA4
#define CTITRIGOUTSTATUS_OFFSET         0x134
#define CTIGATE_OFFSET                  0x140

// Bits in CTICONTROL
#define GLBEN                           (1 <<  0)

// Bits in CTIINTACK
#define ACK0                            (1 <<  0)
#define ACK1                            (1 <<  1)

// Bits in CTIAPPPULSE
#define APPPULSE0                       (1 <<  0)
#define APPPULSE1                       (1 <<  1)

// Bits in CTIOUTEN<n>
#define OUTEN0                          (1 <<  0)
#define OUTEN1                          (1 <<  1)

// Bits in CTITRIGOUTSTATUS
#define TROUT0                          (1 <<  0)
#define TROUT1                          (1 <<  1)

// Bits in CTIGATE
#define GATE0                           (1 <<  0)
#define GATE1                           (1 <<  1)

// Values of EDSCR.STATUS
#define NON_DEBUG                       0x2
#define HLT_BY_DEBUG_REQUEST            0x13


// Bits in PMCR
#define PMCR_MASK                       0x3f
#define PMCR_E                          (1 << 0) /*  Enable all counters */
#define PMCR_P                          (1 << 1) /*  Reset all counters */
#define PMCR_C                          (1 << 2) /*  Cycle counter reset */
#define PMCR_D                          (1 << 3) /*  CCNT counts every 64th cpu cycle */
#define PMCR_X                          (1 << 4) /*  Export to ETM */
#define PMCR_DP                         (1 << 5) /*  Disable CCNT if non-invasive debug*/

// Bits in PMCNTENCLR and PMCNTENSET
#define PMCNTEN_C                       (1 << 31)
#define PMCNTEN_E0                      (1 << 0)
#define PMCNTEN_E1                      (1 << 1)
#define PMCNTEN_E2                      (1 << 2)
#define PMCNTEN_E3                      (1 << 3)
#define PMCNTEN_E4                      (1 << 4)
#define PMCNTEN_E5                      (1 << 5)

// Bits in PMUSERENR_EL0
#define PMUSERENR_ER                    (1 << 3)
#define PMUSERENR_CR                    (1 << 2)
#define PMUSERENR_SW                    (1 << 1)
#define PMUSERENR_EN                    (1 << 0)

struct nailgun_param {
    void __iomem *debug_register;
    void __iomem *cti_register;
} t_param;

uint64_t context[10];

/* This structure points to all of the device functions */

uint32_t reg;
uint64_t scr, scr2;

static void execute_ins_via_itr(void __iomem *debug, uint32_t ins) {
    uint32_t reg;
    // clear previous errors 
    iowrite32(CSE, debug + EDRCR_OFFSET);

    // Write instruction to EDITR register to execute it
    iowrite32(ins, debug + EDITR_OFFSET);

    // Wait until the execution is finished
    reg = ioread32(debug + EDSCR_OFFSET);
    while ((reg & ITE) != ITE) {
        reg = ioread32(debug + EDSCR_OFFSET);
    }

    if ((reg & ERR) == ERR) {
        printk(KERN_ERR "%s failed! instruction: 0x%08x EDSCR: 0x%08x\n", 
            __func__, ins, reg);  
    }
}

uint64_t read_64bit_from_target(void __iomem *debug, uint32_t ins){
    // read general registers of target core
    uint32_t reg;
    uint64_t val, temp;
    // transfer target value from x0 to DBGDTR, which set EDSCR.TXfull to 1
    // 0xd5130400 <=> msr dbgdtr_el0,x0
    // x1,x2,...is the same
    execute_ins_via_itr(debug, ins);

    // check EDSCR.{RXfull, TXfull} before each transfer
    // read_edscr(debug);
    reg = ioread32(debug + EDSCR_OFFSET);
    while ((reg & TXfull) != TXfull)
        reg = ioread32(debug + EDSCR_OFFSET);
        
    // after write, read value from it. First read DBGDTRRX, and second read  DBGDTRTX.
    temp = ioread32(debug + DBGDTRRX_OFFSET);  // higher 32 bits
    val = ioread32(debug + DBGDTRTX_OFFSET);  // lower 32 bits
    val = val + (temp << 32);
    return val;
}

void send_64bit_to_target(void __iomem *debug, uint32_t ins, uint64_t val){
    // write general registers of target core
    uint32_t reg, hval, lval;
    hval = (val >> 32) & 0xffffffff;  // higher 32 bits of val
    lval = val & 0xffffffff; // lower 32 bits of val
    
    // write to DBGDTR, the higher 32 bits of it is DBGDTRTX, and the lower 32 bits of it is DBGDTRRX 
    // first write DBGDTRTX, and then write DBGDTRRX, which set EDSRC.RXfull to 1
    iowrite32(hval, debug + DBGDTRTX_OFFSET);
    iowrite32(lval, debug + DBGDTRRX_OFFSET);

    // check EDSCR.{RXfull, TXfull} before each transfer
    reg = ioread32(debug + EDSCR_OFFSET);
    while ((reg & RXfull) != RXfull) 
        reg = ioread32(debug + EDSCR_OFFSET);
        
    // transfer target value from DBGDTR to x0, which set EDSCR.RXfull to 0
    // 0xd5330400 <=> mrs x0, dbgdtr_el0
    // x1,x2,...is the same
    execute_ins_via_itr(debug, ins);
}

uint64_t read_register_via_x0(void __iomem *debug, uint32_t ins){
    // ins: move the value from the system register to x0
    // store old x0
    uint64_t old_x0, data; // old x0
    old_x0 = read_64bit_from_target(debug, 0xd5130400);

    // read a system register to x0
    execute_ins_via_itr(debug, ins);
    data = read_64bit_from_target(debug, 0xd5130400);
    
    // reload old x0
    send_64bit_to_target(debug, 0xd5330400, old_x0); //reload x0
    
    return data; 
}

void write_register_via_x0(void __iomem *debug, uint32_t ins, uint64_t val){
    // ins: move the value from x0 to the system register
    // store old x0
    uint64_t old_x0; // old x0
    old_x0 = read_64bit_from_target(debug, 0xd5130400);

    // send the value to x0
    send_64bit_to_target(debug, 0xd5330400, val);
    
    // move the value from x0 to the system register
    execute_ins_via_itr(debug, ins);  

    
    // reload old x0
    send_64bit_to_target(debug, 0xd5330400, old_x0); //reload x0     
}

void save_context(void __iomem *debug, uint64_t* context){
    // 0xaa0003e0 <=> mov x0, x0; 0xd503201f <=> nop
    context[0] = read_register_via_x0(debug, 0xd503201f);
    
    // 0xd53b4520 <=> mrs x0, dlr_el0
    context[1] = read_register_via_x0(debug, 0xd53b4520);
    
    // 0xd53b4500 <=> mrs x0, dspsr_el0
    context[5] = read_register_via_x0(debug, 0xd53b4500);
    
    // 0xd5384020 <=> mrs x0, elr_el1
    // context[2] = read_register_via_x0(debug, 0xd5384020);
    // 0xd5385200 <=> mrs x0, esr_el1
    // context[3] = read_register_via_x0(debug, 0xd5385200);
    // 0xd5384000 <=> mrs x0, spsr_el1
    // context[4] = read_register_via_x0(debug, 0xd5384000);    
}

void restore_context(void __iomem *debug, uint64_t* context){
    // 0xd51b4520 <=> msr dlr_el0, x0
    write_register_via_x0(debug, 0xd51b4520, context[1]);

    // 0xd5184020 <=> msr elr_el1, x0
    // write_register_via_x0(debug, 0xd5184020, context[2]);
    // 0xd5185200 <=> msr esr_el1, x0
    // write_register_via_x0(debug, 0xd5185200, context[3]);
    // 0xd5184000 <=> msr spsr_el1, x0
    // write_register_via_x0(debug, 0xd5184000, context[4]);
    
    // 0xd51b4500 <=> msr dspsr_el0, x0
    write_register_via_x0(debug, 0xd51b4500, context[5]);
    
    // 0xaa0003e0 <=> mov x0, x0; 0xd503201f <=> nop
    write_register_via_x0(debug, 0xd503201f, context[0]);
}

void enable_pmu(void __iomem *debug){
    uint32_t reg;
    printk(KERN_INFO "enable PMU!\n");
    // 1. 0xd5033f9f <=> DSB SY
    execute_ins_via_itr(debug, 0xd5033f9f);
    // 2. 0xd5033fdf <=> ISB
    execute_ins_via_itr(debug, 0xd5033fdf);
    // 3. enable EL3 access to PMU (useless)
    //      3.1 0xd53e1320 <=> mrs x0, mdcr_el3
    // reg = read_register_via_x0(debug, 0xd53e1320);
    // printk(KERN_INFO "TEST: read MDCR_EL3: 0x%x\n", reg); 
    // reg |= (1 << 21); // set EPMAD (MDCR_EL3[21])
    // reg |= (1 << 17); // set SMPE (MDCR_EL3[17])
    //      3.2 0xd51e1320 <=> msr mdcr_el3, x0
    // write_register_via_x0(debug, 0xd51e1320, reg);
    // 4. read PMCNTENSET, set PMCNTENSET[31], PMCNTENSET[5:0] 
    //      4.1 0xd53b9c20 <=> mrs x0, pmcntenset_el0
    reg = read_register_via_x0(debug, 0xd53b9c20);
    printk(KERN_INFO "TEST: read PMCNTENSET: 0x%x\n", reg); 
    reg |= PMCNTEN_C;
    reg |= PMCNTEN_E0;
    reg |= PMCNTEN_E1;
    //      4.2 0xd51b9c20 <=> msr pmcntenset_el0, x0 
    printk(KERN_INFO "TEST: write PMCNTENSET: 0x%x\n", reg); 
    write_register_via_x0(debug, 0xd51b9c20, reg);
    // 5. read PMCR_EL0 through system register interface, set PMCR_EL0[3:0]
    //      5.1 0xd53b9c00 <=> mrs x0, pmcr_el0
    reg = read_register_via_x0(debug, 0xd53b9c00);
    printk(KERN_INFO "TEST: PMCR_EL0: 0x%x\n", reg);
    reg |= PMCR_E;
    reg |= PMCR_P;
    reg |= PMCR_C;
    reg |= PMCR_D;
    //      5.2 0xd51b9c00 <=> msr pmcr_el0, x0
    write_register_via_x0(debug, 0xd51b9c00, reg);
    reg = read_register_via_x0(debug, 0xd53b9c00);
    printk(KERN_INFO "TEST: modified PMCR_EL0: 0x%x\n", reg); 
    // 6. clear PMINTENET_EL1
    reg = 0x0;
    // 0xd5189e20 <=> msr pmintenset_el1, x0
    write_register_via_x0(debug, 0xd5189e20, reg);    
    // 7. set PMUSERENR_EL0
    //      7.1 0xd53b9e00 <=> mrs x0, pmuserenr_el0
    reg = read_register_via_x0(debug, 0xd53b9e00);
    reg |= PMUSERENR_ER;
    reg |= PMUSERENR_CR;
    reg |= PMUSERENR_EN;
    //      7.2 0xd51b9e00 <=> msr pmuserenr_el0, x0
    write_register_via_x0(debug, 0xd51b9e00, reg);
    // 8. 0xd5033f9f <=> DSB SY
    execute_ins_via_itr(debug, 0xd5033f9f);
    // 9. 0xd5033fdf <=> ISB
    execute_ins_via_itr(debug, 0xd5033fdf);
}

void disable_pmu(void __iomem *debug){
    uint32_t reg;
    printk(KERN_INFO "disable PMU!\n");
    // 1. 0xd5033f9f <=> DSB SY
    execute_ins_via_itr(debug, 0xd5033f9f);
    // 2. 0xd5033fdf <=> ISB
    execute_ins_via_itr(debug, 0xd5033fdf);
    // 3. read PMCR_EL0 through system register interface, clear PMCR_EL0[3:0]
    reg = read_register_via_x0(debug, 0xd53b9c00);
    reg &= ~PMCR_E;
    reg &= ~PMCR_P;
    reg &= ~PMCR_C;
    reg &= ~PMCR_D;
    write_register_via_x0(debug, 0xd51b9c00, reg);
    // 4. read PMCNTENSET, clear PMCNTENSET[31]
    reg = read_register_via_x0(debug, 0xd53b9c20); 
    reg &= ~PMCNTEN_C;
    write_register_via_x0(debug, 0xd51b9c20, reg);
    // 5 0xd51b9e00 <=> msr pmuserenr_el0, x0
    reg = 0x0;
    write_register_via_x0(debug, 0xd51b9e00, reg);
    // 6. 0xd5033f9f <=> DSB SY
    execute_ins_via_itr(debug, 0xd5033f9f);
    // 7. 0xd5033fdf <=> ISB
    execute_ins_via_itr(debug, 0xd5033fdf);
}


static void excute(void *addr) {

    struct nailgun_param *param = (struct nailgun_param *)addr;

    // Step 1: Unlock debug and cross trigger reigsters
    iowrite32(0xc5acce55, param->debug_register + EDLAR_OFFSET);
    iowrite32(0x0, param->debug_register + OSLAR_OFFSET);
    iowrite32(0xc5acce55, param->cti_register + EDLAR_OFFSET);
    iowrite32(0x0, param->cti_register + OSLAR_OFFSET);

    // Step 2: Enable halting debug on the target processor
    reg = ioread32(param->debug_register + EDSCR_OFFSET);
    reg |= HDE;
    iowrite32(reg, param->debug_register + EDSCR_OFFSET);

    // Step 3: Send halt request to the target processor
    iowrite32(GLBEN, param->cti_register + CTICONTROL_OFFSET);
    reg = ioread32(param->cti_register + CTIGATE_OFFSET);
    reg &= ~GATE0;
    iowrite32(reg, param->cti_register + CTIGATE_OFFSET);
    reg = ioread32(param->cti_register + CTIOUTEN0_OFFSET);
    reg |= OUTEN0;
    iowrite32(reg, param->cti_register + CTIOUTEN0_OFFSET);
    reg = ioread32(param->cti_register + CTIAPPPULSE_OFFSET);
    reg |= APPPULSE0;
    iowrite32(reg, param->cti_register + CTIAPPPULSE_OFFSET);

    // Step 4: Wait the target processor to halt
    printk(KERN_INFO "MODULE: switch to EL1\n");
    reg = ioread32(param->debug_register + EDSCR_OFFSET);
    while ((reg & STATUS) != HLT_BY_DEBUG_REQUEST) {
        reg = ioread32(param->debug_register + EDSCR_OFFSET);
    }
    reg = ioread32(param->cti_register + CTIINTACK_OFFSET);
    reg |= ACK0;
    iowrite32(reg, param->cti_register + CTIINTACK_OFFSET);
    reg = ioread32(param->cti_register + CTITRIGOUTSTATUS_OFFSET);
    while ((reg & TROUT0) == TROUT0) {
        reg = ioread32(param->cti_register + CTITRIGOUTSTATUS_OFFSET);
    }

    // Step 5: Save context of the target core
    save_context(param->debug_register, context);
     
    //Step 6: Switch to EL3 to access secure resource
    // 0xd4a00003 <=> dcps3
    execute_ins_via_itr(param->debug_register, 0xd4a00003);
        
    // Step 7: enable PMU
    reg = read_register_via_x0(param->debug_register, 0xd53b9e00);
    printk(KERN_INFO "test: read pmuserenr_el0: 0x%x\n", reg);
    printk(KERN_INFO "MODULE: begin to enable PMU\n");
    enable_pmu(param->debug_register);

    // Step 8: Restore context
    // 0xd51b4520 <=> msr dlr_el0, x0
    context[1] = (context[1] | 0xfff) - 0xfff + 0x790;  // get out of dead loop
    restore_context(param->debug_register, context);

    // Step 9: Send restart request to the target processor
    reg = ioread32(param->cti_register + CTIGATE_OFFSET);
    reg &= ~GATE1;
    iowrite32(reg, param->cti_register + CTIGATE_OFFSET);
    reg = ioread32(param->cti_register + CTIOUTEN1_OFFSET);
    reg |= OUTEN1;
    iowrite32(reg, param->cti_register + CTIOUTEN1_OFFSET);
    reg = ioread32(param->cti_register + CTIAPPPULSE_OFFSET);
    reg |= APPPULSE1;
    iowrite32(reg, param->cti_register + CTIAPPPULSE_OFFSET);

    // Step 10: Wait the target processor to restart
    reg = ioread32(param->debug_register + EDSCR_OFFSET);
     while ((reg & STATUS) != NON_DEBUG) {
        reg = ioread32(param->debug_register + EDSCR_OFFSET);
    }
    reg = ioread32(param->cti_register + CTIINTACK_OFFSET);
    reg |= ACK1;
    iowrite32(reg, param->cti_register + CTIINTACK_OFFSET);
    reg = ioread32(param->cti_register + CTITRIGOUTSTATUS_OFFSET);
    while ((reg & TROUT1) == TROUT1) {
        reg = ioread32(param->cti_register + CTITRIGOUTSTATUS_OFFSET);
    }

    printk(KERN_INFO "MODULE: switch back to EL0\n");
}

static int __init pmu_enable_init(void) {
    /* Fill buffer with our message */
    /* Set the msg_ptr to the buffer */
    struct nailgun_param *param = kmalloc(sizeof(t_param), GFP_KERNEL);
    printk(KERN_INFO "module init!\n");
    // Mapping the debug and cross trigger registers into virtual memory space 
    param->debug_register = ioremap(DEBUG_REGISTER_ADDR_CPU7, DEBUG_REGISTER_SIZE_CPU7);
    param->cti_register = ioremap(CTI_REGISTER_ADDR_CPU7, CTI_REGISTER_SIZE_CPU7);
    // We use the Core 1 to read the SCR via debugging Core 0
    smp_call_function_single(6, excute, param, 1);
    iounmap(param->cti_register);
    iounmap(param->debug_register);
    kfree(param);
    return 0;
}

static void __exit pmu_enable_exit(void) {
    printk(KERN_INFO "program end!\n");
}
module_init(pmu_enable_init);
module_exit(pmu_enable_exit);

