#include "flow_optimize.h"
extern int optimize_flag;
extern std::string input_filename;
extern std::set<std::string> local_function_table;
///////////////////////////////////////////////////////////
IR_LiveRange_t::IR_LiveRange_t(int left, int right, std::string var_name) :
  left(left), right(right),
  rewritable(true), representative_var_name(var_name),
  defined_position(NULL), referenced_position({}),
  is_front_edge(false), is_back_edge(false),
  successor_LRs({})
{
}

IR_LiveRangeSet_t::IR_LiveRangeSet_t() :
  LR_Array({})
{
}

void _construct_sing_lrset(basic_block_t& blk, IR_LiveRangeSet_t& lrset)
{
    std::map<std::string, IR_LiveRange_t* > current_LiveRange;
    
  for(auto it : blk.RegliveIn)
    {
      IR_LiveRange_t tmplr(0, 0, it);
      tmplr.is_front_edge = true;
      // The variables from liveIn SHOULDN'T been renamed
      current_LiveRange.insert({it, new IR_LiveRange_t(tmplr)});
      // These variables' [defined_position] is NULL
    }
     
  // Forward scan this block.
  for(auto p = blk.begin;
      p != blk.end;
      p++)
    {
      // Update current_LiveRange
      // If this instruction referenced some vars, update their LiveRange
      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_42);
      auto ref_var = Get_Referenced_Position(*(IROper_t*)*p);
      for(auto it_var : ref_var.second)
	{
	  auto var_name = it_var.first;
	  auto reg_position = it_var.second;
	  myAssert(current_LiveRange.count(var_name),ASSERT_EXIT_CODE_43);
	  myAssert(current_LiveRange[var_name]->representative_var_name == var_name,ASSERT_EXIT_CODE_44);
	  current_LiveRange[var_name]->rewritable &= ref_var.first;
	  current_LiveRange[var_name]->referenced_position.push_back(reg_position);
	  current_LiveRange[var_name]->right = ((IROper_t*)*p)->line_number;
	}
      // If this instruction killed some vars, eliminate their LiveRanges
      auto killed_var = Get_Killed_Position(*(IROper_t*)*p);
      for(auto it_var : killed_var.second)
	{
	  auto var_name = it_var.first;
	  auto reg_position = it_var.second;
	  if(current_LiveRange.count(var_name))
	    {
	      if(current_LiveRange[var_name]->left != current_LiveRange[var_name]->right)
		// Not a dead kill
		{
		  myAssert(current_LiveRange[var_name]->representative_var_name == var_name,ASSERT_EXIT_CODE_45);
		  if(!lrset.LR_Array.count(var_name))
		    lrset.LR_Array[var_name] = {};
		  lrset.LR_Array[var_name].insert(current_LiveRange[var_name]);
		}
	      else // Dead Kill should also be taken into consideration
		{
		  myAssert(current_LiveRange[var_name]->representative_var_name == var_name,ASSERT_EXIT_CODE_46);
		  if(!lrset.LR_Array.count(var_name))
		    lrset.LR_Array[var_name] = {};
		  lrset.LR_Array[var_name].insert(current_LiveRange[var_name]);
		}
	      // Refresh [var_name]'s liveRange
	      IR_LiveRange_t tmplr(((IROper_t*)*p)->line_number, ((IROper_t*)*p)->line_number, var_name);
	      tmplr.defined_position = reg_position;
	      tmplr.rewritable &= killed_var.first;
	      current_LiveRange[var_name] = new IR_LiveRange_t(tmplr);
	    }
	  else
	    {
	      // Refresh [var_name]'s liveRange
	      IR_LiveRange_t tmplr(((IROper_t*)*p)->line_number, ((IROper_t*)*p)->line_number, var_name);
	      tmplr.defined_position = reg_position;
	      tmplr.rewritable &= killed_var.first;
	      current_LiveRange[var_name] = new IR_LiveRange_t(tmplr);
	    }
	}
    }

  // Finalize LR Calculation
  for(auto it_var : blk.RegliveOut)
    {
      myAssert(current_LiveRange.count(it_var),ASSERT_EXIT_CODE_47);
      myAssert(current_LiveRange[it_var]->representative_var_name == it_var,ASSERT_EXIT_CODE_48);
      current_LiveRange[it_var]->right = INT32_MAX;
      current_LiveRange[it_var]->is_back_edge = true;
    }
  for(auto it_var : current_LiveRange)
    {
      // if(it_var.second->left != it_var.second->right)
	{
	  if(!lrset.LR_Array.count(it_var.first))
	    lrset.LR_Array.insert({it_var.first, {}});
	  lrset.LR_Array[it_var.first].insert(it_var.second);
	  if(it_var.second->is_front_edge)
	    {
	      myAssert(!lrset.Front_Edges.count(it_var.first),ASSERT_EXIT_CODE_49);
	      lrset.Front_Edges.insert({it_var.first, it_var.second});
	    }
	  if(it_var.second->is_back_edge)
	    {
	      myAssert(!lrset.Back_Edges.count(it_var.first),ASSERT_EXIT_CODE_50);
	      lrset.Back_Edges.insert({it_var.first, it_var.second});
	    }
	}
    }
  std::vector<std::string> regs = {"#0","#1","#2","#3","#4","#5","#6","#7","#8","#9","#10"};
  for(auto it : regs)
  {
    if(!lrset.LR_Array.count(it))
      lrset.LR_Array.insert({it,{}});
  }

  for(auto it1 : lrset.LR_Array)
    {
      for(auto it2 : it1.second)
	{
	  // printf("LiveRange for %s is [%d, %d]\n", it1.first.c_str(), it2->left, it2->right);
	}
    }

}

