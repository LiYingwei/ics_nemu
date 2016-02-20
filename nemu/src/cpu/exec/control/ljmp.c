//
// Created by lyw on 15-10-21.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "ljmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "ljmp-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(ljmp_i);

