//
// Created by lyw on 16-2-16.
//

#define instr cwtl

make_helper(cwtl_w){
    int8_t tmp = reg_b(0);
    reg_w(0) = tmp;
    print_asm("cwtlw");
    return 1;
}

make_helper(cwtl_l){
    int16_t tmp = reg_w(0);
    cpu.eax = tmp;
    print_asm("cwtll");
    return 1;
}

