#ifndef __ETM_ETR_H
#define __ETM_ETR_H

#include "tmc.h"
#include <linux/uaccess.h>
#include <linux/fs.h>

struct etr_config
{
    int size;       // trace buffer的大小
    uint64_t paddr;  // base address of trace buffer in system memory
    int trigger_cntr;   //在trigger后存储多少字的数据
};

void tmc_enable_etr_sink(void *base, struct etr_config con);
void tmc_disable_etr_sink(void *base, struct etr_config con);
void tmc_dump_etr_sink(void *base, struct etr_config con);

#endif