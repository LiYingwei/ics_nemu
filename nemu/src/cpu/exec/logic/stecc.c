//
// Created by lyw on 15-11-15.
//

#include <cpu/decode/modrm.h>
#include <cpu/reg.h>
#include "cpu/decode/decode.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

#define set(con) write_operand_b(op_src, con); break
#define CF cpu.CF
#define ZF cpu.ZF
#define SF cpu.SF
#define OF cpu.OF
#define PF cpu.PF
make_helper(setcc) {
    int instr_len = 1;
    uint8_t cc = instr_fetch(eip , 1);
    instr_len += decode_rm_b(eip+1);
    switch (cc) {
        case 0x90: set(OF);
        case 0x91: set(!OF);
        case 0x92: set(CF);
        case 0x93: set(!CF);
        case 0x94: set(ZF);
        case 0x95: set(!ZF);
        case 0x96: set(CF|ZF);
        case 0x97: set(!CF&!ZF);
        case 0x98: set(SF);
        case 0x99: set(!SF);
        case 0x9A: set(PF);
        case 0x9B: set(!PF);
        case 0x9C: set(SF!=OF);
        case 0x9D: set(SF==OF);
        case 0x9E: set(ZF|(SF!=OF));
        case 0x9F: set(!ZF&(SF==OF));
        default: assert(0);
    }
    return instr_len;
}

#undef CF
#undef ZF
#undef SF
#undef OF
#undef PF
