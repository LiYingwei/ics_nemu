//
// Created by lyw on 15-11-19.
//

#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "cmovcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmovcc-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(cmovcc);

