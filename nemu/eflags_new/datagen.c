#include <stdio.h>
#include <stdlib.h>

/* clear CF */
#define CLC __asm__ __volatile__ ("clc");

/* set CF */
#define STC __asm__ __volatile__ ("stc");

int my_rand()
{
	int a = 0;
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	return a;
}

int getf(int f)
{
	return f & 0x8c5;
}

#define MAKE_FUNC_WITH_PREPARE(name, prepare, instr) \
int get_ ## name ## _eflags(int a, int b, int *na, int *nb) \
{ \
	volatile int f, ta, tb; \
	__asm__ __volatile__ ("mov %0, %%eax"::"m"(a)); \
	__asm__ __volatile__ ("mov %0, %%ecx"::"m"(b)); \
	prepare \
	instr \
	__asm__ __volatile__ ("pushf"); \
	__asm__ __volatile__ ("pop %edx"); \
	__asm__ __volatile__ ("mov %%eax, %0":"=m"(ta)); \
	__asm__ __volatile__ ("mov %%ecx, %0":"=m"(tb)); \
	__asm__ __volatile__ ("mov %%edx, %0":"=m"(f)::"eax","ecx","edx"); \
	*na = ta; \
	*nb = tb; \
	return getf(f); \
}

#define RUN_FUNC(name, N) \
    for (i = 1; i <= N; i++) { \
		    int a = my_rand(), b = my_rand(), f, na, nb; \
		    switch (i) { \
		        case 1:  b = -a;     break; \
		        case 2:  b = -a - 1; break; \
		        case 3:  b = -a + 1; break; \
		        case 4:  b = -a - 2; break; \
		        case 5:  b = -a + 2; break; \
		        case 6:  a = -b;     break; \
		        case 7:  a = -b - 1; break; \
		        case 8:  a = -b + 1; break; \
		        case 9:  a = -b - 2; break; \
		        case 10: a = -b + 2; break; \
		        case 11: a = 0;  break;     \
		        case 12: a = 1;  break;     \
		        case 13: a = -1; break;     \
		        case 14: a = 2;  break;     \
		        case 15: a = -2; break;     \
		        case 16: b = 0;  break;     \
		        case 17: b = 1;  break;     \
		        case 18: b = -1; break;     \
		        case 19: b = 2;  break;     \
		        case 20: b = -2; break;     \
		        case 21: a = 0;  b = 0;  break; \
		        case 22: a = 0;  b = 1;  break; \
		        case 23: a = 0;  b = -1; break; \
		        case 24: a = 0;  b = 2;  break; \
		        case 25: a = 0;  b = -2; break; \
		        case 26: a = 1;  b = 0;  break; \
		        case 27: a = 1;  b = 1;  break; \
		        case 28: a = 1;  b = -1; break; \
		        case 29: a = 1;  b = 2;  break; \
		        case 30: a = 1;  b = -2; break; \
		        case 31: a = -1; b = 0;  break; \
		        case 32: a = -1; b = 1;  break; \
		        case 33: a = -1; b = -1; break; \
		        case 34: a = -1; b = 2;  break; \
		        case 35: a = -1; b = -2; break; \
		 		case 36: a = 2;  b = 0;  break; \
		        case 37: a = 2;  b = 1;  break; \
		        case 38: a = 2;  b = -1; break; \
		        case 39: a = 2;  b = 2;  break; \
		        case 40: a = 2;  b = -2; break; \
		        case 41: a = -2; b = 0;  break; \
		        case 42: a = -2; b = 1;  break; \
		        case 43: a = -2; b = -1; break; \
		        case 44: a = -2; b = 2;  break; \
		        case 45: a = -2; b = -2; break; \
		    } \
		    f = get_ ## name ## _eflags(a, b, &na, &nb); \
		    printf(# name " %08x %08x %08x %08x %08x\n", a, b, f, na, nb); \
	    }

#define MAKE_FUNC_GROUP_WITH_CF(instr) \
    MAKE_FUNC_WITH_PREPARE(instr ## 4_clc, CLC, __asm__ __volatile__ (# instr " %ecx, %eax"); ) \
    MAKE_FUNC_WITH_PREPARE(instr ## 4_stc, STC, __asm__ __volatile__ (# instr " %ecx, %eax"); ) \
    MAKE_FUNC_WITH_PREPARE(instr ## 2_clc, CLC, __asm__ __volatile__ (# instr " %cx, %ax"); ) \
    MAKE_FUNC_WITH_PREPARE(instr ## 2_stc, STC, __asm__ __volatile__ (# instr " %cx, %ax"); ) \
    MAKE_FUNC_WITH_PREPARE(instr ## 1_clc, CLC, __asm__ __volatile__ (# instr " %cl, %al"); ) \
    MAKE_FUNC_WITH_PREPARE(instr ## 1_stc, STC, __asm__ __volatile__ (# instr " %cl, %al"); )

#define RUN_FUNC_GROUP_WITH_CF(instr) \
    RUN_FUNC(instr ## 4_clc, N4) \
    RUN_FUNC(instr ## 4_stc, N4) \
    RUN_FUNC(instr ## 2_clc, N2) \
    RUN_FUNC(instr ## 2_stc, N2) \
    RUN_FUNC(instr ## 1_clc, N1) \
    RUN_FUNC(instr ## 1_stc, N1)


MAKE_FUNC_GROUP_WITH_CF(add)
MAKE_FUNC_GROUP_WITH_CF(adc)
MAKE_FUNC_GROUP_WITH_CF(sub)
MAKE_FUNC_GROUP_WITH_CF(sbb)

int main()
{
	//__asm__ __volatile__ ("int3");
	
	int i, N4 = 1000, N2 = 1000, N1 = 256 * 256 * 100;
	
	RUN_FUNC_GROUP_WITH_CF(add)
	RUN_FUNC_GROUP_WITH_CF(adc)
	RUN_FUNC_GROUP_WITH_CF(sub)
	RUN_FUNC_GROUP_WITH_CF(sbb)

	return 0;
}
