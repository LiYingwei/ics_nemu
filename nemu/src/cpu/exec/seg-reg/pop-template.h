//
// Created by lyw on 15-10-18.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute () {
    OPERAND_W(op_src, INSTR_POP());

    print_asm_template1();
}

make_instr_helper(r);
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm);
#endif

#include "cpu/exec/template-end.h"

