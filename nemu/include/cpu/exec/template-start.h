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

#define MASK ((1<<DATA_BYTE) - 1)

#define SIGN_SHIFT (DATA_BYTE * 8 - 1)

#define EFLAGS_BASE(op1, op2, cin, patch, updatecf) ({ \
    uint32_t __cin, __patch;                                                \
    uint32_t __a, __b, __c, __ch, __as, __bs, __cs, __cc, __p;              \
    __cin = !!(cin);                                                        \
    __patch = !!(patch);                                                    \
    __a = ((uint32_t)(op1)) & MASK;                                         \
    __b = ((uint32_t)(op2)) & MASK;                                         \
    if (__cin) { __b = ~__b & MASK; }                                       \
    __c = (__a + __b + (__cin ^ __patch)) & MASK;                           \
    __ch = (__a + __b) & MASK;                                              \
    __cc = (__ch < __a || __ch < __b || __c < __ch);                        \
    __as = __a >> SIGN_SHIFT;                                               \
    __bs = __b >> SIGN_SHIFT;                                               \
    __cs = __c >> SIGN_SHIFT;                                               \
    __p = __c ^ (__c >> 4);                                                 \
    __p = __p ^ (__p >> 2);                                                 \
    __p = (__p ^ (__p >> 1)) & 1;                                           \
                                                                            \
    cpu.ZF = (__c == 0);                                                    \
    cpu.SF =  __cs;                                                         \
    cpu.OF = (__as == __bs && __as != __cs);                                \
    if (updatecf) cpu.CF = (__cc ^ __cin);                                 \
    cpu.PF = (__p ^ 1);                                                    \
                                                                            \
    __c;                                                                    \
})

/* EFLAGS_ALU(a, b, c, p) : same as EFLAGS(), but p is used by ADC and SBB
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS_ALU(a, b, c, p) EFLAGS_BASE((a), (b), (c), (p), 1)

/*#define EFLAGS_ALU(a, B, S, CIN) ({\
    long long ans; \
    DATA_TYPE b = B; \
    bool Cin = CIN ^ S; \
    if(S) b = b ^ ((1 << DATA_BYTE) - 1); \
    ans = a + b + Cin; \
    cpu.ZF = ans == 0 ? 1 : 0; \
    cpu.SF = (bool) ((ans >> (DATA_BYTE - 1)) & 1); \
    cpu.CF = (bool) ((ans >> (DATA_BYTE)) & 1); \
    cpu.OF = (bool) ((bool)( ((a >> (DATA_BYTE - 1)) & 1)  == ((b >> (DATA_BYTE - 1)) & 1) ) \
                    && (bool)( ((a >> (DATA_BYTE - 1)) & 1) != ((ans >> (DATA_BYTE - 1)) & 1) )); \
    int i; \
    cpu.PF = 1; \
    for(i = 0; i < 8; i++) if( (ans >> i) & 1 ) cpu.PF ^= 1; \
    (DATA_TYPE) ans; \
})*/

/*#define EFLAGS_ALU(a, b, S, Cin) \
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
})*/

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

