#include <setjmp.h>
#include <cpu/reg.h>
#include "common.h"
#include "memory/memory.h"
#include "cpu/mmu.h"
extern jmp_buf jbuf;

static void push(uint32_t val) {
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, val, R_SS);
}

/*static uint32_t pop(uint32_t val) {
    uint32_t ret = swaddr_read(cpu.esp, 4, R_SS);
    cpu.esp += 4;
    return ret;
}*/
extern bool DONT_TOUCH_MY_EIP;
void raise_intr(uint8_t NO) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
     * That is, use ``NO'' to index the IDT.
     */
    push(cpu.EFLAGS);
    push(cpu.cs);
    push(cpu.eip);

    uint32_t gds[2];
    Log("No = %d\n", NO);
    NO = 1;
    gds[0] = lnaddr_read((cpu.IDTR >> 16) + NO * sizeof(GateDesc), 4); Log();
    gds[1] = lnaddr_read((cpu.IDTR >> 16) + NO * sizeof(GateDesc) + 4, 4); Log();
    GateDesc * gd = (GateDesc *)gds; Log();
    cpu.cs = gd->segment; Log();
    load_segment_cache(R_CS); Log();

    cpu.eip = cpu.cs_cache_base + gd->offset_15_0 + (gd->offset_31_16 << 16);
    DONT_TOUCH_MY_EIP = true;
    /* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}
