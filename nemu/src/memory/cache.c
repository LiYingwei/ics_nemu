#include "common.h"
#include "memory/cache.h"
#include "memory/cache2.h"
#include "memory/memory.h"

cache_type cache;
//uint32_t dram_read(hwaddr_t, size_t);
//void dram_write(hwaddr_t, size_t, uint32_t);

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

int cache_check_hit(uint32_t index, uint32_t tag)
{
    int i;
    for(i = 0; i < WAY_NUM; i++) {
        if (cache.set[index].block[i].valid &&
            cache.set[index].block[i].tag == tag)
            return i;
    }
    return -1;
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = (addr >> (BLOCK_WIDTH + SET_WIDTH)) & TAG_MASK;
    uint32_t index = (addr >> BLOCK_WIDTH) & SET_MASK;
    uint32_t offset = addr & BLOCK_MASK;
    //Log("addr = %X, tag = %d index = %d offset = %X", addr, tag, index, offset);

    hit_index[0] = cache_check_hit(index, tag);
    if(offset + len > BLOCK_SIZE)
        hit_index[1] = cache_check_hit((index + 1) % SET_NUM, tag);

    if (hit_index[0] == -1) {
        hit_index[0] = get_block(index);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[index].block[hit_index[0]].data[i] =
                    cache2_read(addr - offset + i, 1) & 0xFF;
        cache.set[index].block[hit_index[0]].valid = true;
        cache.set[index].block[hit_index[0]].tag = tag;
    }
    //else Log("blockid = %d", hit_index[0]);
    if (offset + len > BLOCK_SIZE && hit_index[1] == -1) {
        hit_index[1] = get_block((index + 1) % SET_NUM);
        for (i = 0; i < BLOCK_SIZE; i++)
            cache.set[(index + 1) % SET_NUM].block[hit_index[1]].data[i] =
                    cache2_read(addr - offset + BLOCK_SIZE + i, 1) & 0xFF;
        cache.set[(index + 1) % SET_NUM].block[hit_index[1]].valid = true;
        cache.set[(index + 1) % SET_NUM].block[hit_index[1]].tag = tag;
    }

    uint8_t temp[2 * BLOCK_SIZE];

    memcpy(temp, cache.set[index].block[hit_index[0]].data, BLOCK_SIZE);
    if (offset + len > BLOCK_SIZE) {
        memcpy(temp + BLOCK_SIZE, cache.set[(index + 1) % SET_NUM].block[hit_index[1]].data, BLOCK_SIZE);
    }
    return unalign_rw(temp + offset, 4);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = (addr >> (BLOCK_WIDTH + SET_WIDTH)) & TAG_MASK;
    uint32_t index = (addr >> BLOCK_WIDTH) & SET_MASK;
    uint32_t offset = addr & BLOCK_MASK;

    hit_index[0] = cache_check_hit(index, tag);
    if (offset + len > BLOCK_SIZE)
        hit_index[1] = cache_check_hit((index + 1) % SET_NUM, tag);

    if(hit_index[0] != -1) {
        for (i = 0; i < len; i++)
            if (offset + i < BLOCK_SIZE)
                cache.set[index].block[hit_index[0]].data[offset + i]
                        = (data >> (i * 8)) & 0xFF;
    }

    if(offset + len > BLOCK_SIZE && hit_index[1] != -1) {
        for (i = 0; i < len; i++)
            if (i + offset >= BLOCK_SIZE)
                cache.set[(index + 1) % SET_NUM].block[hit_index[1]].data[i + offset - BLOCK_SIZE]
                        = (data >> (i * 8)) & 0xFF;
    }

    //Log("addr = %X, tag = %d index = %d offset = %X", addr, tag, index, offset);
    //Log("blockid = %d", hit_index[0]);
    cache2_write(addr, len, data);
}
