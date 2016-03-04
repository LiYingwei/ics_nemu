#include <cpu/decode/operand.h>
#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX), R_DS);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr, R_DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#undef instr
#define instr movsx
static void do_execute() {
    if(op_src->type == OP_TYPE_REG) op_src->val = reg_b(op_src->reg);
    DATA_TYPE_S val = (DATA_TYPE_S) ((int8_t)((uint8_t) op_src->val));
    OPERAND_W(op_dest, (DATA_TYPE) val);
    print_asm_template2();
}
make_instr_helper(rm2r)
#undef instr
#define instr movsxl
static void do_execute() {
    DATA_TYPE_S val = (DATA_TYPE_S) ((int16_t)((uint16_t) op_src->val));
    OPERAND_W(op_dest, (DATA_TYPE)val);
    print_asm_template2();
}
make_instr_helper(rm2r)
#undef instr

#define instr movzx
static void do_execute() {
    if(op_src->type == OP_TYPE_REG) op_src->val = reg_b(op_src->reg);
    uint8_t val = (uint8_t) op_src->val;
    OPERAND_W(op_dest, (DATA_TYPE) val);
    print_asm_template2();
}
make_instr_helper(rm2r)
#undef instr
#define instr movzxl
static void do_execute() {
    uint16_t val = (uint16_t) op_src->val;
    OPERAND_W(op_dest, (DATA_TYPE) val);
    print_asm_template2();
}
make_instr_helper(rm2r)
#undef instr
#include "cpu/exec/template-end.h"