bool _diff_lr(basic_block_t* blk,
	      std::map<basic_block_t*, IR_LiveRangeSet_t*>& LRs)
{
  bool changed = false;
  myAssert(LRs.count(blk),ASSERT_EXIT_CODE_51);
  // If an LR is back_edge, insert all succ LRs into its successor set
  auto& lrset = *LRs[blk];

  for(auto it : lrset.Back_Edges)
    {
      auto var_name = it.first;
      std::set<std::pair<IR_LiveRange_t*, void*>> tmp_successor_LRs;
      IR_LiveRange_t* back_lr = it.second;
      for(auto it2 : blk->out_edges)
	{
	  myAssert(LRs.count(it2),ASSERT_EXIT_CODE_52);
	  auto& lrset2 = *LRs[it2];
	  if(lrset2.Front_Edges.count(var_name))
	    {
	      tmp_successor_LRs.insert({lrset2.Front_Edges[var_name], (void*)LRs[it2]});
	      it.second->rewritable &= lrset2.Front_Edges[var_name]->rewritable;
	    }
	}
      if(tmp_successor_LRs != it.second->successor_LRs)
	{
	  it.second->successor_LRs = tmp_successor_LRs;
	  changed = true;
	}
    }
  return changed;
}

void _propagate_lrs(std::list<basic_block_t*>& blks,
		    std::map<basic_block_t*, IR_LiveRangeSet_t*>& LRs)
{
  bool changed = true;
  while(changed)
    {
      changed = false;
      for(auto it : blks)
	{
	  changed |= _diff_lr(it, LRs);
	}
    }
}
void Propagate_LiveRanges(std::list<basic_block_t*>& blks)
{
  // 1. Construct LiveRanges for each block
  std::map<basic_block_t*, IR_LiveRangeSet_t*> LRs;
  for(auto it : blks)
    {
      IR_LiveRangeSet_t* lrs = new IR_LiveRangeSet_t;
      _construct_sing_lrset(*it, *lrs);
      LRs.insert({it, lrs});
    }
  // 2. Propagate Relations between different lrs
  _propagate_lrs(blks, LRs);
}

bool IR_LiveRangeSet_t::Check_Interleave(IR_LiveRange_t *A, IR_LiveRange_t *B)
{
  int& l1 = A->left;
  int& r1 = A->right;
  int& l2 = B->left;
  int& r2 = B->right;
  return (CHECK_CROSS(l1, r1, l2, r2));
}

bool IR_LiveRangeSet_t::Check_Interleave(IR_LiveRange_t *lr, std::string regname)
{
  myAssert(lr != NULL,ASSERT_EXIT_CODE_53);
  myAssert(lr->rewritable,ASSERT_EXIT_CODE_54);
  bool interleaved = false;
  for(auto range : this->LR_Array[regname])
    {
      if(Check_Interleave(lr, range))
	{
	  interleaved = true;
	  break;
	}
    }
  return interleaved;
}

// std::string IR_LiveRangeSet_t::Get_Non_Interleaved_Register(IROper_t &irop)
// {
//   auto kill_set = Get_Killed_Position(irop);
//   if(kill_set.first == false)
//     return {};
//   if(kill_set.second.size() != 1)
//     return {};
//   std::string var_name =kill_set.second.front().first;
//   IR_LiveRange_t* irlr = this->get_Defined_LiveRange(irop.line_number, var_name);
//   if(irlr == NULL) // Non-use interval
//     return "";
  
//   myAssert(irlr != NULL,ASSERT_EXIT_CODE_55);

//   if(!irlr->rewritable)
//     return "";
//   std::vector<std::string> st;
//   bool interleaved = false;

//   for(auto it : this->LR_Array)
//     {
//       // 1. Check local interfere
//       myAssert(it.first != "",ASSERT_EXIT_CODE_56);
//       interleaved |= Check_Interleave(irlr, it.first);
//       if(interleaved == true)
// 	continue;
//       // 2. Check successor interfere
//       if(irlr->is_back_edge == true)
// 	{
// 	  myAssert(this->Back_Edges.count(it.first),ASSERT_EXIT_CODE_57);
// 	  myAssert(this->Back_Edges[it.first] == irlr,ASSERT_EXIT_CODE_58);
// 	  for(auto it2 : irlr->successor_LRs)
// 	    {
	      
// 	    }
// 	}
//     }
//   if(!st.empty())
//     {
//       // printf("All available registers for:");
//       // asmop.print(std::cerr);
//       // for(auto it : st)
//       // 	{
//       // 	  std::cerr<<it<<" ";
//       // 	}
//       // std::cerr<<std::endl;
//       return st[rand() % st.size()];
//     }
//   return "";

// }

///////////////////////////////////////////////////////////
bool basic_block_t::Compute_AvailIn(const std::set<std::string> all_exprs)
{
  if(this->in_edges.size() == 0) // Entry Node
    return false; 
  // Do NOT change, still empty set
  
  std::set<std::string> newAvailIn = all_exprs;
  for(auto m : this->in_edges)
    {
      auto diff = mySetDifference(m->AvailIn, m->ExprKill);
      auto to_be_intersect = mySetUnion(m->DEExpr, diff);
      newAvailIn = mySetIntersect(newAvailIn, to_be_intersect);
    }
  if(newAvailIn == this->AvailIn)
    {
      return false; // Not modified
    }
  else
    {
      this->AvailIn = newAvailIn;
      return true; // Modified
    }
}

void basic_block_t::Finalize_AvailOut(const std::set<std::string> all_exprs)
{
  this->AvailOut = mySetUnion(this->DEExpr, mySetDifference(this->AvailIn, this->ExprKill));
}

