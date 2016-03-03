#include "common.h"
#include "memory.h"
#include <string.h>
#include <sys/user.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
inline PDE* get_updir();

static PTE video[16] align_to_page;

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
    PDE* updir = get_updir() + (VMEM_ADDR >> 20);
    int i;
    for(i = 0; i < (SCR_SIZE / PAGE_SIZE) + (SCR_SIZE % PAGE_SIZE != 0); i++)
    {
        updir->val = make_pde(va_to_pa(&video[i]));
        video[i].val = make_pte(VMEM_ADDR + i * PAGE_SIZE);
        updir++;
    }
	//panic("please implement me");
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
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

