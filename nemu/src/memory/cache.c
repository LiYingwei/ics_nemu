#include <memory/cache.h>
#include "common.h"
#include "memory/cache.h"
#include "memory/memory.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


cache_type cache;

void init_cache()
{
    int i,j;
    for(i = 0; i < SET_NUM; i++)
        for(j = 0; j < WAY_NUM; j++)
            cache.set[i].block[j].valid = false;
}

static int get_block(uint32_t way_index) {
    int i;
    for (i = 0; i < WAY_NUM; i++)
        if (cache.set[way_index].block[i].valid == 0)
            return i;
    return rand() % WAY_NUM;
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = addr >> (BLOCK_WIDTH + WAY_WIDTH);
    uint32_t way_index = (addr >> BLOCK_WIDTH) & WAY_MASK;
    uint32_t offset = addr & BLOCK_MASK;
    for (i = 0; i < WAY_NUM; i++) {
        if (cache.set[way_index].block[i].valid &&
            cache.set[way_index].block[i].tag == tag) {
            hit_index[0] = i;
        }

        if (offset + len <= BLOCK_SIZE) continue;

        if (cache.set[(way_index + 1) % SET_NUM].block[i].valid &&
            cache.set[(way_index + 1) % SET_NUM].block[i].tag == tag) {
            hit_index[1] = i;
        }
    }

    if (hit_index[0] == -1) {
        hit_index[0] = get_block(way_index);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[way_index].block[hit_index[0]].data[i] =
                    dram_read(addr - offset + i, 1) & (~0u >> 27);
        cache.set[way_index].block[hit_index[0]].valid = true;
    }
    if (offset + len > BLOCK_SIZE && hit_index[1] == -1) {
        hit_index[1] = get_block((way_index + 1) % SET_NUM);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[(way_index + 1) % SET_NUM].block[hit_index[1]].data[i] =
                    dram_read(addr - offset + BLOCK_SIZE + i, 1) & (~0u >> 27);
        cache.set[way_index].block[hit_index[1]].valid = true;
    }

    uint8_t temp[2 * BLOCK_SIZE];

    memcpy(temp, cache.set[way_index].block[hit_index[0]].data, BLOCK_SIZE);

    if(offset + len > BLOCK_SIZE) {
        memcpy(temp + BLOCK_SIZE, cache.set[way_index].block[hit_index[1]].data, BLOCK_SIZE);
    }

    return unalign_rw(temp + offset, 4);
}