bool basic_block_t::Compute_AntOut(const std::set<std::string> all_exprs)
{
  if(this->out_edges.size() == 0) // Exiting Node
    return false;
  // Do NOT change, still empty set

  std::set<std::string> newAntOut = all_exprs;
  for(auto m : this->out_edges)
    {
      auto diff = mySetDifference(m->AntOut, m->ExprKill);
      auto to_be_intersect = mySetUnion(m->UEExpr, diff);
      newAntOut = mySetIntersect(newAntOut, to_be_intersect);
    }
  if(newAntOut == this->AntOut)
    {
      return false; // Not modified
    }
  else
    {
      this->AntOut = newAntOut;
      return true; // Modified
    }
}

void basic_block_t::Finalize_AntIn(const std::set<std::string> all_exprs)
{
  this->AntIn = mySetUnion(this->UEExpr, mySetDifference(this->AntOut, this->ExprKill));  
}

void basic_block_t::Finalize_Earliest(const std::set<std::string> all_exprs)
{
  for(auto j : this->out_edges)
    {
      myAssert(!this->Earliest.count(j),ASSERT_EXIT_CODE_59);
      auto intersect1 = mySetDifference(j->AntIn, this->AvailOut);
      auto intersect2 = mySetUnion(this->ExprKill, mySetDifference(all_exprs, this->AntOut));      
      this->Earliest[j] = mySetIntersect(intersect1, intersect2);
    }
}

bool basic_block_t::Compute_LaterIn(const std::set<std::string> all_exprs)
{
  if(this->in_edges.size() == 0) // Entry Node
    return false;
  // Do NOT change, still empty set
  
  std::set<std::string> newLaterIn = all_exprs;
  for(auto i : this->in_edges)
    {
      myAssert(i->Earliest.count(this),ASSERT_EXIT_CODE_60);
      auto diff = mySetDifference(i->LaterIn, i->UEExpr);
      auto to_be_intersect = mySetUnion(i->Earliest[this], diff);
      newLaterIn = mySetIntersect(newLaterIn, to_be_intersect);
    }
  if(newLaterIn == this->LaterIn)
    {
      return false; // Not modified
    }
  else
    {
      this->LaterIn = newLaterIn;
      return true; // Modified
    }
}

void basic_block_t::Finalize_Later(const std::set<std::string> all_exprs)
{
  // [this] is [j]
  for(auto i : this->in_edges)
    {
      myAssert(i->Earliest.count(this),ASSERT_EXIT_CODE_61);
      myAssert(!i->Later.count(this),ASSERT_EXIT_CODE_62);
      auto diff = mySetDifference(i->LaterIn, i->UEExpr);
      i->Later[this] = mySetUnion(i->Earliest[this],
				  diff);
    }
}

void basic_block_t::Finalize_Insert(const std::set<std::string> all_exprs)
{
  // [this] is [j]
  for(auto i : this->in_edges)
    {
      myAssert(i->Later.count(this),ASSERT_EXIT_CODE_63);
      myAssert(!i->Insert.count(this),ASSERT_EXIT_CODE_64);
      auto diff = mySetDifference(i->Later[this], this->LaterIn);
      i->Insert[this] = diff;
    }  
}

void basic_block_t::Finalize_Delete(const std::set<std::string> all_exprs)
{
  if(this->in_edges.size() == 0) // Entry Node
    return;
  this->Delete = mySetDifference(this->UEExpr, this->LaterIn);
}

void basic_block_t::print_Insert(std::ostream &out)
{
  out<<"Insert for block "<<this->id<<":"<<std::endl;
  for(auto it : this->Insert)
    {
      out<<"Insert at edge "<<this->id<<" -> "<<it.first->id<<" : ";
      for(auto it2 : it.second)
	{
	  out<<it2<<" ";
	}
      out<<std::endl;
    }
}
void basic_block_t::print_Delete(std::ostream &out)
{
  out<<"Delete for block "<<this->id<<":"<<std::endl;
  for(auto it : this->Delete)
    {
      out<<it<<" ";
    }
  out<<std::endl;
}

////////////////////////////////////////////////////////
static bool is_immediate_operand(const IROper_t* op)
// consist only 
{
#define TEST_IMMEDIATE(oper)			\
  if(oper.type != OPERAND_IMM &&		\
     oper.type != OPERAND_NOT_DEFINED)		\
    return false;
  TEST_IMMEDIATE(op->ops1);
  TEST_IMMEDIATE(op->ops2);
  TEST_IMMEDIATE(op->ops3);
  return true;
}

std::set<OPERATION_TYPE> consideration_instrs =
  {
    OPER_ADD,
    OPER_SUB,
    OPER_MUL,
    OPER_DIV,
    OPER_SHL,
    OPER_SHR,
    OPER_SAR,
  };

bool check_scheduable(const IROper_t& irop)
{
  bool scheduable = true;
  if(!consideration_instrs.count(irop.oper))
    {
      scheduable = false;
    }
#define TEST_LOCALITY(oper)				\
  {							\
    if(oper.get_static_type() != STATIC_LOCAL_VAR &&	\
       oper.get_static_type() != STATIC_IMM &&		\
       oper.get_static_type() != STATIC_NONE)		\
      {							\
	scheduable = false;				\
      }							\
  }
  TEST_LOCALITY(irop.dest);
  TEST_LOCALITY(irop.ops1);
  TEST_LOCALITY(irop.ops2);
  TEST_LOCALITY(irop.ops3);
#undef TEST_LOCALITY

  if(is_immediate_operand(&irop))
    {
      scheduable = false;
    }

  // If a instruction is scheduable, check if it satisfy the NameSpace Rule
  if(scheduable)
  {
    myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_65);
    bool _test_local_var = false;
#define TEST_NAMESPACE(oper)						\
    if(oper.get_static_type() == STATIC_LOCAL_VAR)			\
      {									\
	_test_local_var = true;						\
	if(var_to_int(oper.name) >= var_to_int(irop.dest.name))		\
	  {								\
	    printf("Assertion Failed at:\n");				\
	    IROper_t copy = irop;					\
	    copy.print(std::cerr, -1);					\
	  }								\
	if(var_to_int(oper.name) >= var_to_int(irop.dest.name))		\
	  {								\
	    return false;						\
	  }								\
      }
    TEST_NAMESPACE(irop.ops1);
    TEST_NAMESPACE(irop.ops2);
    TEST_NAMESPACE(irop.ops3);
    myAssert(_test_local_var == true,ASSERT_EXIT_CODE_67);
  }
