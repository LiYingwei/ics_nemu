#include <cpu/reg.h>
#include <device/mmio.h>
#include "cpu/mmu.h"
#include "memory/tlb.h"
#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
uint32_t cache_read(hwaddr_t, size_t);
void cache_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	//uint32_t ret = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
    //uint32_t test= cache_read(addr, len) & (~0u >> ((4 - len) << 3));
    //printf("%02x\n", ret);
    //Assert(ret == test ,"%u, %u(addr = %08x)", ret, test, addr);
    //return ret;
    int map_NO = is_mmio(addr);
    if(map_NO != -1) {
        //Log("map_NO = %d", map_NO);
        return mmio_read(addr, len, map_NO) & (~0u >> ((4 - len) << 3));
    }
    return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
    //dram_write(addr, len, data);
    int map_NO = is_mmio(addr);
    if(map_NO != -1)
    {
        //Log("map_NO = %d", map_NO);
        mmio_write(addr, len, data, map_NO);
        return;
    }
    cache_write(addr, len, data);
}

hwaddr_t page_translate(lnaddr_t addr)
{
    //if(cpu.paging == 1 || cpu.protect_enable != 1) printf("PE = %d, PG = %d\n", cpu.protect_enable, cpu.paging);
    if(cpu.protect_enable != 1 || cpu.paging !=1) return addr;
    return tlb_translate(addr);
    /*PDE dir_entry;
    dir_entry.val = hwaddr_read(((uint32_t)cpu.page_directory_base << 12) + (addr >> 22) * 4, 4);
    Assert(dir_entry.present, "addr = %08x", addr);
    PTE page_entry;
    page_entry.val = hwaddr_read(((uint32_t)dir_entry.page_frame << 12) + ((addr >> 12) & 0x3FF) * 4, 4);
    Assert(page_entry.present, "addr = %08x", addr);
    hwaddr_t ret = ((uint32_t)page_entry.page_frame << 12) + (addr & 0xFFF);
    //Log("lnaddr_t: %08X, hwaddr_t: %08X", addr, ret);
    return ret;*/
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
    assert(len == 1 || len == 2 || len == 4);
    if ((addr & 0xFFF) + len > 0xFFF + 1) { /*data cross the page boundary*/
        /* this is a special case, you can handle it later. */
        /*hwaddr_t hwaddr1 = page_translate(addr);
        hwaddr_t hwaddr2 = page_translate((addr + (lnaddr_t)len) & 0xFFFFF000U);
        size_t len1 = hwaddr2 - hwaddr1;*/
        //printf("addr = %08x len = %d eip = %08x eax = %08x\n", addr, (int)len, cpu.eip, cpu.eax);
        uint32_t ret = 0;
        int i;
        for(i = 0; i < len; i++)
            ret = ret + (hwaddr_read(page_translate(addr + i), 1) << (i * 8));
        return ret;
    }
    else {
        hwaddr_t hwaddr = page_translate(addr);
        return hwaddr_read(hwaddr, len);
    }
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
    assert(len == 1 || len == 2 || len == 4);
    if ((addr & 0xFFF) + len > 0xFFF + 1) { /*data cross the page boundary*/
        /* this is a special case, you can handle it later. */
        int i;
        for(i = 0; i < len; i++)
            hwaddr_write(page_translate(addr + i), 1, (data >> (i * 8)) & 0xFF);
        return;
    }
    else {
        hwaddr_t hwaddr = page_translate(addr);
        hwaddr_write(hwaddr, len, data);
    }
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
    /*uint32_t descriptor_low = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    uint32_t base_addr = (descriptor_low >> 16)
                         + ((descriptor_high & 0xFF) << 16)
                         + (descriptor_high & 0xFF000000);*/
    //uint32_t limit = (descriptor_low & 0xFFFF) + (descriptor_high & 0xF0000);
    //printf("base_addr = 0x%u, addr = 0x%u, rseg = %s\n", base_addr, addr, segsw[sreg]);
    return cpu.spr[sreg]._32[0] + addr;
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

