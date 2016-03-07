#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
enum { E_CF, E_t1, E_PF, E_t2, E_AF, E_t3, E_ZF, E_SF, E_TF, E_IF, E_DF, E_OF, E_IOPL1, E_IOPL2, E_NT, E_t4, E_RF, E_VM};
enum { R_ES, R_CS, R_SS, R_DS, R_FS, R_GS };


/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union {
        union {
            uint32_t _32;
            uint16_t _16;
            uint8_t _8[2];
        } gpr[8];

        /* Do NOT change the order of the GPRs' definitions. */

        struct {
            uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
        };
    };

    union {
        struct {
            uint16_t _16;
            uint32_t _32[2];
        }spr[6];
        struct {
            uint16_t es; uint32_t es_cache_base; uint32_t es_cache_limit;
            uint16_t cs; uint32_t cs_cache_base; uint32_t cs_cache_limit;
            uint16_t ss; uint32_t ss_cache_base; uint32_t ss_cache_limit;
            uint16_t ds; uint32_t ds_cache_base; uint32_t ds_cache_limit;
        };
    };

	union {
		uint32_t EFLAGS;
		struct {
			uint32_t CF:1;
			uint32_t t1:1;
			uint32_t PF:1;
			uint32_t t2:1;
			uint32_t AF:1;
			uint32_t t3:1;
			uint32_t ZF:1;
			uint32_t SF:1;
			uint32_t TF:1;
			uint32_t IF:1;
			uint32_t DF:1;
			uint32_t OF:1;
			uint32_t IOPL:2;
			uint32_t NT:1;
			uint32_t t4:1;
			uint32_t RF:1;
			uint32_t VM:1;
		};
	};

	swaddr_t eip;
    uint64_t GDTR:48;
    uint64_t IDTR:48;

    union {
        struct {
            uint32_t protect_enable      : 1;
            uint32_t monitor_coprocessor : 1;
            uint32_t emulation           : 1;
            uint32_t task_switched       : 1;
            uint32_t extension_type      : 1;
            uint32_t numeric_error       : 1;
            uint32_t CR0_pad0            : 10;
            uint32_t write_protect       : 1;
            uint32_t CR0_pad1            : 1;
            uint32_t alignment_mask      : 1;
            uint32_t CR0_pad2            : 10;
            uint32_t no_write_through    : 1;
            uint32_t cache_disable       : 1;
            uint32_t paging              : 1;
        };
        uint32_t CR0;
    };

    /* the Control Register 3 (physical address of page directory) */
    union {
        struct {
            uint32_t CR3_pad0            : 3;
            uint32_t page_write_through  : 1;
            uint32_t page_cache_disable  : 1;
            uint32_t CR3_pad1            : 7;
            uint32_t page_directory_base : 20;
        };
        uint32_t CR3;
    };
    volatile bool INTR;

} CPU_state;

extern CPU_state cpu;

uint32_t lnaddr_read(lnaddr_t addr, size_t len);
static inline void load_segment_cache(uint8_t sreg)
{
    uint32_t descriptor_low = lnaddr_read((sreg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((sreg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    cpu.spr[sreg]._32[0] = (descriptor_low >> 16)
                                   + ((descriptor_high & 0xFF) << 16)
                                   + (descriptor_high & 0xFF000000);
    cpu.spr[sreg]._32[1] = (((descriptor_low & 0xFFFF) + (descriptor_high & 0xF0000)) << 12) | 0xFFF;
}

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

static inline int check_seg_index(int index) {
    Assert(index >= 0 && index < 6, "index = %d",index);
    return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])
#define seg_w(index)   (cpu.spr[check_seg_index(index)]._16)

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* regeflags[];
extern const char* segsw[];

#endif