#undef TEST_NAMESPACE
	      
  return scheduable;
}

void ExpressionSet_t::InitExprSet(basic_block_t *blk, const std::map<std::string, IROper_t *> define_chain)
{
  this->_expr_set.clear();
  for(auto it : blk->AvailIn)
    {
      myAssert(define_chain.count(it),ASSERT_EXIT_CODE_68);
      _rhs_t rhs = define_chain.at(it)->get_rhs();
      if(this->_expr_set.count(rhs))
	{
	  // See which variable is defined early
	  auto r1 = this->_expr_set.at(rhs);
	  if(var_to_int(r1) > var_to_int(it))
	    {
	      this->_expr_set[rhs] = it;
	    }
	}
      else
	{
	  this->_expr_set.insert({rhs, it});
	}
    }
}

void ExpressionSet_t::PropagateKilled(std::string killed_var)
{
  auto p = this->_expr_set.begin();
  while(p != this->_expr_set.end())
    {
      bool need_to_kill = false;
#define TEST_KILL_NEEDED(oper)				\
      if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
	 oper.name == killed_var)			\
	{						\
	  need_to_kill = true;				\
	}
      TEST_KILL_NEEDED(p->first.ops1);
      TEST_KILL_NEEDED(p->first.ops2);
      TEST_KILL_NEEDED(p->first.ops3);
#undef TEST_KILL_NEEDED
      if(need_to_kill)
	{
	  p = this->_expr_set.erase(p);
	}
      else
	{
	  p++;
	}
    }
}

bool ExpressionSet_t::RewriteBlock(basic_block_t *blk, const std::map<std::string, IROper_t *> define_chain)
{
  bool changed = false;
  // Initialize local _expr_set
  this->InitExprSet(blk, define_chain);

  for(auto p = blk->begin;
      p != blk->end;
      ++p)
    {
      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_69);
      IROper_t& irop = *(IROper_t*)(*p);
      bool scheduable = check_scheduable(irop);
      // printf("Considering: ");
      // irop.print(std::cerr, -1);
      // First, try to rewrite its RHS
#define TEST_REWRITE(oper)						\
      if(oper.get_static_type() == STATIC_LOCAL_VAR &&			\
	 this->_rewrite_rule.count(oper.name))				\
	{								\
	  if(oper.name != this->_rewrite_rule[oper.name])		\
	    {								\
	      oper.name = this->_rewrite_rule[oper.name];		\
	      changed = true;						\
	    }								\
	}
      
      TEST_REWRITE(irop.ops1);
      TEST_REWRITE(irop.ops2);
      TEST_REWRITE(irop.ops3);
#undef TEST_REWRITE
      if(!scheduable)
	// If not scheduable, check its killed vars
	{
	  if(OPERATION_REGISTERS.count(irop.oper) &&
	     OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
	     irop.dest.get_static_type() == STATIC_LOCAL_VAR)
	    {
	      this->PropagateKilled(irop.dest.name);
	      
	    }
	  continue;
	}
      myAssert(scheduable,ASSERT_EXIT_CODE_70);
      // Then, try to find if it can be rewrite
      myAssert(OPERATION_REGISTERS.count(irop.oper) &&
	     OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
	       irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_71);

      if(this->_expr_set.count(irop.get_rhs()))
	{
	  myAssert(irop.dest.name != "" && irop.dest.name[0] == '%',ASSERT_EXIT_CODE_72);
	  myAssert(this->_expr_set[irop.get_rhs()] != "" && this->_expr_set[irop.get_rhs()][0] == '%',ASSERT_EXIT_CODE_73);
	  int date1 = std::stoi(irop.dest.name.substr(1));
	  int date2 = std::stoi(this->_expr_set[irop.get_rhs()].substr(1));
	  if(date1-date2<12)
	    this->_rewrite_rule[irop.dest.name] = this->_expr_set[irop.get_rhs()];
	  else
	    {
	      this->_expr_set.insert({irop.get_rhs(), irop.dest.name});
	      this->_rewrite_rule[irop.dest.name] = irop.dest.name;
	    }
	}
      else
	{
	  this->_expr_set.insert({irop.get_rhs(), irop.dest.name});
	  this->_rewrite_rule[irop.dest.name] = irop.dest.name;
	}
      // printf("Rewrite Rule: %s -> %s\n", irop.dest.name.c_str(), _rewrite_rule[irop.dest.name].c_str());

      // Finally, check its killed vars
      this->PropagateKilled(irop.dest.name);
    }
  return changed;
}

