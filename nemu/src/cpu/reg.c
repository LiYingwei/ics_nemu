#include "nemu.h"
#include <stdlib.h>
#include <time.h>
#include <cpu/reg.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;
	uint32_t eflags_sample = rand();
	cpu.EFLAGS.eflags = eflags_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);

	assert(cpu.EFLAGS.CF == (eflags_sample & 1));
	assert(cpu.EFLAGS.t1 == ((eflags_sample >> 1) & 1));
	assert(cpu.EFLAGS.PF == ((eflags_sample >> 2) & 1));
	assert(cpu.EFLAGS.t2 == ((eflags_sample >> 3) & 1));
	assert(cpu.EFLAGS.AF == ((eflags_sample >> 4) & 1));
	assert(cpu.EFLAGS.t3 == ((eflags_sample >> 5) & 1));
	assert(cpu.EFLAGS.ZF == ((eflags_sample >> 6) & 1));
	assert(cpu.EFLAGS.SF == ((eflags_sample >> 7) & 1));
	assert(cpu.EFLAGS.TF == ((eflags_sample >> 8) & 1));
	assert(cpu.EFLAGS.IF == ((eflags_sample >> 9) & 1));
	assert(cpu.EFLAGS.DF == ((eflags_sample >> 10) & 1));
	assert(cpu.EFLAGS.OF == ((eflags_sample >> 11) & 1));
	assert(cpu.EFLAGS.IOPL == ((eflags_sample >> 12) & 1));
	assert(cpu.EFLAGS.NT == ((eflags_sample >> 13) & 1));
	assert(cpu.EFLAGS.t4 == ((eflags_sample >> 14) & 1));
	assert(cpu.EFLAGS.RF == ((eflags_sample >> 15) & 1));
	assert(cpu.EFLAGS.VM == ((eflags_sample >> 16) & 1));

	assert(eflags_sample == cpu.EFLAGS.eflags);
}

