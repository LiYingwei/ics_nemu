//
// Created by lyw on 15-11-15.
//
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"


make_helper(stc)
{
    cpu.CF = 1;
    print_asm("stc");
    return 1;
}
make_helper(clc)
{
    cpu.CF = 0;
    print_asm("clc");
    return 1;
}
make_helper(cmc)
{
    cpu.CF = ~cpu.CF;
    print_asm("cmc");
    return 1;
}
make_helper(cld)
{
    cpu.DF = 0;
    print_asm("cld");
    return 1;
}
make_helper(std)
{
    cpu.DF = 1;
    print_asm("std");
    return 1;
}
make_helper(cli)
{
    cpu.IF = 0;
    print_asm("cli");
    return 1;
}
make_helper(sti)
{
    cpu.IF = 1;
    print_asm("sti");
    return 1;
}