void Reduce_Phi_Moves(std::list<basic_block_t*>& blks)
{
  // Pass #1, Check each variable's reference count
  std::map<std::string, int> ref_count;
  
  for(auto it : blks)
    {
      for(auto p = it->begin;
	  p != it->end;
	  ++p)
	{
	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_74);
	  IROper_t& irop = *(IROper_t*)(*p);
	  myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_75);

#define COUNT_REFS(opern, i)					\
	  if(OPERATION_REGISTERS.at(irop.oper)[i] == 0		\
	     /* Is a Reference */				\
	     && opern.get_static_type() == STATIC_LOCAL_VAR)	\
	    {							\
	      auto liter = opern.name;				\
	      if(!ref_count.count(liter))			\
		ref_count[liter] = 1;				\
	      else						\
		ref_count[liter]++;				\
	    }
	  COUNT_REFS(irop.dest, 0);
	  COUNT_REFS(irop.ops1, 1);
	  COUNT_REFS(irop.ops2, 2);
	  COUNT_REFS(irop.ops3, 3);
#undef COUNT_REFS
	}
    }
  // Pass #1+, Write reference count to MUL instruction
  for(auto it : blks)
    {
      for(auto p = it->begin;
	  p != it->end;
	  ++p)
	{
	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_76);
	  IROper_t& irop = *(IROper_t*)(*p);
	  if(irop.oper == OPER_MUL && ref_count.count(irop.dest.name))
	    {
	      // printf("Ref Count for %s is %d at:", irop.dest.name.c_str(), ref_count.at(irop.dest.name));
	      // irop.print(std::cerr, -1);
	      irop.ref_count = ref_count.at(irop.dest.name);
	    }
	}
    }
  // Pass 2, Check Replace Rules
  std::map<std::string, std::string> replace_rule;
  std::map<std::string, int> target_position;
  for(auto it : blks)
    {
      for(auto p = it->begin;
	  p != it->end;
	  ++p)
	{
	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_77);
	  IROper_t& irop = *(IROper_t*)(*p);
	  if(irop.oper != OPER_PHI_MOV)
	    continue;
	  myAssert(irop.oper == OPER_PHI_MOV,ASSERT_EXIT_CODE_78);
	  if(irop.ops1.get_static_type() != STATIC_LOCAL_VAR)
	    {
	      continue;
	    }
	  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_79);
	  if(ref_count[irop.ops1.name] == 1)
	    {
	      myAssert(!replace_rule.count(irop.ops1.name),ASSERT_EXIT_CODE_80);
	      replace_rule.insert({irop.ops1.name, irop.dest.name});
	      target_position[irop.ops1.name] = irop.line_number;
	    }
	}
    }

  // Pass 3, Check if there occurrs any reference for target PHI vars
  // if exists, we can't rewrite to PHI var, because the PHI var will be referenced,
  // and cannot be destroyed.

  // Pass 3.1, check first define position
  std::map<std::string, int> define_position;
  {
    for(auto it : blks)
      {
	for(auto p = it->begin;
	    p != it->end;
	    ++p)
	  {
	    myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_81);
	    IROper_t& irop = *(IROper_t*)(*p);
	    myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_82);
	    if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
	       irop.dest.get_static_type() == STATIC_LOCAL_VAR &&
	       replace_rule.count(irop.dest.name))
	      {
		if(irop.out_current_loop)
		  {
		    replace_rule.erase(irop.dest.name);
		  }
		else
		  {
		    myAssert(target_position.count(irop.dest.name),ASSERT_EXIT_CODE_83);
		    if(!define_position.count(irop.dest.name))
		      {
			define_position[irop.dest.name] = irop.line_number;
		      }
		    else
		      {
			define_position[irop.dest.name] = std::min(irop.line_number,
								   define_position[irop.dest.name]);
		    
		      }
		  }
	      }
	  }
      }

    // pass 3.2, check if there are any reference for PHI vars, inside a replace variable's
    // live range
    std::set<std::string> un_replaceable;
    for(auto it : blks)
      {
	for(auto p = it->begin;
	    p != it->end;
	    ++p)
	  {
	    myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_84);
	    IROper_t& irop = *(IROper_t*)(*p);
	    myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_85);

	    for(auto replace_pair : replace_rule)
	      {
		std::string src = replace_pair.first;
		std::string tar = replace_pair.second;
		if(!define_position.count(src)) // [src] is not defined, but referred
		  {
		    continue;
		  }
		myAssert(define_position.count(src),ASSERT_EXIT_CODE_86);
		myAssert(target_position.count(src),ASSERT_EXIT_CODE_87);
		if(!
		   (define_position[src] < irop.line_number &&
		    irop.line_number     <= target_position[src])
		   )
		  {
		    continue;
		  }
		int i = 0;
		auto opern = irop.dest;
#define TEST_REPLACEABLE(opern, i)					\
		if(OPERATION_REGISTERS.at(irop.oper)[i] == 0		\
		   /* Is a Reference */					\
		   && opern.get_static_type() == STATIC_LOCAL_VAR)	\
		  {							\
		    auto liter = opern.name;				\
		    if(liter == tar)					\
		      {							\
			un_replaceable.insert(src);			\
		      }							\
		  }
		TEST_REPLACEABLE(irop.dest, 0);
		TEST_REPLACEABLE(irop.ops1, 1);
		TEST_REPLACEABLE(irop.ops2, 2);
		TEST_REPLACEABLE(irop.ops3, 3);
#undef TEST_REPLACEABLE
		
	      }
	  }
      }
       
    // Pass 3, Operate Replace
    for(auto it : blks)
      {
	for(auto p = it->begin;
	    p != it->end;
	    ++p)
	  {
	    myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_88);
	    IROper_t& irop = *(IROper_t*)(*p);
#define REPLACE(oper)						\
	    if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
	       replace_rule.count(oper.name) &&			\
	       !un_replaceable.count(oper.name))		\
	      {							\
		oper.name = replace_rule[oper.name];		\
	      }
	    REPLACE(irop.dest);
	    REPLACE(irop.ops1);
	    REPLACE(irop.ops2);
	    REPLACE(irop.ops3);
#undef REPLACE
	  }
      }
  }
 
}

void Flow_Optimize(std::list<basic_block_t*>& blks)

