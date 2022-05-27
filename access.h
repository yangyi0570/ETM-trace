#ifndef __ACCESS_H
#define __ACCESS_H

#include <linux/kernel.h>
#include <asm/io.h>

uint32_t read_32bit_from_register(void __iomem *base, uint32_t offset);
void write_32bit_to_register(void __iomem *base, uint32_t offset, uint32_t val);


#endif