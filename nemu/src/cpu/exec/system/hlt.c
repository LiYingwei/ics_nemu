//
// Created by lyw on 16-3-5.
//

#include <cpu/reg.h>
#include "cpu/exec/helper.h"

make_helper(hlt) {
    while (cpu.INTR == 0);
    return 1;
}