// [blks]'s instruction is held in the [irs] list
{
  // Pass 1. Lazy Code Motion, ref: Engineering A Compiler 2nd, Section 10.3.1
  // See [codegen.h] for more information
  bool changed = true;
  while(changed)
    {
      changed = false;
      // Pass 1.1, Find DEExpr, UEExpr and ExprKill
      for(auto it : blks)
	{
	  it->DEExpr = {};
	  it->UEExpr = {};
	  it->ExprKill = {};
	  // Pass 1.1.1, DEExpr
	  {
	    std::set<std::string> DEExpr_killed_vars;

	    auto p = it->end;
	    if(p == it->begin)
	      continue;
	    p--;
	    while(1)
	      {
		myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_89);
		IROper_t& irop = *(IROper_t*)(*p);

	    
		// First, consider if this instruction can be added into DEExpr
		bool scheduable = check_scheduable(irop);
	    
		if(!scheduable)
		  // Even if a instruction is not scheduable, it might killed some vars
		  {
		    // Check if it killed some vars
		    if(OPERATION_REGISTERS.count(irop.oper) &&
		       OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
		       irop.dest.get_static_type() == STATIC_LOCAL_VAR)
		      {
			// myAssert(!DEExpr_killed_vars.count(irop.dest.name),ASSERT_EXIT_CODE_90);
			DEExpr_killed_vars.insert(irop.dest.name);
		      }
		    if(p == it->begin)
		      break;
		    else
		      {
			p--;
			continue;
		      }
		  }
		myAssert(scheduable,ASSERT_EXIT_CODE_91);

		// If a instruction is scheduable, check if its Rvalue is defined
		bool rval_killed = false;
#define TEST_RVAL_KILLED(oper)						\
		if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
		   DEExpr_killed_vars.count(oper.name))			\
		  {							\
		    rval_killed = true;					\
		  }
		TEST_RVAL_KILLED(irop.ops1);
		TEST_RVAL_KILLED(irop.ops2);
		TEST_RVAL_KILLED(irop.ops3);
#undef TEST_RVAL_KILLED
		if(rval_killed == false)
		  // Fine!
		  {
		    it->DEExpr.insert(irop.dest.name);
		    // we can use the expression name to find its operation
		  }

		// Check its killed variable
		{
		  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_92);
		  myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_93);
		  myAssert(OPERATION_REGISTERS.at(irop.oper)[0] == 1,ASSERT_EXIT_CODE_94);
		  myAssert(!DEExpr_killed_vars.count(irop.dest.name),ASSERT_EXIT_CODE_95);
		  DEExpr_killed_vars.insert(irop.dest.name);
		}
	    
		// End of While
		if(p == it->begin)
		  break;
		else
		  {
		    p--;
		    continue;
		  }
	      }
	  }
	
	  // Pass 1.1.2, UEExpr
	  {
	    std::set<std::string> UEExpr_killed_vars;
	    auto p = it->begin;
	    while(p != it->end)
	      {
		myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_96);
		IROper_t& irop = *(IROper_t*)(*p);
	    
		// First, consider if this instruction can be added into UEExpr
		bool scheduable = check_scheduable(irop);

		if(!scheduable)
		  // Even if a instruction is not scheduable, it might killed some vars
		  {
		    // Check if it killed some vars
		    if(OPERATION_REGISTERS.count(irop.oper) &&
		       OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
		       irop.dest.get_static_type() == STATIC_LOCAL_VAR)
		      {
			// myAssert(!UEExpr_killed_vars.count(irop.dest.name),ASSERT_EXIT_CODE_97);
			UEExpr_killed_vars.insert(irop.dest.name);
		      }
		    if(p == it->end)
		      {
			myAssert(0,ASSERT_EXIT_CODE_98);
			// Segmentation Fault may occurs, and may not execute to this point.
		      }
		    else
		      {
			p++;
			continue;
		      }
		  }
		myAssert(scheduable,ASSERT_EXIT_CODE_99);

		// If a instruction is scheduable, check if its Rvalue is defined
		bool rval_killed = false;
#define TEST_RVAL_KILLED(oper)						\
		if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
		   UEExpr_killed_vars.count(oper.name))			\
		  {							\
		    rval_killed = true;					\
		  }
		TEST_RVAL_KILLED(irop.ops1);
		TEST_RVAL_KILLED(irop.ops2);
		TEST_RVAL_KILLED(irop.ops3);
#undef TEST_RVAL_KILLED
		if(rval_killed == false)
		  // Fine!
		  {
		    it->UEExpr.insert(irop.dest.name);
		    // we can use the expression name to find its operation
		  }
	    
		// Check its killed variable
		{
		  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_100);
		  myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_101);
		  myAssert(OPERATION_REGISTERS.at(irop.oper)[0] == 1,ASSERT_EXIT_CODE_102);
		  myAssert(!UEExpr_killed_vars.count(irop.dest.name),ASSERT_EXIT_CODE_103);
		  UEExpr_killed_vars.insert(irop.dest.name);
		}
	    
		// End of While
		if(p == it->end)
		  {
		    myAssert(0,ASSERT_EXIT_CODE_104);
		  }
		else
		  {
		    p++;
		    continue;
		  }
	      }
	  }

	  // Pass 1.1.3, ExprKill
	  {
	    // Pass 1.1.3.1, Find Killed Vars
	    std::set<std::string> Block_killed_vars;
	    for(auto p = it->begin;
		p != it->end;
		++p)
	      {
		myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_105);
		IROper_t& irop = *(IROper_t*)(*p);
		if(OPERATION_REGISTERS.count(irop.oper) &&
		   OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
		   irop.dest.get_static_type() == STATIC_LOCAL_VAR)
		  {
		    // myAssert(!Block_killed_vars.count(irop.dest.name),ASSERT_EXIT_CODE_106);
		    Block_killed_vars.insert(irop.dest.name);
		  }
	      }

	    // Pass 1.1.3.2, Find ExprKill Set
	    for(auto p = it->begin;
		p != it->end;
		++p)
	      {
		myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_107);
		IROper_t& irop = *(IROper_t*)(*p);

		bool scheduable = check_scheduable(irop);
		if(!scheduable)
		  continue;
		bool rval_killed = false;
