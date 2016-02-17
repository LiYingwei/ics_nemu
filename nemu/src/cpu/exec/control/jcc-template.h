//
// Created by lyw on 15-10-21.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define do_jcc_helper(instr, con) \
static void do_execute_ins(instr)() { \
		DATA_TYPE_S dt = op_src->val; \
        if(con) cpu.eip += dt; \
		print_asm(str(instr) str(SUFFIX) " %d", dt); \
    } \
    make_instr_helper_ins(i, instr)

#define CF cpu.CF
#define ZF cpu.ZF
#define SF cpu.SF
#define OF cpu.OF
#define PF cpu.PF

do_jcc_helper(ja, !CF&&!ZF)
do_jcc_helper(jae, !CF)
do_jcc_helper(jb, CF)
do_jcc_helper(jbe, CF||ZF)
do_jcc_helper(je, ZF)
do_jcc_helper(jg, !ZF&&SF==OF)
do_jcc_helper(jge, SF==OF)
do_jcc_helper(jl, SF!=OF)
do_jcc_helper(jle, ZF||SF!=OF)
do_jcc_helper(jno, !OF)
do_jcc_helper(jne, !ZF)
do_jcc_helper(jnp, !PF)
do_jcc_helper(jns, !SF)
do_jcc_helper(jnz, !ZF)
do_jcc_helper(jo, OF)
do_jcc_helper(jp, PF)
do_jcc_helper(js, SF)
do_jcc_helper(jcxz, !REG(R_ECX))
#undef ZF
#undef CF
#undef SF
#undef OF
#undef PF

#include "cpu/exec/template-end.h"

