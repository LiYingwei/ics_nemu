//
// Created by lyw on 16-2-18.
//

#ifndef NEMU_CACHE2_H
#define NEMU_CACHE2_H

#include "nemu.h"

#define BLOCK2_WIDTH 6
#define CACHE2_WIDTH 22
#define WAY2_WIDTH 4
#define ADDR2_WIDTH 32
#define SET2_WIDTH (CACHE2_WIDTH - WAY2_WIDTH - BLOCK2_WIDTH)  //12
#define TAG2_WIDTH (ADDR2_WIDTH - SET2_WIDTH - BLOCK2_WIDTH)   //14

#define BLOCK2_SIZE (1 << BLOCK2_WIDTH) //64
#define CACHE2_SIZE (1 << CACHE2_WIDTH) //4M
#define WAY2_NUM (1 << WAY2_WIDTH) //16
#define SET2_NUM (CACHE2_SIZE / BLOCK2_SIZE / WAY2_NUM) //16K

//#define WAY2_MASK (WAY2_NUM - 1)
#define BLOCK2_MASK (BLOCK2_SIZE - 1)
#define SET2_MASK (SET2_NUM - 1)
#define TAG2_MASK ((1 << TAG2_WIDTH) - 1)

typedef struct {
    bool valid;
    bool dirty;
    uint32_t tag : (ADDR2_WIDTH - (CACHE2_WIDTH - BLOCK2_WIDTH - WAY2_WIDTH));
    uint8_t data[BLOCK2_SIZE];
}cache2_block;

typedef struct {
    cache2_block block[WAY2_NUM];
}cache2_set;

typedef struct {
    cache2_set set[SET2_NUM];
}cache2_type;

void init_cache2();
int cache2_check_hit(uint32_t, uint32_t);
uint32_t cache2_read(hwaddr_t, size_t);
void cache2_write(hwaddr_t, size_t, uint32_t);


#endif //NEMU_CACHE2_H
