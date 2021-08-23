#ifndef _ASM_FLOW_OPTIMIZE_H
#define _ASM_FLOW_OPTIMIZE_H
#include "codegen.h"
#include "common.h"
#include "asm_optimize.h"

void ASM_Flow_Optimize(std::list<asm_basic_block_t*>& blks);

#endif
