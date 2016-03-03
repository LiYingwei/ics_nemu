//
// Created by lyw on 16-2-19.
//

#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "lidt-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "lidt-template.h"
#undef DATA_BYTE

make_helper_v(lidt_rm);
