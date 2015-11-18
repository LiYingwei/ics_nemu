//
// Created by lyw on 15-11-15.
//
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"


make_helper(stc)
{
    cpu.CF = 1;
    return 1;
}
make_helper(clc)
{
    cpu.CF = 0;
    return 1;
}
make_helper(cmc)
{
    cpu.CF = ~cpu.CF;
    return 1;
}
make_helper(cld)
{
    cpu.DF = 0;
    return 1;
}
make_helper(std)
{
    cpu.DF = 1;
    return 1;
}

