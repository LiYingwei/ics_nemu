//
// Created by lyw on 15-11-15.
//

#include <cpu/reg.h>
#include "cpu/reg.h"
#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(cltd) {
    if ((int32_t) cpu.eax < 0) cpu.edx = 0xFFFFFFFF;
    else cpu.edx = 0;
    print_asm("cltd");
    return 1;
}

make_helper(cwtl) {
    int32_t tmp = (int16_t) reg_w(0);
    cpu.eax = tmp;
    print_asm("cwtl");
    return 1;
}
