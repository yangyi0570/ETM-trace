#include "access.h"


uint32_t read_32bit_from_register(void __iomem *base, uint32_t offset)
{
    uint32_t val = ioread32(base + offset);
    return val;
}
void write_32bit_to_register(void __iomem *base, uint32_t offset, uint32_t val)
{
    iowrite32(val, base + offset);
}