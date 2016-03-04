#include <cpu/reg.h>
#include "cpu/exec/helper.h"

#if DATA_BYTE == 1

#define SUFFIX b
#define DATA_TYPE uint8_t
#define DATA_TYPE_S int8_t

#elif DATA_BYTE == 2

#define SUFFIX w
#define DATA_TYPE uint16_t
#define DATA_TYPE_S int16_t

#elif DATA_BYTE == 4

#define SUFFIX l
#define DATA_TYPE uint32_t
#define DATA_TYPE_S int32_t

#else

#error unknown DATA_BYTE

#endif

#define REG(index) concat(reg_, SUFFIX) (index)
#define REG_NAME(index) concat(regs, SUFFIX) [index]

#define MEM_R(addr, sreg) swaddr_read(addr, DATA_BYTE, sreg)
#define MEM_W(addr, data, sreg) swaddr_write(addr, DATA_BYTE, data, sreg)

#define OPERAND_W(op, src) concat(write_operand_, SUFFIX) (op, src)

#define MSB(n) ((DATA_TYPE)(n) >> ((DATA_BYTE << 3) - 1))

#define EFLAGS_ALU(a, b, S, Cin) \
({ \
    int i; \
    bool A[DATA_BYTE * 8], B[DATA_BYTE * 8], F[DATA_BYTE * 8], C[DATA_BYTE * 8 + 1]; \
\
    C[0] = Cin ^ S; \
    for (i = 0; i < DATA_BYTE * 8; i++) { \
        A[i] = (a >> i) & true; \
        B[i] = S ^ ((b >> i) & true); \
    } \
\
    cpu.ZF = 0; \
\
    for (i = 0; i < DATA_BYTE * 8; i++) { \
        F[i] = A[i] ^ B[i] ^ C[i]; \
        C[i + 1] = (A[i] & B[i]) | (A[i] & C[i]) | (B[i] & C[i]); \
        cpu.ZF |= F[i]; \
    } \
\
    cpu.ZF = ~cpu.ZF; \
    cpu.OF = C[DATA_BYTE * 8] ^ C[DATA_BYTE * 8 - 1]; \
    cpu.SF = F[DATA_BYTE * 8 - 1]; \
    cpu.CF = S ^ C[DATA_BYTE * 8]; \
    cpu.PF = true; \
    for (i = 0; i < 8; i++) cpu.PF ^= F[i]; \
\
    DATA_TYPE Sum = 0; \
    for (i = 0; i < DATA_BYTE * 8; i++) { \
        Sum ^= F[i] << i; \
    } \
    Sum; \
})

#define INSTR_POP() \
({ \
    DATA_TYPE ret = MEM_R(cpu.esp, R_SS); \
    cpu.esp += DATA_BYTE; \
    ret; \
})

#define INSTR_PUSH(val) \
do{ \
    cpu.esp -= (DATA_BYTE == 2) ? 2 : 4; \
    swaddr_write(cpu.esp, (DATA_BYTE == 2)? 2 : 4,val, R_SS); \
}while(0)

#define EFLAGS_UPDATE_LOGIC(val) \
do{ \
    uint8_t low8 = val & 0xFF; \
    cpu.PF = 1; \
    while (low8) { \
        cpu.PF ^= low8 & 1; \
        low8 >>= 1; \
    } \
    cpu.ZF = val == 0 ? true : false; \
    cpu.SF = (bool) (val >> (DATA_BYTE * 8 - 1)); \
    cpu.CF = 0; \
    cpu.OF = 0; \
}while(0)

#define eflags_add_sub(_dest, _src, _sub, _byte)\
do {\
    uint32_t dest = (_dest), src = (_src), sub = (_sub) & 1, byte = (_byte), needCF = (_sub) >> 1 ? cpu.CF : 0;\
    byte *= 8;\
    uint32_t y = sub ? ~ src : src;\
    uint32_t res = dest + y + (sub ^ needCF);\
    res &= ((1 << (byte - 1)) << 1) - 1;\
    dest &= ((1 << (byte - 1)) << 1) - 1;\
    src &= ((1 << (byte - 1)) << 1) - 1;\
    y &= ((1 << (byte - 1)) << 1) - 1;\
    cpu.ZF = !res;\
    cpu.OF = (dest & (1 << (byte - 1))) == (y  & (1 << (byte - 1))) && (y  & (1 << (byte - 1))) != (res  & (1 << (byte - 1)));\
    cpu.CF = sub ^ ((res - (sub ^ needCF)) < dest || (res - (sub ^ needCF)) < y || res < (sub ^ needCF));\
    cpu.PF = 1;\
    uint32_t i = 1, j = res;\
    for (; i <= 8; i ++ , j /= 2)\
        cpu.PF ^= j & 1;\
    cpu.SF = res  >> (byte - 1);\
}while (0)

