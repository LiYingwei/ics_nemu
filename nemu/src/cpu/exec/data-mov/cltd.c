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

<<<<<<< HEAD
make_helper(cwtl) {
    int16_t ax = cpu.gpr[0]._16;
    int32_t eax = (int32_t) ax;
    cpu.eax = eax;
    return 1;
}

=======
>>>>>>> dev_PA2
