#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat3(instr,_,SUFFIX)) {
    cpu.esp = cpu.ebp;
    cpu.ebp = INSTR_POP();
    print_asm("leave");
    return 1;
}
#include "cpu/exec/template-end.h"
