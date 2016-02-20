#include <cpu/reg.h>
#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
uint32_t cache_read(hwaddr_t, size_t);
void cache_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	/*uint32_t ret = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
     *uint32_t test= cache_read(addr, len) & (~0u >> ((4 - len) << 3));
     *printf("%02x\n", ret);
     *Assert(ret == test ,"%u, %u", ret, test);
     *return ret;*/
    return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
    cache_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

static lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg)
{
    if(cpu.protect_enable == 0) return addr;
    /*else {
        printf("PE = %d\n", cpu.protect_enable);
        printf("CR0 = %d\n", cpu.CR0);
    }*/
    uint16_t SR = seg_w(sreg);
    Assert((SR >> 3) * 8 < (cpu.GDTR & 0xFFFF), "段选择符索引越界");
    uint32_t descriptor_low = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    uint32_t base_addr = (descriptor_low >> 16)
                         + ((descriptor_high & 0xFF) << 16)
                         + (descriptor_high & 0xFF000000);
    //uint32_t limit = (descriptor_low & 0xFFFF) + (descriptor_high & 0xF0000);
    //printf("base_addr = 0x%u, addr = 0x%u, rseg = %s\n", base_addr, addr, segsw[sreg]);
    return base_addr + addr;
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

