#include "common.h"
#include "memory/memory.h"
#include "memory/tlb.h"
#include "cpu/mmu.h"

static tlb_type tlb;

void init_tlb() {
    int i;
    for (i = 0; i < TLB_NUM; i++)
        tlb.block[i].valid = false;
}

hwaddr_t tlb_translate(lnaddr_t addr) {
    uint32_t tag = addr >> 12;
    uint32_t offset = addr & 0xFFF;
    int i;
    for (i = 0; i < TLB_NUM; i++)
        if (tlb.block[i].valid && tlb.block[i].tag == tag)
            return ((uint32_t) tlb.block[i].data << 12) + offset;

    int index = rand() % TLB_NUM;
    tlb.block[index].valid = true;
    tlb.block[index].tag = tag;

    PDE dir_entry;
    dir_entry.val = hwaddr_read(((uint32_t) cpu.page_directory_base << 12) + (addr >> 22) * 4, 4);
    Assert(dir_entry.present, "addr = %08x", addr);
    PTE page_entry;
    page_entry.val = hwaddr_read(((uint32_t) dir_entry.page_frame << 12) + ((addr >> 12) & 0x3FF) * 4, 4);
    assert(page_entry.present);
    tlb.block[index].data = page_entry.page_frame;

    return ((uint32_t) tlb.block[index].data << 12) + offset;
}