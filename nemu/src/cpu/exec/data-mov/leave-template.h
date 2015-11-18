#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat3(instr,_,SUFFIX)) {
    REG(R_ESP) = REG(R_EBP);
    REG(R_EBP) = INSTR_POP();
    print_asm("leave");
    return 1+(DATA_BYTE == 2);
}
#include "cpu/exec/template-end.h"
