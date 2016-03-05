#include <memory/cache2.h>
#include "common.h"
#include "memory/cache2.h"
#include "memory/memory.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


cache2_type cache2;

void init_cache2()
{
    int i,j;
    for(i = 0; i < SET2_NUM; i++)
        for(j = 0; j < WAY2_NUM; j++)
            cache2.set[i].block[j].valid = false;
}

static void write_back(uint32_t index, uint32_t blockid)
{
    int offset;
    for(offset = 0; offset < BLOCK2_SIZE; offset += 4) {
        uint32_t tag = cache2.set[index].block[blockid].tag;
        uint32_t addr = (tag << (BLOCK2_WIDTH + SET2_WIDTH)) +
                        (index << BLOCK2_WIDTH) +
                        offset;
        uint32_t data = ((uint32_t)cache2.set[index].block[blockid].data[offset + 0] << 0U)
                      + ((uint32_t)cache2.set[index].block[blockid].data[offset + 1] << 8U)
                      + ((uint32_t)cache2.set[index].block[blockid].data[offset + 2] << 16U)
                      + ((uint32_t)cache2.set[index].block[blockid].data[offset + 3] << 24U);
        dram_write(addr, 4, data);
    }
    cache2.set[index].block[blockid].dirty = false;
}

static int get_block(uint32_t index) {
    int i;
    for (i = 0; i < WAY2_NUM; i++)
        if (cache2.set[index].block[i].valid == 0)
            return i;
    int ret = rand() % WAY2_NUM;
    if(cache2.set[index].block[ret].dirty)
    {
        write_back(index, ret);
    }
    return ret;
}

int cache2_check_hit(uint32_t index, uint32_t tag)
{
    int i;
    for(i = 0; i < WAY2_NUM; i++) {
        if (cache2.set[index].block[i].valid &&
            cache2.set[index].block[i].tag == tag)
            return i;
    }
    return -1;
}

uint32_t cache2_read(hwaddr_t addr, size_t len) {
    //Log("addr = %X\n", addr);
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = (addr >> (BLOCK2_WIDTH + SET2_WIDTH)) & TAG2_MASK;
    uint32_t index = (addr >> BLOCK2_WIDTH) & SET2_MASK;
    uint32_t offset = addr & BLOCK2_MASK;

    hit_index[0] = cache2_check_hit(index, tag);
    if(offset + len > BLOCK2_SIZE)
        hit_index[1] = cache2_check_hit((index + 1) % SET2_NUM, tag);

    if (hit_index[0] == -1) {
        hit_index[0] = get_block(index);
        for (i = 0; i < BLOCK2_SIZE; i++)
            cache2.set[index].block[hit_index[0]].data[i] =
                    dram_read(addr - offset + i, 1) & 0xFF;
        cache2.set[index].block[hit_index[0]].valid = true;
        cache2.set[index].block[hit_index[0]].tag = tag;
        cache2.set[index].block[hit_index[0]].dirty = false;
    }
    if (offset + len > BLOCK2_SIZE && hit_index[1] == -1) {
        hit_index[1] = get_block((index + 1) % SET2_NUM);
        for (i = 0; i < BLOCK2_SIZE; i++)
            cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].data[i] =
                    dram_read(addr - offset + BLOCK2_SIZE + i, 1) & 0xFF;
        cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].valid = true;
        cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].tag = tag;
        cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].dirty = false;
    }

    uint8_t temp[2 * BLOCK2_SIZE];

    memcpy(temp, cache2.set[index].block[hit_index[0]].data, BLOCK2_SIZE);
    if (offset + len > BLOCK2_SIZE) {
        memcpy(temp + BLOCK2_SIZE, cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].data, BLOCK2_SIZE);
    }
    return unalign_rw(temp + offset, 4);
}

void cache2_write(hwaddr_t addr, size_t len, uint32_t data) {
    int hit_index[2] = {-1, -1};
    int i;
    uint32_t tag = (addr >> (BLOCK2_WIDTH + SET2_WIDTH)) & TAG2_MASK;
    uint32_t index = (addr >> BLOCK2_WIDTH) & SET2_MASK;
    uint32_t offset = addr & BLOCK2_MASK;

    hit_index[0] = cache2_check_hit(index, tag);
    if (offset + len > BLOCK2_SIZE)
        hit_index[1] = cache2_check_hit((index + 1) % SET2_NUM, tag);

    if(hit_index[0] == -1) {
        hit_index[0] = get_block(index);
        for (i = 0; i < BLOCK2_SIZE; i++)
            cache2.set[index].block[hit_index[0]].data[i] =
                    dram_read(addr - offset + i, 1) & 0xFF;
        cache2.set[index].block[hit_index[0]].valid = true;
        cache2.set[index].block[hit_index[0]].tag = tag;
        cache2.set[index].block[hit_index[0]].dirty = false;
    }

    if (offset + len > BLOCK2_SIZE && hit_index[1] == -1)
    {
        hit_index[1] = get_block((index + 1) % SET2_NUM);
        for (i = 0; i < BLOCK2_SIZE; i++)
            cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]].data[i] =
                    dram_read(addr - offset + BLOCK2_SIZE + i, 1) & 0xFF;
        cache2.set[(index + 1) % SET_NUM].block[hit_index[1]].valid = true;
        cache2.set[(index + 1) % SET_NUM].block[hit_index[1]].tag = tag;
        cache2.set[(index + 1) % SET_NUM].block[hit_index[1]].dirty = false;
    }

    for (i = 0; i < len; i++) {
        if (offset + i < BLOCK2_SIZE)
            cache2.set[index].block[hit_index[0]].data[offset + i]
                    = (data >> (i * 8)) & 0xFF;
        cache2.set[index].block[hit_index[0]].dirty = true;
    }

    if(offset + len > BLOCK2_SIZE) {
        for (i = 0; i < len; i++)
            if (i + offset >= BLOCK2_SIZE)
                cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]]
                        .data[i + offset - BLOCK2_SIZE]
                        = (data >> (i * 8)) & 0xFF;
        cache2.set[(index + 1) % SET2_NUM].block[hit_index[1]]
                .dirty = true;
    }
}
