//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr pusha

static void do_execute () {

#if DATA_BYTE == 1
assert(0);
#endif


    print_asm("pusha%c", DATA_BYTE == 2 ? ' ':'d');
    assert(0);
}

make_helper(concat3(instr,_,SUFFIX))
{
    do_execute();
    return 1;
}

#include "cpu/exec/template-end.h"
