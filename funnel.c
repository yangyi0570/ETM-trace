#include "funnel.h"

/* 
    0x0: unlocked 
    0x3: locked
*/
uint32_t funnel_lock_status(void *base)
{
    return ioread32(base + FUNNEL_LOCK_STATUS);
}


void funnel_enable(void *base, struct funnel_config con)
{
    uint32_t functl;
    CS_UNLOCK(base);

    functl = ioread32(base + FUNNEL_FUNCTL);
	functl &= ~FUNNEL_HOLDTIME_MASK;
	functl |= FUNNEL_HOLDTIME;  // 把holdtime写死为0x7
	functl |= (1 << con.port);
    printk(KERN_INFO "functl:%x",functl);
    functl &= (0xFFFFFF00 + (0xFF & (1 << con.port)));
    printk(KERN_INFO "functl:%x",functl);
    
	iowrite32(functl, base + FUNNEL_FUNCTL);
	iowrite32(con.priority, base + FUNNEL_PRICTL);

	CS_LOCK(base);
}



void funnel_disable(void *base)
{
    iowrite32(0x300, base + FUNNEL_FUNCTL);
    iowrite32(0xFAC688, base + FUNNEL_PRICTL);
}