#define TEST_RVAL_KILLED(oper)						\
		if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
		   Block_killed_vars.count(oper.name))			\
		  {							\
		    rval_killed = true;					\
		  }
		TEST_RVAL_KILLED(irop.ops1);
		TEST_RVAL_KILLED(irop.ops2);
		TEST_RVAL_KILLED(irop.ops3);
#undef TEST_RVAL_KILLED

		if(rval_killed == true)
		  {
		    it->ExprKill.insert(irop.dest.name);
		  }
	      }	  
 
	  }

	  {
	    // Debug
	    // printf("The DEExpr for Block %d is:\n", it->id);
	    // for(auto p : it->DEExpr)
	    //   {
	    //     std::cerr<<p<<" ";
	    //   }
	    // std::cerr<<std::endl;
	    // printf("The UEExpr for Block %d is:\n", it->id);
	    // for(auto p : it->UEExpr)
	    //   {
	    //     std::cerr<<p<<" ";
	    //   }
	    // std::cerr<<std::endl;
	    // printf("The ExprKill for Block %d is:\n", it->id);
	    // for(auto p : it->ExprKill)
	    //   {
	    //     std::cerr<<p<<" ";
	    //   }
	    // std::cerr<<std::endl;

	  }
	}

      // Pass 1.2, Collect All Expressions, for Initial Condition
      std::set<std::string> all_exprs;
      std::map<std::string, IROper_t*> lval_to_expr;
      for(auto it : blks)
	{
	  for(auto p = it->begin;
	      p != it->end;
	      ++p)
	    {
	      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_108);
	      IROper_t& irop = *(IROper_t*)(*p);
	      if(check_scheduable(irop))
		{
		  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_109);
		  myAssert(!all_exprs.count(irop.dest.name),ASSERT_EXIT_CODE_110);
		  all_exprs.insert(irop.dest.name);
		  myAssert(!lval_to_expr.count(irop.dest.name),ASSERT_EXIT_CODE_111);
		  lval_to_expr.insert({irop.dest.name, (IROper_t*)(*p)});
		}
	    }
	}
    
      // Pass 1.3, Initialize proper values for each set (and create a global start & end block)
      {
	// Initialize AvailIn, AntOut, LaterIn, for each block
	for(auto it : blks)
	  {
	    it->AvailOut = {};
	    it->AntIn = {};
	    it->Earliest = {};
	    it->Later = {};
	    it->Insert = {};
	    it->Delete = {};
	    // AvailIn
	    if(it->in_edges.empty())
	      {
		it->AvailIn = {};
	      }
	    else
	      {
		it->AvailIn = all_exprs;
	      }

	    // AntOut
	    if(it->out_edges.empty())
	      {
		it->AntOut = {};
	      }
	    else
	      {
		it->AntOut = all_exprs;
	      }

	    // LaterIn
	    if(it->in_edges.empty())
	      {
		it->LaterIn = {};
	      }
	    else
	      {
		it->LaterIn = all_exprs;
	      }
	  }

      }
      // Pass 1.4, Calculate AvailIn, AvailOut
      {
	bool modified = true;
	while(modified)
	  {
	    modified = false;

	    for(auto p = blks.begin();
		p != blks.end();
		++p)
	      {
		modified |= (*p)->Compute_AvailIn(all_exprs);
	      }
	  }

	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->Finalize_AvailOut(all_exprs);
	  }
      }

      // Pass 1.5, Calculate AntIn, AntOut
      {
	bool modified = true;
	while(modified)
	  {
	    modified = false;

	    for(auto p = blks.begin();
		p != blks.end();
		++p)
	      {
		modified |= (*p)->Compute_AntOut(all_exprs);
	      }
	  }

	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->Finalize_AntIn(all_exprs);
	  }      
      }

      // Pass 1.6, Calculate Earliest, LaterIn and Later
      {
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->Finalize_Earliest(all_exprs);
	  }      
	bool modified = true;
	while(modified)
	  {
	    modified = false;

	    for(auto p = blks.begin();
		p != blks.end();
		++p)
	      {
		modified |= (*p)->Compute_LaterIn(all_exprs);
	      }
	  }

	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->Finalize_Later(all_exprs);
	  }       
      }

      // Pass 1.7, Calculate Insert and Delete
      {
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->Finalize_Insert(all_exprs);
	    (*p)->Finalize_Delete(all_exprs);
	  }        
      }

    
      // Pass 2, Eliminate Common Subexpressions
      // (need to use lval_to_expr)
      // Pass 2.1, Eliminate
	{
	  for(auto it : blks)
	    {
	      ExpressionSet_t ExprSet;
	      changed |= ExprSet.RewriteBlock(it, lval_to_expr);
	    }
	}
      // Pass 2.2, Redo LiveVariable Analysis
      // Pass 2.2.1 Find [UEVar] and [VarKill] for each block
      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  (*p)->get_register_dependency();
	  (*p)->liveOut.clear();
	}
    
      // Pass 2.2.2 Find [LiveOut] for each block
      // LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

      bool modified = true;
      while(modified)
	{
	  modified = false;

	  for(auto p = blks.begin();
	      p != blks.end();
	      ++p)
	    {
	      modified |= (*p)->compute_liveout();
	    }
	}

    }    

  { // Pass 3, Optimize PHI Moves
    Reduce_Phi_Moves(blks);
    // Pass 3.2, Redo LiveVariable Analysis
    // Pass 3.2.1 Find [UEVar] and [VarKill] for each block
    for(auto p = blks.begin();
	p != blks.end();
	++p)
      {
	(*p)->get_register_dependency();
	(*p)->liveOut.clear();
      }
    
    // Pass 3.2.2 Find [LiveOut] for each block
    // LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

    bool modified = true;
    while(modified)
      {
	modified = false;

	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    modified |= (*p)->compute_liveout();
	  }
	}
  }

  // Pass 2, Eliminate Not-Referenced Assignments
  {
      
    bool changed = true;
    int cnt = 0;
    while(changed)
      {
	cnt++;
	changed = false;

	// Pass 2.2, Redo LiveVariable Analysis
	// Pass 2.2.1 Find [UEVar] and [VarKill] for each block
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    (*p)->get_register_dependency();
	    (*p)->liveOut.clear();
	  }
        
	// Pass 2.2.2 Find [LiveOut] for each block
	// LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

	bool modified = true;
	while(modified)
	  {
	    modified = false;

	    for(auto p = blks.begin();
		p != blks.end();
		++p)
	      {
		modified |= (*p)->compute_liveout();
	      }
	  }
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    std::set<std::string> liveNow = (*p)->liveOut;
	    auto q = (*p)->end;
	    if(q == (*p)->begin)
	      // Empty Block
	      {
		continue;
	      }

	    q--; // Safe
	    while(true)
	      {
		myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_88);
		auto& irop = *(IROper_t*)(*q);
		myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_112);
		if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
		   irop.dest.get_static_type() == STATIC_LOCAL_VAR &&
		   !liveNow.count(irop.dest.name))
		  { 
		    if(irop.oper == OPER_MOVEQ ||
		       irop.oper == OPER_MOVNE ||
		       irop.oper == OPER_MOVLT ||
		       irop.oper == OPER_MOVGT ||
		       irop.oper == OPER_MOVLE ||
		       irop.oper == OPER_MOVGE ||
		       irop.oper == OPER_SHRGE ||
		       irop.oper == OPER_SHRLT ||
		       irop.oper == OPER_ADDLT ||
		       irop.oper == OPER_SARGE ||
		       irop.oper == OPER_SARLT
		       )
		      {
			// do nothing
		      }
		    else
		      {
			// If it's CALL Operation, might have side effects
			if(irop.oper == OPER_CALL)
			  {
			    if(local_function_table.count(irop.ops1.name))
			      {
			    	irop.valid = 0;
			    	changed = true;
			      }
			    else
			      {
				irop.oper = OPER_CALL_NRET;
				irop.dest = irop.ops1;
				irop.ops1 = operand_t();
			      }
			  }
			else 
			  {
			    irop.valid = 0;
			    changed = true;
			  }
		      }
		  }
		// Remove Define from LiveNow

		if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
		   irop.dest.type == OPERAND_VAR &&
		   liveNow.count(irop.dest.name))
		  liveNow.erase(irop.dest.name);
	    
		// Insert Reference into LiveNow
