#include <cpu/reg.h>
#include <cpu/decode/operand.h>
#include <cpu/decode/modrm.h>
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

#define DATA_BYTE 1
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "mov-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(mov_i2r)
make_helper_v(mov_i2rm)
make_helper_v(mov_r2rm)
make_helper_v(mov_rm2r)
make_helper_v(mov_a2moffs)
make_helper_v(mov_moffs2a)
make_helper_v(movsx_rm2r)
make_helper_v(movzx_rm2r)

make_helper(mov_cr2r)
{
    op_dest->size = 4;
    int len = read_ModR_M(eip + 1, op_dest, op_src);
    if(op_src->reg == 0)
    {
        reg_l(op_dest->reg) = cpu.CR0;
        strcpy(op_src->str, "%cr0");
    }
    else
    {
        assert(op_src->reg == 3);
        reg_l(op_dest->reg) = cpu.CR3;
        strcpy(op_src->str, "%cr3");
    }
    print_asm("mov %s, %s", op_src->str, regsl[op_dest->reg]);
    return len + 1;
}

make_helper(mov_r2cr)
{
    op_src->size = 4;
    int len = read_ModR_M(eip + 1, op_src, op_dest);
    if(op_dest->reg == 0)
    {
        cpu.CR0 = op_src->val;
        strcpy(op_dest->str, "%cr0");
    }
    else // op_dest->reg == 3
    {
        assert(op_dest->reg == 3);
        cpu.CR3 = op_src->val;
        strcpy(op_dest->str, "%cr3");
        init_tlb();
    }
    print_asm("mov %s, %s", op_src->str, op_dest->str);
    return len + 1;
}

make_helper(mov_rm2sreg)
{
    int len = decode_rm2r_w(eip + 1);
    seg_w(op_dest->reg) = (uint16_t) op_src->val;
    /*uint32_t descriptor_low = lnaddr_read((op_dest->reg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((op_dest->reg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    cpu.spr[op_dest->reg]._32[0] = (descriptor_low >> 16)
                                   + ((descriptor_high & 0xFF) << 16)
                                   + (descriptor_high & 0xFF000000);
    cpu.spr[op_dest->reg]._32[1] = (((descriptor_low & 0xFFFF) + (descriptor_high & 0xF0000)) << 12) | 0xFFF;*/
    load_segment_cache((uint8_t) op_dest->reg);
    print_asm("movw %s, %%%s", op_src->str, segsw[op_dest->reg]);
    return len + 1;
}
