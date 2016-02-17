//
// Created by lyw on 16-2-16.
//

#define instr cltd

make_helper(cltd_w){
    if ((int16_t)reg_w(0) < 0) reg_w(2) = 0xffff;
    else reg_w(2) = 0;
    print_asm("cltdw");
    return 1;
}

make_helper(cltd_l){
    if ((int32_t)cpu.eax < 0) cpu.edx = 0xffffffff;
    else cpu.edx = 0;
    print_asm("cltdl");
    return 1;
}