#define INSERT_TO_LIVENOW(ops)			\
		if(ops.type == OPERAND_VAR)	\
		  {				\
		    liveNow.insert(ops.name);	\
		  }
	    
		// [dest] might be a reference too
		if(OPERATION_REGISTERS.at(irop.oper)[0] == 0 &&
		   irop.dest.type == OPERAND_VAR)
		  liveNow.insert(irop.dest.name);
	    
		INSERT_TO_LIVENOW(irop.ops1);
		INSERT_TO_LIVENOW(irop.ops2);
		INSERT_TO_LIVENOW(irop.ops3);
#undef INSERT_TO_LIVENOW

		if(q == (*p)->begin)
		  break;
		else
		  q--;
	      }
	  }

	// Refresh, delete invalid assignments.
	for(auto p = blks.begin();
	    p != blks.end();
	    ++p)
	  {
	    std::list<IR_t*>* tmpList = new std::list<IR_t*>;
	    for(auto q = (*p)->begin;
		q != (*p)->end;
		q++)
	      {
		myAssert(typeid(*(IROper_t*)*q) == typeid(IROper_t),ASSERT_EXIT_CODE_113);
		IROper_t& irop = *(IROper_t*)(*q);
		if(irop.valid != 0)
		  tmpList->push_back((IR_t*)&irop);
	      }
	    (*p)->begin = tmpList->begin();
	    (*p)->end = tmpList->end();
	
	  }
      }

  }
}

void Recompute_PostAlloc_LiveIn_LiveOut(std::list<basic_block_t*>& blks)
{
  // Pass #1 Find [UEVar] and [VarKill] for each block
  for(auto p = blks.begin();
      p != blks.end();
      ++p)
    {
      (*p)->get_postalloc_reg_dependency();
      (*p)->RegliveOut.clear();
    }
    
  // Pass #2 Find [LiveOut] for each block
  // LiveIn[Blk] = UEVar[Blk] UNION (LiveOut[Blk] INTERSECT NEG(VarKill[Blk]) )

  bool modified = true;
  while(modified)
    {
      modified = false;

      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  modified |= (*p)->compute_Regliveout();
	}
    }
  for(auto it : blks)
    {
      it->Finalize_RegliveIn();
    }
}

void PostAlloc_Flow_Optimize(std::list<basic_block_t*>& blks)
{
  // Pass 1, Redo Live Analysis
  {
    Recompute_PostAlloc_LiveIn_LiveOut(blks);
  }

  // Pass 2, Build LiveRanges
  {
    // 2.1, Set Line Number
    int linum = 1;
    for(auto it : blks)
      {
	for(auto p = it->begin;
	    p != it->end;
	    ++p)
	  {
	    myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_114);
	    IROper_t& irop = *(IROper_t*)(*p);
	    irop.line_number = linum++;
	  }
      }

    // 2.2 Propagate LiveRanges
    Propagate_LiveRanges(blks);
    
  }
  // for(auto it : blks)
  //   {
  //     it->print(std::cerr);
  //   }
}
