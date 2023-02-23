#ifndef __FUNNEL_H
#define __FUNNEL_H

#include <linux/kernel.h>
#include <asm/io.h>
#include "coresight_priv.h"


// funnel
#define FUNNEL_FUNCTL		        0x000
#define FUNNEL_PRICTL		        0x004
#define FUNNEL_LOCK_ACCESS          0xFB0
#define FUNNEL_LOCK_STATUS          0xFB4
 
// Bits in funnel
#define EnS0                        1
#define EnS1                        (1 << 1)
#define EnS2                        (1 << 2)
#define EnS3                        (1 << 3)
#define EnS4                        (1 << 4)

#define FUNNEL_HOLDTIME_MASK	0xf00
#define FUNNEL_HOLDTIME_SHFT	0x8
#define FUNNEL_HOLDTIME		    (0x7 << FUNNEL_HOLDTIME_SHFT)

struct funnel_config
{
    int port;
    int priority;   //reset后，0位置设为0，1位置设为1。数字越小说明对应端口优先级越高。默认为0xFA C688
};

void funnel_enable(void *base, struct funnel_config con);
void funnel_disable(void *base);

#endif