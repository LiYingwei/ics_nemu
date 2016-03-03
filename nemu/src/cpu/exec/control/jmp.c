//
// Created by lyw on 15-10-21.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jmp-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(jmp_si);
make_helper_v(jmp_rm);

make_helper(ljmp)
{
    int len = 1;
    len += decode_i_l(eip + 1);
    cpu.eip = op_src->val;
    len += decode_i_w(eip + len);
    cpu.cs = op_src->val;
    uint16_t SR = R_CS;
    /*uint32_t descriptor_low = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16), 4);
    uint32_t descriptor_high = lnaddr_read((SR>>3) * 8 + (uint32_t)(cpu.GDTR >> 16) + 4, 4);
    cpu.cs_cache_base = (descriptor_low >> 16)
                        + ((descriptor_high & 0xFF) << 16)
                        + (descriptor_high & 0xFF000000);
    cpu.cs_cache_limit = (((descriptor_low & 0xFFFF) + (descriptor_high & 0xF0000))<<12) | 0xFFF;*/
    load_segment_cache(SR);
    DONT_TOUCH_MY_EIP = true;
    print_asm("ljmp %04x:%08x", cpu.cs, cpu.eip);
    return len;
}
