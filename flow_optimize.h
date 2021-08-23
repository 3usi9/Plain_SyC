#ifndef _FLOW_OPTIMIZE_H
#define _FLOW_OPTIMIZE_H
#include<vector>
#include<assert.h>
#include<typeinfo>
#include "codegen.h"
#include "common.h"
#include "IR.h"
void Flow_Optimize(std::list<basic_block_t*>& blks);

bool check_scheduable(const IROper_t& irop);

class ExpressionSet_t
{
public:
  // Invalidate some expressions
  bool RewriteBlock(basic_block_t *blk, const std::map<std::string, IROper_t *> define_chain);
private:
  void InitExprSet(basic_block_t* blk, const std::map<std::string, IROper_t*> define_chain);
  std::map<_rhs_t, std::string> _expr_set;
  std::map<std::string, std::string> _rewrite_rule;
  void PropagateKilled(std::string killed_var);

};

struct IR_LiveRange_t
{
  int left, right;
  std::string  representative_var_name;
  std::string* defined_position;
  std::vector<std::string*> referenced_position;
  bool rewritable;
  bool is_front_edge;
  bool is_back_edge;
  void rewrite_reference(std::string rewrited);
  void rewrite_definition(std::string rewrited);
  std::set<std::pair<IR_LiveRange_t*, void*>> successor_LRs;
  // the [second] parameter should be [IR_LiveRangeSet_t]
  // Rewrite all reference of [representative_reg_name] in [referenced_position]
  // into [rewrited]
  IR_LiveRange_t(int left, int right, std::string var_name);
};

struct IR_LiveRangeSet_t
{
public:
  std::map<std::string, std::set<IR_LiveRange_t*>> LR_Array;
  std::map<std::string, IR_LiveRange_t*> Front_Edges;
  std::map<std::string, IR_LiveRange_t*> Back_Edges;
  // contains liverange for [#k], [^k] and [~k]
  IR_LiveRange_t* get_Reference_LiveRange(int position, std::string regname);
  // Check if A and B are interleaved.
  bool Check_Interleave(IR_LiveRange_t* A, IR_LiveRange_t* B);
  
public:
  IR_LiveRange_t* get_Defined_LiveRange(int position, std::string regname);
  // Get LR for [regname] at instruction [position]
  // Only valid for [Killed] vars, not for [Referenced] vars

  bool Check_Interleave(IR_LiveRange_t* lr, std::string regname);
  
  IR_LiveRangeSet_t();

  std::string Get_Non_Interleaved_Register(IROper_t& irop);
  // Get a [non-interleaved] register against [asmop]'s defined LiveRange
  // try to replace corrsepond register for better instruction scheduling.
  void Try_Rename_Register(IROper_t& irop);
  
  
};
void Propagate_LiveRanges(std::list<basic_block_t*>& blks);
void PostAlloc_Flow_Optimize(std::list<basic_block_t*>& blks);


#endif
