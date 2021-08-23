#ifndef _ASM_OPTIMIZE_H
#define _ASM_OPTIMIZE_H
#include "codegen.h"
#include "common.h"
#include "IR.h"
#include<list>
void ASM_Optimize(std::list<instr_t*>::iterator& begin,
		  std::list<instr_t*>::iterator end,
		  std::list<instr_t*>& InstrList,
		  bool is_fake);
std::list<instr_t*> Instruction_Scheduling(std::list<instr_t*>::iterator begin,
					   std::list<instr_t*>::iterator end);

std::set<std::string> Get_Killed_vars(const instr_t& asmop);
std::set<std::string> Get_Referenced_vars(const instr_t& asmop);




struct mem_offset_t
{
  operand_t base;
  operand_t offset;
  mem_offset_t(operand_t oper);
  std::string get_literal();
};

// Not very exact...
const static std::map<MNEMONIC_TYPE, int> INSTR_EXECUTION_TIME =
  {
    {MNEMO_RAW,    0},     
    {MNEMO_ADD,    1},     
    {MNEMO_SUB,    1},     
    {MNEMO_SDIV,   2},    
    {MNEMO_MOV,    1},     
    {MNEMO_MOV32,  1},   
    {MNEMO_LDR,    2},     
    {MNEMO_STR,    2},     
    {MNEMO_LDR_GLB,3}, 
    {MNEMO_MUL,    2},     
    {MNEMO_LSL,    1},     
    {MNEMO_LSR,    1},     
    {MNEMO_ASR,    1},     
    {MNEMO_MLA,    2},     
    {MNEMO_MLS,    2},     
    {MNEMO_CMP,    1},     
    {MNEMO_B,      3},       
    {MNEMO_BL,     4},      
    {MNEMO_NOP,    0},     
    {MNEMO_LABEL,  0},   
    {MNEMO_NOTE,   0},    
    {MNEMO_PASSNOTE,0},
  };

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
class asm_basic_block_t
{
// private:
//   int __has_ts;
public:
  int id;
  std::list<instr_t*>::iterator begin;
  std::list<instr_t*>::iterator end;
  std::vector<asm_basic_block_t*> out_edges;
  std::vector<asm_basic_block_t*> in_edges;
  
  // std::set<std::string> require_variables;
  // std::set<std::string> provide_variables;
  std::set<std::string> liveIn;
  std::set<std::string> UEVar;
  std::set<std::string> VarKill;

  std::set<std::string> liveOut;
  bool compute_liveout();
  void Finalize_LiveIn();
  // Live Variable DataFlow Analyze,
  // Reference: [Engineering a Compiler, K. Cooper, P.447]

  // Lazy Code Motion, ref:
  // [Engineering A Compiler 2nd, Section 10.3.1]
  // [Lazy Code Motion, J.Knoop, O.Ruthing, B.Steffen, 1992]
  // [A Variation of Knoop, Ruthing and Steffen's Lazy Code Motion, Karl-Heinz, 1993]
  //////////////////////////////////////////
  // Intra-block
  std::set<std::string> DEExpr;
  std::set<std::string> UEExpr;
  std::set<std::string> ExprKill;
  // Inter-block: Availability
  std::set<std::string> AvailIn;
  std::set<std::string> AvailOut;
  bool Compute_AvailIn(const std::set<std::string> all_exprs);
  void Finalize_AvailOut(const std::set<std::string> all_exprs);
  // Inter-block: Anticipate
  std::set<std::string> AntIn;
  std::set<std::string> AntOut;
  void Finalize_AntIn(const std::set<std::string> all_exprs);
  bool Compute_AntOut(const std::set<std::string> all_exprs);
  // Inter-block: Earliest and Later Placement
  std::map<asm_basic_block_t*, std::set<std::string>> Earliest;
  std::map<asm_basic_block_t*, std::set<std::string>> Later;
  std::set<std::string> LaterIn;
  void Finalize_Earliest(const std::set<std::string> all_exprs);
  bool Compute_LaterIn(const std::set<std::string> all_exprs);
  void Finalize_Later(const std::set<std::string> all_exprs);
  // Insert and Delete
  std::map<asm_basic_block_t*, std::set<std::string>> Insert;
  std::set<std::string> Delete;
  void Finalize_Insert(const std::set<std::string> all_exprs);
  void Finalize_Delete(const std::set<std::string> all_exprs);
  void print_Insert(std::ostream& out);
  void print_Delete(std::ostream& out);
  //////////////////////////////////////////

  
  asm_basic_block_t(std::list<instr_t*>::iterator begin,
		    std::list<instr_t*>::iterator end);
  void print(std::ostream& out);
  static void AddEdge(asm_basic_block_t* from,
		      asm_basic_block_t* to);

  void get_register_dependency();

};

struct LiveRange_t
{
  int left, right;
  std::string  representative_reg_name;
  std::string* defined_position;
  std::vector<std::string*> referenced_position;
  bool rewritable;
  void rewrite_reference(std::string rewrited);
  void rewrite_definition(std::string rewrited);
  // Rewrite all reference of [representative_reg_name] in [referenced_position]
  // into [rewrited]
  LiveRange_t(int left, int right, std::string reg_name);
};

struct LiveRangeSet_t
{
public:
  std::map<std::string, std::set<LiveRange_t*>> LR_Array;
  LiveRange_t* get_Reference_LiveRange(int position, std::string regname);
  bool Check_Interleave(LiveRange_t* A, LiveRange_t* B);
  // Check if A and B are interleaved.
public:
  LiveRange_t* get_Defined_LiveRange(int position, std::string regname);
  // Get LR for [regname] at instruction [position]
  // Only valid for [Killed] vars, not for [Referenced] vars
  
  bool Check_Alias_Replacable(const instr_t& asmop);
  // Check the MOV operation is an alias
  bool Check_Available_Replacable(LiveRange_t* inner, LiveRange_t* outer);

  bool Check_Interleave(LiveRange_t* lr, std::string regname);
  
  LiveRangeSet_t(const asm_basic_block_t& asmbb);
  // Construct LR Set inside an asm block

  std::string Get_Non_Interleaved_Register(instr_t& asmop);
  // Get a [non-interleaved] register against [asmop]'s defined LiveRange
  // try to replace corrsepond register for better instruction scheduling.
  std::set<std::string> Get_Non_Interleaved_RegisterSet(instr_t& asmop);
  
  void Try_Rename_Register(instr_t& asmop);
  
  
};
void Eliminate_Useless_Instructions(asm_basic_block_t* asmbb);
void Instruction_Scheduling(asm_basic_block_t* asmbb);

class _asm_ExpressionSet_t
{
private:
  std::map<_asm_rhs_t, std::pair<std::string, instr_t*> > definition_expression;
  void Invalidate_Instr(instr_t& asmop);
  void Try_To_Rewrite(instr_t& asmop, LiveRangeSet_t& lrset);
  int Get_Invalidate_Distance(std::string rewrite_var, int current_line);
  void Try_Rename_Register(instr_t& asmop, LiveRangeSet_t& lrset);
  
public:
  void Proceed(asm_basic_block_t* blk, LiveRangeSet_t& lrset);
  // r4 = r2 + r1
  // r1 = r1 + r3 [invalidate r1]
  // r2 = r1 + r3
  // r5 <- r2   ;; r5 <- r1
  // Invalidate first, then insert.
  
};
#endif
