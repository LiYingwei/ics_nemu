#include <stdio.h>
#include "include/cpu/exec/template-start.h"
CPU_state cpu;
#if DATA_BYTE == 1
#define DATA_BYTE_STR "1"
#elif DATA_BYTE == 2
#define DATA_BYTE_STR "2"
#elif DATA_BYTE == 4
#define DATA_BYTE_STR "4"
#else
#error
#endif
int main()
{
    int a, b, na, nb, f, f2, r;
    int cnt = 0;
    char buf[5];
    while (scanf("%s%x%x%x%x%x", buf, &a, &b, &f, &na, &nb) == 6) {
        if (strcmp(buf, "add" DATA_BYTE_STR "_clc") == 0) {
            r = EFLAGS_ALU(a, b, 0, 0);
        } else if (strcmp(buf, "add" DATA_BYTE_STR "_stc") == 0) {
            r = EFLAGS_ALU(a, b, 0, 0);
        } else if (strcmp(buf, "sub" DATA_BYTE_STR "_clc") == 0) {
            r = EFLAGS_ALU(a, b, 1, 0);
        } else if (strcmp(buf, "sub" DATA_BYTE_STR "_stc") == 0) {
            r = EFLAGS_ALU(a, b, 1, 0);
        } else if (strcmp(buf, "adc" DATA_BYTE_STR "_clc") == 0) {
            r = EFLAGS_ALU(a, b, 0, 0);
        } else if (strcmp(buf, "adc" DATA_BYTE_STR "_stc") == 0) {
            r = EFLAGS_ALU(a, b, 0, 1);
        } else if (strcmp(buf, "sbb" DATA_BYTE_STR "_clc") == 0) {
            r = EFLAGS_ALU(a, b, 1, 0);
        } else if (strcmp(buf, "sbb" DATA_BYTE_STR "_stc") == 0) {
            r = EFLAGS_ALU(a, b, 1, 1);
        } else {
            continue;
        }
        na &= ((1 << (DATA_BYTE * 8 - 1)) << 1) - 1;
        f2 = cpu.EFLAGS & 0x8c5;
        if (f != f2 || r != na || nb != b) {
            printf("ERROR: type=%s a=%x b=%x ans=%x right=%x ansf=%x rightf=%x\n", buf, a, b, r, na, f2, f);
            return 1;
        }
        cnt++;
    }
    printf("DATA_BYTE=%d : %d TESTS OK\n", DATA_BYTE, cnt);
    return 0;
}
