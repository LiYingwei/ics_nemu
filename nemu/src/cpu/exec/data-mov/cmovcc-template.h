//
// Created by lyw on 15-11-19.
//

#include <cpu/decode/modrm.h>
#include <cpu/reg.h>
#include "cpu/decode/decode.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "cpu/exec/template-start.h"

#define MOV(con) if(con) OPERAND_W(op_dest, op_src->val); break
#define CF cpu.CF
#define ZF cpu.ZF
#define SF cpu.SF
#define OF cpu.OF
#define PF cpu.PF
make_helper(concat(cmovcc_,SUFFIX)) {
    int instr_len = 1;
    uint8_t cc = instr_fetch(eip , 1);
    instr_len += concat(decode_rm2r_,SUFFIX)(eip+1);
    switch (cc) {
        case 0x40: MOV(OF);
        case 0x41: MOV(!OF);
        case 0x42: MOV(CF);
        case 0x43: MOV(!CF);
        case 0x44: MOV(ZF);
        case 0x45: MOV(!ZF);
        case 0x46: MOV(CF|ZF);
        case 0x47: MOV(!CF&!ZF);
        case 0x48: MOV(SF);
        case 0x49: MOV(!SF);
        case 0x4A: MOV(PF);
        case 0x4B: MOV(!PF);
        case 0x4C: MOV(SF!=OF);
        case 0x4D: MOV(SF==OF);
        case 0x4E: MOV(ZF|(SF!=OF));
        case 0x4F: MOV(ZF&(SF!=OF));
        default: assert(0);
    }
    return instr_len;
}

#undef CF
#undef ZF
#undef SF
#undef OF
#undef PF
#undef MOV
#include "cpu/exec/template-end.h"

