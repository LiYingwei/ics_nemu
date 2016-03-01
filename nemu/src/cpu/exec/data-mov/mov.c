#include <cpu/reg.h>
#include <cpu/decode/operand.h>
#include "cpu/exec/helper.h"

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
    int len = decode_rm_l(eip + 1);
    reg_l(op_src->reg) = cpu.CR0;
    print_asm("mov cr0, %s", op_src->str);
    return len + 1;
}

make_helper(mov_r2cr)
{
    int len = decode_rm_l(eip + 1);
    cpu.CR0 = reg_l(op_src->reg);
    print_asm("mov %s, cr0", op_src->str);
    return len + 1;
}

make_helper(mov_rm2sreg)
{
    int len = decode_rm2r_w(eip + 1);
    seg_w(op_dest->reg) = (uint16_t) op_src->val;
    uint32_t descriptor_low = lnaddr_read((op_dest->reg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((op_dest->reg>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    cpu.spr[op_dest->reg]._64 = ((uint64_t)descriptor_high << 32ULL) + descriptor_low;
    print_asm("movw %s, %%%s", op_src->str, segsw[op_dest->reg]);
    return len + 1;
}
