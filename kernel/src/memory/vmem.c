#include "common.h"
#include "memory.h"
#include <string.h>
#include <sys/user.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
inline PDE* get_updir();

static PTE video[NR_PTE] align_to_page;

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
    PDE* updir = get_updir() + (VMEM_ADDR >> 20);
    int i;
    updir->val = make_pde(va_to_pa(&video[0]));
    for(i = VMEM_ADDR / PAGE_SIZE; i < VMEM_ADDR / PAGE_SIZE + (SCR_SIZE / PAGE_SIZE) + (SCR_SIZE % PAGE_SIZE != 0); i++)
    {
        video[i].val = make_pte(i * PAGE_SIZE);
    }
	//panic("please implement me");
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
        //Log("%d", i);
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}