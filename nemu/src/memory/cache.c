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

static int get_block(uint32_t index) {
    int i;
    for (i = 0; i < WAY_NUM; i++)
        if (cache.set[index].block[i].valid == 0)
            return i;
    return rand() % WAY_NUM;
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = (addr >> (BLOCK_WIDTH + SET_WIDTH)) & TAG_MASK;
    uint32_t index = (addr >> BLOCK_WIDTH) & SET_MASK;
    uint32_t offset = addr & BLOCK_MASK;
    //Log("cache read %x, len = %u", addr, (unsigned)len);
    //Log("tag: %u\tindex: %u\toffset: %x", tag, index, offset);
    for (i = 0; i < WAY_NUM; i++) {
        if (cache.set[index].block[i].valid &&
            cache.set[index].block[i].tag == tag) {
            hit_index[0] = i;
            //Log("hit, blockid = %u", hit_index[0]);
            break;
        }

        /*if (offset + len <= BLOCK_SIZE) continue;

        if (cache.set[(index + 1) % SET_NUM].block[i].valid &&
            cache.set[(index + 1) % SET_NUM].block[i].tag == tag) {
            hit_index[1] = i;
        }*/
    }

    if (1 || hit_index[0] == -1) {
        hit_index[0] = get_block(index);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[index].block[hit_index[0]].data[i] =
                    dram_read(addr - offset + i, 1) & 0xFF;
        cache.set[index].block[hit_index[0]].valid = true;
        cache.set[index].block[hit_index[0]].tag = tag;
        //Log("miss, blockid = %u==============================", hit_index[0]);
    }
    if (offset + len >= BLOCK_SIZE && (1 || hit_index[1] == -1)) {
        hit_index[1] = get_block((index + 1) % SET_NUM);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[(index + 1) % SET_NUM].block[hit_index[1]].data[i] =
                    dram_read(addr - offset + BLOCK_SIZE + i, 1) & 0xFF;
        cache.set[index].block[hit_index[1]].valid = true;
        cache.set[index].block[hit_index[1]].tag = tag;
    }

    uint8_t temp[2 * BLOCK_SIZE];

    //Log("index = %u, blockid = %u", index, hit_index[0]);
    memcpy(temp, cache.set[index].block[hit_index[0]].data, BLOCK_SIZE);
    /*for(i = 0; i < BLOCK_SIZE; i++)printf("%02x", cache.set[index].block[hit_index[0]].data[i]);
    printf("\n");*/
    if(offset + len >= BLOCK_SIZE) {
        memcpy(temp + BLOCK_SIZE, cache.set[index].block[hit_index[1]].data, BLOCK_SIZE);
    }
    return unalign_rw(temp + offset, 4);
}