#ifndef _IRTREE_OPTIMIZE_H
#define _IRTREE_OPTIMIZE_H
#include "IR.h"
#include "AST.h"
#include "common.h"
#include "codegen.h"
#include "flow_optimize.h"
// use some of its static variables

void IRTree_Optimize(IR_t* root);
void IRLine_Optimize(std::list<IR_t*>& LinearIR, int current_id, bool is_fake);
void IRLine_Optimize_PostFlow(std::list<IR_t*>& LinearIR, std::list<IR_t*>& Fake_LinearIR);
bool Invariant_Code_Motion(std::list<IR_t*>& ir_outer,
			   std::list<IR_t*>& ir_inner);
  
#endif
