#ifndef __TMC_ETF_H
#define __TMC_ETF_H

#include "tmc.h"
#include <linux/uaccess.h>
#include <linux/fs.h>

struct etf_config
{
    int trigger_cntr;
};

void tmc_enable_etf_sink(void *base, struct etf_config con);
void tmc_disable_etf_sink(void *base);
void tmc_enable_etf_link(void *base);
void tmc_disable_etf_link(void *base);
void tmc_dump_etf_sink(void *base);
void tmc_info_etf(void* base);
#endif