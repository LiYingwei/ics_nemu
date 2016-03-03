//
// Created by lyw on 16-3-3.
//

#ifndef NEMU_TLB_H
#define NEMU_TLB_H

#include "nemu.h"

#define TLB_NUM 64

typedef struct {
    bool valid;
    uint32_t tag :20;
    uint32_t data:20;
}tlb_block;

typedef struct {
    tlb_block block[TLB_NUM];
}tlb_type;

void init_tlb();
hwaddr_t tlb_translate(lnaddr_t addr);

#endif //NEMU_TLB_H
