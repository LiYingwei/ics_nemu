//
// Created by lyw on 15-11-19.
//

#include <cpu/decode/modrm.h>
#include <cpu/reg.h>
#include "cpu/decode/decode.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "cpu/exec/template-start.h

#define MOV(con) if(con) OPERAND_W(op_dest, op_src->val); break
#define CF cpu.CF
#define ZF cpu.ZF
#define SF cpu.SF
#define OF cpu.OF
#define PF cpu.PF
make_helper(cmccov) {
    int instr_len = 1;
    uint8_t cc = instr_fetch(eip , 1);
    instr_len += concat(decode_rm2r_,SUFFIX)(eip+1);
    switch (cc) {
        case 0x40: set(OF);
        case 0x41: set(!OF);
        case 0x42: set(CF);
        case 0x43: set(!CF);
        case 0x44: set(ZF);
        case 0x45: set(!ZF);
        case 0x46: set(CF|ZF);
        case 0x47: set(!CF&!ZF);
        case 0x48: set(SF);
        case 0x49: set(!SF);
        case 0x4A: set(PF);
        case 0x4B: set(!PF);
        case 0x4C: set(SF!=OF);
        case 0x4D: set(SF==OF);
        case 0x4E: set(ZF|(SF!=OF));
        case 0x4F: set(ZF&(SF!=OF));
        default: assert(0);
    }
    return instr_len;
}

#undef CF
#undef ZF
#undef SF
#undef OF
#undef PF
#undef MOV(con)
#include "cpu/exec/template-end.h

