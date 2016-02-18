//
// Created by lyw on 16-2-18.
//

#ifndef NEMU_CACHE_H
#define NEMU_CACHE_H

#include "nemu.h"

#define BLOCK_WIDTH 6 //(B)
#define CACHE_WIDTH 16
#define WAY_WIDTH 3
#define ADDR_WIDTH 32
#define SET_WIDTH (CACHE_WIDTH - WAY_WIDTH - BLOCK_WIDTH)

#define BLOCK_SIZE (1 << BLOCK_WIDTH)
#define CACHE_SIZE (1 << CACHE_WIDTH)
#define WAY_NUM (1 << WAY_WIDTH)
#define SET_NUM (CACHE_SIZE / BLOCK_SIZE / WAY_NUM)

#define WAY_MASK (WAY_NUM - 1)
#define BLOCK_MASK (BLOCK_SIZE - 1)

typedef struct {
    bool valid;
    uint32_t tag : (ADDR_WIDTH - (CACHE_WIDTH - BLOCK_WIDTH - WAY_WIDTH));
    uint8_t data[BLOCK_SIZE];
}cache_block;

typedef struct {
    cache_block block[WAY_NUM];
}cache_set;

typedef struct {
    cache_set set[SET_NUM];
}cache_type;

void init_cache();
#endif //NEMU_CACHE_H