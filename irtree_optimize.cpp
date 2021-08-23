#include "irtree_optimize.h"

std::set<std::string> local_function_table;
std::set<std::string> phi_vars;
// Also used by other source files
/////////////////////////////////////////////
//////U T I L I T I E S//////////////////////
/////////////////////////////////////////////


int _eval_const_cmp(IROper_t& irop)
{
  myAssert(irop.oper == OPER_CMP,ASSERT_EXIT_CODE_129);
  myAssert(irop.dest.get_static_type() == STATIC_IMM,ASSERT_EXIT_CODE_130);
  myAssert(irop.ops1.get_static_type() == STATIC_IMM,ASSERT_EXIT_CODE_131);
  return irop.dest.imm - irop.ops1.imm;
}

int extract_number(std::string liter)
{
  auto p = liter.begin();
  while(!isdigit(*p) && p != liter.end())
    {
      p++;
    }
  if(p == liter.end())
    return -1; // No number is founded
  
  int res = 0;
  while(p != liter.end() && isdigit(*p))
    {
      res = res*10+(*p - '0');
      p++;
    }
  return res;
}

std::string replace_number(std::string liter,
			   int src,
			   int dest)
{
  myAssert(extract_number(liter) == src,ASSERT_EXIT_CODE_132);
  auto p = liter.begin();
  while(!isdigit(*p) && p != liter.end())
    {
      p++;
    }
  myAssert(p != liter.end(),ASSERT_EXIT_CODE_133);
  auto q = p;
  while(q != liter.end() && isdigit(*q))
    q++;
  std::string replaced = liter.replace(p, q, std::to_string(dest));
  return replaced;
}

std::set<std::string> IR_t::get_phi_vars()
{
  if(typeid(*this) == typeid(IR_t))
    myAssert(0,ASSERT_EXIT_CODE_134);
  return {};
}


std::set<std::string> IRList_t::get_phi_vars()
{
  std::set<std::string> res = {};
  for(auto it : this->body)
    {
      if(it != NULL)
	res = mySetUnion(res, it->get_phi_vars());
    }
  this->phi_vars = res;
  return res;
}

std::set<std::string> IROper_t::get_phi_vars()
{
  auto& irop = *this;
  if(irop.oper == OPER_PHI_MOV)
    {
      myAssert(irop.dest.type == OPERAND_VAR,ASSERT_EXIT_CODE_135);
      myAssert(irop.dest.name[0] == '%',ASSERT_EXIT_CODE_136);
      return {irop.dest.name};
    }
  return {};
}

int IR_t::set_linenum(int linenum)
{
  if(typeid(*this) == typeid(IR_t))
    myAssert(0,ASSERT_EXIT_CODE_137);
  if(typeid(*this) == typeid(IRList_t))
    myAssert(0,ASSERT_EXIT_CODE_138);
  return linenum;
}

int IRList_t::set_linenum(int linenum)
{
  for(auto it : this->body)
    {
      if(it == NULL)
	continue;
      linenum = it->set_linenum(linenum);
    }
  return linenum;
}

int IROper_t::set_linenum(int linenum)
{
  this->line_number = linenum;
  return linenum+1;
}

void IR_t::rewrite_varname(const std::map<std::string, std::string>& rules)
{
  if(typeid(*this) == typeid(IR_t))
    myAssert(0,ASSERT_EXIT_CODE_139);
  return;
}

void IRList_t::rewrite_varname(const std::map<std::string, std::string>& rules)
{
  for(auto it : this->body)
    {
      if(it == NULL)
	continue;
      it->rewrite_varname(rules);
    }
}

void IROper_t::rewrite_varname(const std::map<std::string, std::string> &rules)
{
  // printf("Rewriting consideration for");
  // this->print(std::cout, -1);
  // for(auto it : rules)
  //   {
  //     printf("%s -> %s\n", it.first.c_str(),
  // 	     it.second.c_str());
  //   }
  // test [dest]
  if(rules.count(this->dest.name) &&
     OPERATION_REGISTERS.count(this->oper) &&
     OPERATION_REGISTERS.at(this->oper)[0] == 1)     
    {
      this->valid = 0;
      printf("Drop ");
      this->print(std::cout, -1);
      printf("due to %s -> %s\n\n",
	     this->dest.name.c_str(), rules.at(this->dest.name).c_str());		     
      // No need anymore, we need to delete from IRTree
      return;
    }
  // test other opers

#define TEST_OPER(oper)						\
  if(rules.count(oper.name))					\
    {								\
      printf("Rewriting %s -> %s\n At",				\
	     oper.name.c_str(), rules.at(oper.name).c_str());	\
      this->print(std::cout, -1);				\
      printf("\n\n");						\
      myAssert(oper.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_140);	\
      oper.name = rules.at(oper.name);				\
    }
  TEST_OPER(this->ops1);
  TEST_OPER(this->ops2);
  TEST_OPER(this->ops3);
  return;
}

void IR_t::refresh_validation()
{
  // if(typeid(*this) == typeid(IR_t))
  //   myAssert(0,ASSERT_EXIT_CODE_141);
}

void IRList_t::refresh_validation()
{
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      if((*p) == NULL)
	continue;
      if(typeid(* *(p)) == typeid(IROper_t) &&
      	 ((IROper_t*)(*p))->valid == 0)
      	{
      	  (*p) = NULL;
	  continue;
      	}
      if(typeid(* *(p)) == typeid(IRList_t))
      	{
      	  ((IRList_t*)(*p))->refresh_validation();
	  continue;
      	}
    }

}
bool cmp(IROper_t* op1, IROper_t* op2)
{
  // 
  return ((*op1)<(*op2));
}

bool  is_immediate_operand(IROper_t* op)
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


bool shape_equal(IROper_t* op1, IROper_t* op2)
// [dest] might differ
{
  return op1->oper == op2->oper &&
    op1->ops1 == op2->ops1 &&
    op1->ops2 == op2->ops2 &&
    op1->ops3 == op2->ops3 &&
    !is_immediate_operand(op1);
}

class RewritingGraph
{
public:
  std::map<std::string, std::vector<std::string> > Edge;
  void AddEdge(std::string u, std::string v)
  {
    Edge[u].push_back(v);
  }
  std::string FindRwTarget(std::string s) const
  {
    myAssert(Edge.count(s),ASSERT_EXIT_CODE_142);
    printf("FindRwTarget %s", s.c_str());
    while(Edge.count(s))
      {
	myAssert(!Edge.at(s).empty(),ASSERT_EXIT_CODE_143);
	s = Edge.at(s).front();
	printf(" -> %s", s.c_str());
      }
    printf("\n");
    return s;
  }

} RG;


void IRTree_Optimize(IR_t* root)
{
  
}



bool Check_Local_Function(std::list<IR_t*>::iterator begin,
			  std::list<IR_t*>::iterator end)
{
  for(auto p = begin;
      p != end;
      ++p)
    {
      myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_144);
      IROper_t& irop = *(IROper_t*)(*p);
      // Shouldn't refer to something
#define CHECK_LOCALITY(oper)			\
      {						\
	auto type = oper.get_static_type();	\
	if(type == STATIC_GLOBL_ARR ||		\
	   type == STATIC_GLOBL_VAR ||		\
	   type == STATIC_REFER_ARR)		\
	  return false;				\
      }
      CHECK_LOCALITY(irop.dest);
      CHECK_LOCALITY(irop.ops1);
      CHECK_LOCALITY(irop.ops2);
      CHECK_LOCALITY(irop.ops3);
#undef CHECK_LOCALITY
      // Shouldn't call any function
      if(irop.oper == OPER_CALL || irop.oper == OPER_CALL_NRET)
	return false;
    }
  return true;
}

void Reduce_Redundant_Function_Call(std::list<IR_t*>::iterator begin,
				    std::list<IR_t*>::iterator end,
				    const std::set<std::string>& local_function_table)
{
  std::map<std::pair<std::string, std::vector<operand_t> >, std::string> call_trace;
  for(auto p = begin;
      p != end;
      ++p)
    {
      myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_145);
      IROper_t& irop = *(IROper_t*)(*p);
      if(irop.oper == OPER_CALL)
	{
	  std::pair<std::string, std::vector<operand_t> > calling_body;
	  myAssert(irop.ops1.get_static_type() == STATIC_OTHERS,ASSERT_EXIT_CODE_146);

	  if(!local_function_table.count(irop.ops1.name))
	    {
	      continue;
	    }

	  calling_body.first = irop.ops1.name;
	  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_147);
	  std::string calling_retn = irop.dest.name;
	  auto q = std::prev(p);
	  while(1)
	    {
	      IROper_t& sub_irop = *(IROper_t*)(*q);
	      if(sub_irop.oper != OPER_SET_ARG)
		break;
	      calling_body.second.push_back(sub_irop.ops1);
	      q--;
	    }
	  if(call_trace.count(calling_body))
	    // Replace happens
	    {
	      // myAssert(0,ASSERT_EXIT_CODE_148); // Manual CountCheck
	      auto q = std::prev(p);
	      while(1)
		{
		  IROper_t& sub_irop = *(IROper_t*)(*q);
		  if(sub_irop.oper != OPER_SET_ARG)
		    break;
		  sub_irop.valid = 0;
		  q--;
		}
	      irop.oper = OPER_MOVE;
	      irop.ops1 = operand_t(call_trace[calling_body]);
	    }
	  else
	    {
	      call_trace.insert({calling_body, calling_retn});
	    }
	}
    }
}
int _add(int a, int b)
{ return a+b; }
int _sub(int a, int b)
{ return a-b; }
int _mul(int a, int b)
{ return a*b; }
int _div(int a, int b)
{ return a/b; }
int _shl(int a, int b)
{ return a<<b; }
int _shr(int a, int b)
{ return (int)((unsigned int)(a)>>b); }
int _sar(int a, int b)
{ return (int)(a)>>b; }
IROper_t _add_sub_propagation(IROper_t irop)
{
  myAssert(irop.ops2.get_static_type() == STATIC_IMM,ASSERT_EXIT_CODE_149);
  if(irop.ops2.imm == 0)
    {
      return IROper_t(OPER_MOVE, irop.dest, irop.ops1);
    }
  return irop;
}

IROper_t _mul_propagation(IROper_t irop)
{ myAssert(irop.ops2.get_static_type() == STATIC_IMM,ASSERT_EXIT_CODE_150);
  if(irop.ops2.imm == 0)
    {
      return IROper_t(OPER_MOVE, irop.dest, 0);
    }
  if(IS_POWER_OF_2(irop.ops2.imm))
    {
      if(0  <= get_low(irop.ops2.imm) &&				
	 32 >  get_low(irop.ops2.imm)				
	 )							
	{								
	  return IROper_t(OPER_SHL,			
			  irop.dest,			
			  irop.ops1,			
			  get_low(irop.ops2.imm)	
			  );
	}								
      else							
	{
	  return IROper_t(OPER_MOVE,		
			  irop.dest,			
			  0				
			  );				
	}										    
    }
  return irop;
}

IROper_t _oth_propagation(IROper_t irop)
{ myAssert(irop.ops2.get_static_type() == STATIC_IMM,ASSERT_EXIT_CODE_151);
  return irop;
}


bool eval_ir(std::list<IR_t*>::iterator current,
	     std::map<std::string, int>& const_var_table)
// constant transformation
{
  ///////////////////////////////////////
  // Part 1, Constant Propagation ///////
  ///////////////////////////////////////
  myAssert(typeid(*(*current)) == typeid(IROper_t),ASSERT_EXIT_CODE_152);
  IROper_t& ir = *(IROper_t*)(*current);

  bool transformed = false;
  // Assert the [dest] hasn't be assigned to a const
  myAssert(OPERATION_REGISTERS.count(ir.oper),ASSERT_EXIT_CODE_153);
  if(OPERATION_REGISTERS.at(ir.oper)[0] == 1 &&
     ir.dest.get_static_type() == STATIC_LOCAL_VAR)
    {
      myAssert(!const_var_table.count(ir.dest.name),ASSERT_EXIT_CODE_154);
    }
  
  if(OPERATION_REGISTERS.at(ir.oper)[0] == 0 && 
     ir.dest.get_static_type() == STATIC_LOCAL_VAR)
    {
      if(const_var_table.count(ir.dest.name))
	{
	  ir.dest.imm = const_var_table[ir.dest.name];		
	  ir.dest.type = OPERAND_IMM;				
	  ir.dest.name = "";					
	  transformed = true;
	}
    }


#define CHECK_REWRITE(oper)				\
  if(oper.get_static_type() == STATIC_LOCAL_VAR &&	\
     const_var_table.count(oper.name))			\
    {							\
      oper.imm = const_var_table[oper.name];		\
      oper.type = OPERAND_IMM;				\
      oper.name = "";					\
      transformed = true;				\
    }
  CHECK_REWRITE(ir.ops1);
  CHECK_REWRITE(ir.ops2);
  CHECK_REWRITE(ir.ops3);
#undef CHECK_REWRITE

  ///////////////////////////////////////
  // Part 2, Strength Weakening /////////
  ///////////////////////////////////////
  switch(ir.oper)
    {
    case OPER_CMP:
      {
	break;
      }
    case OPER_MOVE:
      {
	if(ir.ops1.get_static_type() == STATIC_IMM &&
	   ir.dest.get_static_type() == STATIC_LOCAL_VAR)
	  {
	    myAssert(!const_var_table.count(ir.dest.name),ASSERT_EXIT_CODE_155);
	    const_var_table[ir.dest.name] = ir.ops1.imm;
	  }
	break;
      }
    case OPER_ADD:
    case OPER_SUB:
    case OPER_MUL:
    case OPER_DIV:
    case OPER_SHL:
    case OPER_SHR:
    case OPER_SAR:
      {

	const std::map<OPERATION_TYPE, int(*)(int,int)> eval_target =
	  {
	    { OPER_ADD, _add},
	    { OPER_SUB, _sub},
	    { OPER_MUL, _mul},
	    { OPER_DIV, _div},
	    { OPER_SHL, _shl},
	    { OPER_SHR, _shr},
	    { OPER_SAR, _sar},
	  };
	if(ir.ops1.get_static_type() == STATIC_IMM &&
	   ir.ops2.get_static_type() == STATIC_IMM)
	  {
	    myAssert(eval_target.count(ir.oper),ASSERT_EXIT_CODE_156);
	    int dest_imm = eval_target.at(ir.oper)(ir.ops1.imm, ir.ops2.imm);
	    const_var_table[ir.dest.name] = dest_imm;
	    ir = IROper_t(OPER_MOVE, ir.dest, dest_imm);
	    transformed = true;
	    break;
	  }

	// If Cannot eval directly, try to weaken the operation
	// first, try to swap the operands if possible
	if(ir.oper == OPER_ADD || ir.oper == OPER_MUL)
	  {
	    if(ir.ops1.get_static_type() == STATIC_IMM)
	      {
		std::swap(ir.ops1, ir.ops2);
	      }
	  }
	if(ir.ops2.get_static_type() != STATIC_IMM)
	  break;
	
	const std::map<OPERATION_TYPE, IROper_t(*)(IROper_t)> weaken_target =
	  {
	    // ADD
	    { OPER_ADD, _add_sub_propagation},
	    // SUB
	    { OPER_SUB, _add_sub_propagation},
	    // MUL
	    { OPER_MUL, _mul_propagation},
	    // DIV
	    { OPER_DIV, _oth_propagation},
	    // SHL
	    { OPER_SHL, _oth_propagation},
	    // SHR
	    { OPER_SHR, _oth_propagation},
	    // SAR
	    { OPER_SAR, _oth_propagation},
	  };
	
	myAssert(weaken_target.count(ir.oper),ASSERT_EXIT_CODE_157);
	
	if(ir != weaken_target.at(ir.oper)(ir))
	  {

	    transformed = true;
	    ir = weaken_target.at(ir.oper)(ir);
	  }
	break;
      }

 
 
    case OPER_SPACE:
    case OPER_WORD:
      {
	myAssert(0,ASSERT_EXIT_CODE_158);
      }
    case OPER_ADD_OFFSET:
    case OPER_SET_ARG:
    case OPER_CALL:
    case OPER_CALL_NRET:
    case OPER_LOAD_ARG:
    case OPER_STACK_ALLOC:
    case OPER_LOAD:
    case OPER_STORE:
    case OPER_FUNC:
    case OPER_ENDF:
    case OPER_RETN:
    case OPER_RET:
    case OPER_JMP:
    case OPER_JEQ:
    case OPER_JNE:
    case OPER_JGT:
    case OPER_JLT:
    case OPER_JGE:
    case OPER_JLE:
    case OPER_NOP:
    case OPER_LABEL:
    case OPER_PHI_MOV:
    case OPER_MOVEQ:
    case OPER_MOVNE:
    case OPER_MOVGT:
    case OPER_MOVLT:
    case OPER_MOVGE:
    case OPER_MOVLE:
    case OPER_ADDLT:
    case OPER_SHRGE:
    case OPER_SHRLT:
    case OPER_SARGE:
    case OPER_SARLT:
      {
	break;
      }
    }
  
  return transformed;
}

void Constant_Propagation(std::list<IR_t*>::iterator begin,
			  std::list<IR_t*>::iterator end)
{
  
  bool changed = true;

  int cnt = 0;
  while(changed)
    {
      std::map<std::string, int> const_var_table;
      changed = false;
      for(auto p = begin;
	  p != end;
	  ++p)
	{
	  changed |= eval_ir(p, const_var_table);
	}
      cnt++;
    }
}

std::string Initialize_Calling_Dependency(std::list<IR_t*>::iterator begin,
					  std::list<IR_t*>::iterator end,
					  std::map<std::string, std::set<std::string> >& calling_dependency)
{
  std::string function_name;
  for(auto p = begin;
      p != end;
      ++p)
    {
      myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_159);
      IROper_t& irop = *(IROper_t*)(*p);
      switch(irop.oper)
	{
	case OPER_FUNC:
	  {
	    myAssert(irop.dest.name != "",ASSERT_EXIT_CODE_160);
	    function_name = irop.dest.name;
	    if(!calling_dependency.count(function_name))
	      calling_dependency.insert({function_name, {}});
	    break;
	  }
	case OPER_CALL:
	  {
	    myAssert(function_name != "",ASSERT_EXIT_CODE_161);
	    if(!calling_dependency.count(function_name))
	      calling_dependency.insert({function_name, {}});
	    myAssert(irop.ops1.name != "",ASSERT_EXIT_CODE_162);
	    calling_dependency[function_name].insert(irop.ops1.name);
	    break;
	  }
	case OPER_CALL_NRET:
	  {
	    myAssert(function_name != "",ASSERT_EXIT_CODE_163);
	    if(!calling_dependency.count(function_name))
	      calling_dependency.insert({function_name, {}});
	    myAssert(irop.dest.name != "",ASSERT_EXIT_CODE_164);
	    calling_dependency[function_name].insert(irop.dest.name);
	    break;
	    
	  }
	default:
	  {
	    break;
	  }
	}
    }
  myAssert(function_name != "",ASSERT_EXIT_CODE_165);
  return function_name;
}

IROper_t _Rename_Local_Vars(IROper_t irop,
			    std::map<int, int>& number_reference,
			    int& current_id)
{
  switch(irop.oper)
    {
    case OPER_LABEL:
    case OPER_JMP:
    case OPER_JEQ:
    case OPER_JNE:
    case OPER_JGT:
    case OPER_JLT:
    case OPER_JGE:
    case OPER_JLE:
      {
	int num = extract_number(irop.dest.name);
	myAssert(num != -1,ASSERT_EXIT_CODE_166);
      _rename_local_vars_beg:
	if(number_reference.count(num))
	  {
	    irop.dest.name = replace_number(irop.dest.name, num, number_reference.at(num));
	  }
	else
	  {
	    int newnum = current_id++;
	    number_reference.insert({num, newnum});
	    goto _rename_local_vars_beg;

	  }
      }
    default:
      {
#define TEST_RENAME(oper)						\
	if(oper.get_static_type() == STATIC_LOCAL_VAR ||		\
	   oper.get_static_type() == STATIC_LOCAL_ARR ||		\
	   oper.get_static_type() == STATIC_REFER_ARR)			\
	  {								\
	    myAssert(oper.name != "",ASSERT_EXIT_CODE_167);					\
	  int num = extract_number(oper.name);				\
	  myAssert(num != -1,ASSERT_EXIT_CODE_168);						\
	  if(number_reference.count(num))				\
	    {								\
	      oper.name = replace_number(oper.name, num, number_reference.at(num)); \
	    }								\
	  else								\
	    {								\
	      int newnum = current_id++;				\
	      number_reference.insert({num, newnum});			\
	      myAssert(number_reference.count(num),ASSERT_EXIT_CODE_169);			\
	      oper.name = replace_number(oper.name, num, number_reference.at(num)); \
	    }								\
	  }

	TEST_RENAME(irop.dest);
	TEST_RENAME(irop.ops1);
	TEST_RENAME(irop.ops2);
	TEST_RENAME(irop.ops3);
#undef TEST_RENAME
      }
    }
  return irop;
}


void Inline_Expand(std::list<IR_t*>& LinearIR,
		  std::list<IR_t*>::iterator expansion_point,
		  std::pair<std::list<IR_t*>::iterator,
		  std::list<IR_t*>::iterator> function_range,
		  int& current_id)
{
  // printf("Inline Expansion at:");
  // (*expansion_point)->print(std::cerr, -1);
  myAssert(typeid(*(IROper_t*)*function_range.first) == typeid(IROper_t),ASSERT_EXIT_CODE_170);
  myAssert(typeid(*(IROper_t*)*expansion_point) == typeid(IROper_t),ASSERT_EXIT_CODE_171);
  myAssert(((IROper_t*)*function_range.first)->oper == OPER_FUNC,ASSERT_EXIT_CODE_172);
  if(((IROper_t*)*expansion_point)->oper == OPER_CALL)
    {
      myAssert(((IROper_t*)*function_range.first)->dest.name ==
	       ((IROper_t*)*expansion_point)->ops1.name,ASSERT_EXIT_CODE_173);


      std::string retn_target = ((IROper_t*)*expansion_point)->dest.name;
  
      std::list<IR_t*> to_be_inserted;
  
      { // Pass #1, pre-process for inserting list
	// Pass #1.1, refresh variable and label numbers
	std::map<int, int> number_reference;
	for(auto p = function_range.first;
	    p != function_range.second;
	    p++)
	  {
	    myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_174);
	    IROper_t irop = *(IROper_t*)*p;
	    IROper_t& copy_irop = *(new IROper_t
				    (_Rename_Local_Vars(irop, number_reference, current_id))
				    );
	    to_be_inserted.push_back((IR_t*)&copy_irop);
	  }
	// Pass #1+, Remove OPER_FUNC at the beginning of to_be_inserted
	{
	  auto p = to_be_inserted.begin();
	  myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_175);
	  IROper_t& irop = *(IROper_t*)*p;
	  myAssert(irop.oper == OPER_FUNC,ASSERT_EXIT_CODE_176);
	  to_be_inserted.erase(p);
	}
    
	// Pass #2, Eliminate LOAD_ARG, set with correspond SET_ARG
	{
	  auto p = to_be_inserted.begin();
	  auto q = std::prev(expansion_point);
	  while(1)
	    {
	      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_177);
	      IROper_t& irop = *(IROper_t*)*p;
	      myAssert(typeid(*(IROper_t*)*q) == typeid(IROper_t),ASSERT_EXIT_CODE_178);
	      IROper_t& prev_irop = *(IROper_t*)*q;
	  	  
	      if(irop.oper == OPER_LOAD_ARG)
		{
		  myAssert(prev_irop.oper == OPER_SET_ARG,ASSERT_EXIT_CODE_179);
		  myAssert(irop.ops1.name != "",ASSERT_EXIT_CODE_180);
		  myAssert(irop.ops1.name[0] == '@',ASSERT_EXIT_CODE_181);
		  myAssert(std::stoi(irop.ops1.name.substr(1)) ==
			   prev_irop.dest.imm,ASSERT_EXIT_CODE_182);
		  irop = IROper_t(OPER_MOVE, irop.dest, prev_irop.ops1);
		  prev_irop = IROper_t(OPER_NOP);
		}
	      else
		{
		  break;
		}
	      p++;
	      q--;
	    }
	}

	// Pass #2+, Insert a Function End Label at the end of code
	std::string end_label = std::string("INLINE_END_") + std::to_string(current_id++);
	{
	  to_be_inserted.push_back(new IROper_t(OPER_LABEL, end_label));
	}
	// Pass #3, Replace OPER_RETN and OPER_RET
	//────────────
	//   RETN X
	//   ─────>
	//   PHI_MOV RETN_TARGET, X
	//   JMP INLINE_END
	//────────────
	//   RET
	//   ─────>
	//   JMP INLINE_END
	//────────────
	{
	  auto p = to_be_inserted.begin();
	  while(1)
	    {
	      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_183);
	      IROper_t& irop = *(IROper_t*)*p;
	      if(irop.oper == OPER_RETN)
		{
		  to_be_inserted.insert(p, new IROper_t(OPER_PHI_MOV, retn_target, irop.dest));
		  irop = IROper_t(OPER_JMP, end_label);
		}
	      if(irop.oper == OPER_RET)
		{
		  irop = IROper_t(OPER_JMP, end_label);
		}
	      p++;
	      if(p == to_be_inserted.end())
		break;
	    }
	}

	// Pass #4, Inject code into proper position
	LinearIR.insert(expansion_point, to_be_inserted.begin(), to_be_inserted.end());
	// Pass #4+, Remove original CALL
	{
	  auto p = expansion_point;
	  myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_184);
	  IROper_t& irop = *(IROper_t*)*p;
	  myAssert(irop.oper == OPER_CALL,ASSERT_EXIT_CODE_185);
	  irop = IROper_t(OPER_NOP);
	}
	// printf("To Be Inserted:\n");
	// for(auto it : to_be_inserted)
	//   {
	// 	it->print(std::cerr, -1);
	//   }
	// printf("\n\n");

      }
      return;
    }
  else
    {
      myAssert(((IROper_t*)*expansion_point)->oper == OPER_CALL_NRET,ASSERT_EXIT_CODE_186);
      myAssert(((IROper_t*)*function_range.first)->dest.name ==
	       ((IROper_t*)*expansion_point)->dest.name,ASSERT_EXIT_CODE_187);
  
      std::list<IR_t*> to_be_inserted;
  
      { // Pass #1, pre-process for inserting list
	// Pass #1.1, refresh variable and label numbers
	std::map<int, int> number_reference;
	for(auto p = function_range.first;
	    p != function_range.second;
	    p++)
	  {
	    myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_188);
	    IROper_t irop = *(IROper_t*)*p;
	    IROper_t& copy_irop = *(new IROper_t
				    (_Rename_Local_Vars(irop, number_reference, current_id))
				    );
	    to_be_inserted.push_back((IR_t*)&copy_irop);
	  }
	// Pass #1+, Remove OPER_FUNC at the beginning of to_be_inserted
	{
	  auto p = to_be_inserted.begin();
	  myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_189);
	  IROper_t& irop = *(IROper_t*)*p;
	  myAssert(irop.oper == OPER_FUNC,ASSERT_EXIT_CODE_190);
	  to_be_inserted.erase(p);
	}
    
	// Pass #2, Eliminate LOAD_ARG, set with correspond SET_ARG
	{
	  auto p = to_be_inserted.begin();
	  auto q = std::prev(expansion_point);
	  while(1)
	    {
	      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_191);
	      IROper_t& irop = *(IROper_t*)*p;
	      myAssert(typeid(*(IROper_t*)*q) == typeid(IROper_t),ASSERT_EXIT_CODE_192);
	      IROper_t& prev_irop = *(IROper_t*)*q;
	  	  
	      if(irop.oper == OPER_LOAD_ARG)
		{
		  if(prev_irop.oper != OPER_SET_ARG)
		    {
		      printf("Assertion failed at:");
		      prev_irop.print(std::cerr, -1);
		      
		    }
		  myAssert(prev_irop.oper == OPER_SET_ARG,ASSERT_EXIT_CODE_193);
		  myAssert(irop.ops1.name != "",ASSERT_EXIT_CODE_194);
		  myAssert(irop.ops1.name[0] == '@',ASSERT_EXIT_CODE_195);
		  myAssert(std::stoi(irop.ops1.name.substr(1)) ==
			   prev_irop.dest.imm,ASSERT_EXIT_CODE_196);
		  irop = IROper_t(OPER_MOVE, irop.dest, prev_irop.ops1);
		  prev_irop = IROper_t(OPER_NOP);
		}
	      else
		{
		  break;
		}
	      p++;
	      q--;
	    }
	}

	// Pass #2+, Insert a Function End Label at the end of code
	std::string end_label = std::string("INLINE_END_") + std::to_string(current_id++);
	{
	  to_be_inserted.push_back(new IROper_t(OPER_LABEL, end_label));
	}
	// Pass #3, Replace OPER_RETN and OPER_RET
	//────────────
	//   RETN X
	//   ─────>
	//   PHI_MOV RETN_TARGET, X
	//   JMP INLINE_END
	//────────────
	//   RET
	//   ─────>
	//   JMP INLINE_END
	//────────────
	{
	  auto p = to_be_inserted.begin();
	  while(1)
	    {
	      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_197);
	      IROper_t& irop = *(IROper_t*)*p;
	      if(irop.oper == OPER_RETN)
		{
		  irop = IROper_t(OPER_JMP, end_label);
		}
	      if(irop.oper == OPER_RET)
		{
		  irop = IROper_t(OPER_JMP, end_label);
		}
	      p++;
	      if(p == to_be_inserted.end())
		break;
	    }
	}

	// Pass #4, Inject code into proper position
	LinearIR.insert(expansion_point, to_be_inserted.begin(), to_be_inserted.end());
	// Pass #4+, Remove original CALL
	{
	  auto p = expansion_point;
	  myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_198);
	  IROper_t& irop = *(IROper_t*)*p;
	  myAssert(irop.oper == OPER_CALL_NRET,ASSERT_EXIT_CODE_199);
	  irop = IROper_t(OPER_NOP);
	}
	// printf("To Be Inserted:\n");
	// for(auto it : to_be_inserted)
	//   {
	// 	it->print(std::cerr, -1);
	//   }
	// printf("\n\n");

      }

    }

}

void Function_RETN_to_RET(std::list<IR_t*>::iterator begin,
			  std::list<IR_t*>::iterator end)
{
  for(auto p = begin; p != end; p++)
    {
      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t),ASSERT_EXIT_CODE_200);
      IROper_t& irop = *(IROper_t*)*p;
      if(irop.oper == OPER_RETN)
	{
	  irop.oper = OPER_RET;
	  irop.dest = operand_t();
	  irop.ops1 = operand_t();
	  irop.ops2 = operand_t();
	  irop.ops3 = operand_t();
	}
    }
}
std::list<IR_t*> IRLine_Flow_Optimize(std::list<IR_t*>::iterator begin,
				      std::list<IR_t*>::iterator end);

void IRLine_Optimize(std::list<IR_t*>& LinearIR, int current_id, bool is_fake)
{
  // Pass 1, Find those function that
  //   1. Not refer or modify the global variable
  //   2. Arguments not contains array

  {
    auto fp_begin = LinearIR.begin();
    bool in_func = false;
    for(auto p = LinearIR.begin();
	p != LinearIR.end();
	++p)
      {
	auto& ptr = *p;
	if(typeid(*ptr) == typeid(IROper_t))
	  {
	    auto& op = *(IROper_t*)ptr;
	    if(op.oper == OPER_FUNC)
	      {
		fp_begin = p;
		in_func = true;
	      }
	    else if(op.oper == OPER_ENDF)
	      {
		myAssert(in_func == true, ASSERT_EXIT_CODE_67);
		if(Check_Local_Function(fp_begin, p))
		  {
		    IROper_t* ptr = (IROper_t*) (*fp_begin);
		    myAssert(typeid(*ptr) == typeid(IROper_t),ASSERT_EXIT_CODE_201);
		    myAssert(ptr->dest.get_static_type() == STATIC_OTHERS,ASSERT_EXIT_CODE_202);
		    local_function_table.insert(ptr->dest.name);
		    // std::cerr << COLOR_CYAN << ptr->dest.name << " is local function." << std::endl;
		  }
		in_func = false;
	      }
	  }
      }

    // Special: [main] is never local function
    if(local_function_table.count("main"))
      local_function_table.erase("main");
  }

  // Pass 2, Check local function elimination
  {
    auto fp_begin = LinearIR.begin();
    bool in_func = false;
    for(auto p = LinearIR.begin();
	p != LinearIR.end();
	++p)
      {
	auto& ptr = *p;
	if(typeid(*ptr) == typeid(IROper_t))
	  {
	    auto& op = *(IROper_t*)ptr;
	    if(op.oper == OPER_FUNC)
	      {
		fp_begin = p;
		in_func = true;
	      }
	    else if(op.oper == OPER_ENDF)
	      {
		myAssert(in_func == true, ASSERT_EXIT_CODE_67);
		Reduce_Redundant_Function_Call(fp_begin, p, local_function_table);
		in_func = false;
	      }
	  }
      }

    // 2.2, Refresh
    auto p = LinearIR.begin();
    while(p != LinearIR.end())
      {
	auto& ptr = *p;
	if(typeid(*ptr) == typeid(IROper_t))
	  {
	    auto& op = *(IROper_t*)ptr;
	    if(op.valid == 0)
	      {
		p = LinearIR.erase(p);
		continue;
	      }
	  }
	p++;
      }
  }

  // Pass 3. Constant Propagation
  // Disable it to gain more performance, hah.
  {
    auto fp_begin = LinearIR.begin();
    bool in_func = false;
    for(auto p = LinearIR.begin();
    	p != LinearIR.end();
    	++p)
      {
    	auto& ptr = *p;
    	if(typeid(*ptr) == typeid(IROper_t))
    	  {
    	    auto& op = *(IROper_t*)ptr;
    	    if(op.oper == OPER_FUNC)
    	      {
    		fp_begin = p;
    		in_func = true;
    	      }
    	    else if(op.oper == OPER_ENDF)
    	      {
    		myAssert(in_func == true, ASSERT_EXIT_CODE_67);
    		Constant_Propagation(fp_begin, p);
    		in_func = false;
    	      }
    	  }
      }
  }

  // Pass 3+, pre-Flow Optimize
  {
    auto fp_begin = LinearIR.begin();
    bool in_func = false;
    for(auto p = LinearIR.begin();
  	p != LinearIR.end();
  	++p)
      {
  	auto& ptr = *p;
  	if(typeid(*ptr) == typeid(IROper_t))
  	  {
  	    auto& op = *(IROper_t*)ptr;
  	    if(op.oper == OPER_FUNC)
  	      {
  		fp_begin = p;
  		in_func = true;
  	      }
  	    else if(op.oper == OPER_ENDF)
  	      {
  		myAssert(in_func == true, ASSERT_EXIT_CODE_67);
  		auto li = IRLine_Flow_Optimize(fp_begin, p);
  		p = LinearIR.erase(fp_begin, p);
  		LinearIR.insert(p, li.begin(), li.end());
  		in_func = false;
  	      }
  	  }
      }
  }
  
  // Pass 4. Function Inlining
  if(!is_fake)
    { // Pass 4.1, construct function call dependency

      // Pass 4.1.1, Initialize
      std::map<std::string, std::set<std::string> > calling_dependency;
      std::map<std::string, int> function_size;
      std::map<std::string, std::pair<std::list<IR_t*>::iterator,
    				      std::list<IR_t*>::iterator>> function_range;
      auto fp_begin = LinearIR.begin();
      bool in_func = false;
      int  cnt = 0;
      for(auto p = LinearIR.begin();
    	  p != LinearIR.end();
    	  ++p)
    	{
    	  auto& ptr = *p;
    	  if(typeid(*ptr) == typeid(IROper_t))
    	    {
    	      auto& op = *(IROper_t*)ptr;
    	      if(op.oper == OPER_FUNC)
    		{
    		  fp_begin = p;
    		  in_func = true;
    		  cnt = 0;
    		}
    	      else if(op.oper == OPER_ENDF)
    		{
    		  myAssert(in_func == true, ASSERT_EXIT_CODE_67);
    		  std::string func_name = Initialize_Calling_Dependency(fp_begin, p, calling_dependency);
    		  function_size[func_name] = cnt + 1;
    		  function_range[func_name] = {fp_begin, p};
    		  cnt = 0;
    		  in_func = false;
    		}
    	      else
    		{
    		  cnt++;
    		}
	    
    	    }
    	}

      // Pass 4.1.2, Iterate
      bool changed = true;
      while(changed)
    	{
    	  changed = false;
    	  for(auto& p : calling_dependency)
    	    {
    	      std::set<std::string> callees = p.second;
    	      for(auto q : p.second)
    		{
    		  callees.insert(calling_dependency[q].begin(),
    				 calling_dependency[q].end());
    		}
    	      if(callees != p.second)
    		{
    		  p.second = callees;
    		  changed = true;
    		}
    	    }
    	}
      // for(auto p : calling_dependency)
      //   {
      // 	printf("Function %s calls:", p.first.c_str());
      // 	for(auto q : p.second)
      // 	  {
      // 	    printf("%s ", q.c_str());
      // 	  }
      // 	printf("\n");
      //   }

      // Pass 4.2, Collect all inlinable functions
      std::set<std::string> inlinable_functions;
      for(auto p : calling_dependency)
    	{
    	  if(!p.second.count(p.first) &&
    	     function_size[p.first] < 200)
    	    {
    	      inlinable_functions.insert(p.first);
    	    }
    	}
      // printf("Inlinable:\n");
      // for(auto p : inlinable_functions)
      // 	{
      // 	  std::cout<<p<<" ";
      // 	}
      // std::cout<<std::endl;
      // Pass 4.3, Expand inline functions
      {
    	auto p = LinearIR.begin();
    	while(1)
    	  {
    	    if(typeid(*(IROper_t*)*p) != typeid(IROper_t))
    	      {
    		p++;
    		if(p == LinearIR.end())
    		  break;
    		continue;
    	      }
    	    IROper_t irop = *(IROper_t*)*p;
    	    if(irop.oper == OPER_CALL &&
    	       inlinable_functions.count(irop.ops1.name) &&
    	       function_range.count(irop.ops1.name))
    	      {
    		auto q = std::next(p);
    		// printf("Function Range Testing: %s\n",irop.ops1.name.c_str());
    		Inline_Expand(LinearIR, p, function_range.at(irop.ops1.name), current_id);
    		p = q;
    	      }
    	    else if(irop.oper == OPER_CALL_NRET &&
		    inlinable_functions.count(irop.dest.name) &&
		    function_range.count(irop.dest.name))

    	      {
    		auto q = std::next(p);
    		// printf("Function Range Testing: %s\n",irop.ops1.name.c_str());
    		Inline_Expand(LinearIR, p, function_range.at(irop.dest.name), current_id);
    		p = q;
	      }
	    else
	      {
    		p++;
    	      }
    	    if(p == LinearIR.end())
    	      break;
    	  }
      }
    }

  // Pass 5. Eliminate Unused SET_ARG
  {
    auto setarg_begin = LinearIR.begin();
    bool in_setarg = false;
    for(auto p = LinearIR.begin();
  	p != LinearIR.end();
  	++p)
      {
  	auto& ptr = *p;
  	if(typeid(*ptr) == typeid(IROper_t))
  	  {
  	    auto& op = *(IROper_t*)ptr;
  	    if(op.oper == OPER_SET_ARG)
  	      {
		if(!in_setarg)
		  {
		    setarg_begin = p;
		    in_setarg = true;
		  }
  	      }
  	    else
  	      {
		if(in_setarg &&
		   op.oper != OPER_CALL &&
		   op.oper != OPER_CALL_NRET)
		  {
		    p = LinearIR.erase(setarg_begin, p);
		  }
		in_setarg = false;
  	      }
  	  }
      }
  }
  // Pass 5. Flow Optimize
  {
    auto fp_begin = LinearIR.begin();
    bool in_func = false;
    for(auto p = LinearIR.begin();
  	p != LinearIR.end();
  	++p)
      {
  	auto& ptr = *p;
  	if(typeid(*ptr) == typeid(IROper_t))
  	  {
  	    auto& op = *(IROper_t*)ptr;
  	    if(op.oper == OPER_FUNC)
  	      {
  		fp_begin = p;
  		in_func = true;
  	      }
  	    else if(op.oper == OPER_ENDF)
  	      {
  		myAssert(in_func == true, ASSERT_EXIT_CODE_67);
  		auto li = IRLine_Flow_Optimize(fp_begin, p);
  		p = LinearIR.erase(fp_begin, p);
  		LinearIR.insert(p, li.begin(), li.end());
  		in_func = false;
  	      }
  	  }
      }
  }

  // Pass 4. Record Phi Vars
    for(auto p = LinearIR.begin();
	p != LinearIR.end();
	++p)
      {
	if(typeid(*(*p)) != typeid(IROper_t))
	  continue;
	IROper_t& irop = *(IROper_t*)(*p);
	if(irop.oper == OPER_PHI_MOV)
	  {
	    myAssert(irop.dest.name != "",ASSERT_EXIT_CODE_203);
	    myAssert(irop.dest.name[0] == '%',ASSERT_EXIT_CODE_204);
	    phi_vars.insert(irop.dest.name);
	  }
      }

  
  // Pass 6. Eliminate Not-used Function
    {
      // 6.1, Collect Calling dependency
      std::map<std::string, std::set<std::string> > calling_dependency;
      std::map<std::string, int> function_size;
      std::map<std::string, std::pair<std::list<IR_t*>::iterator,
    				      std::list<IR_t*>::iterator>> function_range;
      std::set<std::string> func_names;
      auto fp_begin = LinearIR.begin();
      bool in_func = false;
      int  cnt = 0;
      for(auto p = LinearIR.begin();
    	  p != LinearIR.end();
    	  ++p)
    	{
    	  auto& ptr = *p;
    	  if(typeid(*ptr) == typeid(IROper_t))
    	    {
    	      auto& op = *(IROper_t*)ptr;
    	      if(op.oper == OPER_FUNC)
    		{
    		  fp_begin = p;
    		  in_func = true;
    		  cnt = 0;
    		}
    	      else if(op.oper == OPER_ENDF)
    		{
    		  myAssert(in_func == true, ASSERT_EXIT_CODE_67);
    		  std::string func_name = Initialize_Calling_Dependency(fp_begin, p, calling_dependency);
		  func_names.insert(func_name);
    		  function_size[func_name] = cnt + 1;
    		  function_range[func_name] = {fp_begin, p};
    		  cnt = 0;
    		  in_func = false;
    		}
    	      else
    		{
    		  cnt++;
    		}
	    
    	    }
    	}

      // 6.2, Propagate
      bool changed = true;
      while(changed)
    	{
    	  changed = false;
    	  for(auto& p : calling_dependency)
    	    {
    	      std::set<std::string> callees = p.second;
    	      for(auto q : p.second)
    		{
    		  callees.insert(calling_dependency[q].begin(),
    				 calling_dependency[q].end());
    		}
    	      if(callees != p.second)
    		{
    		  p.second = callees;
    		  changed = true;
    		}
    	    }
    	}

      // 6.3, Eliminate
      for(auto it : func_names)
	{

	  bool can_be_eliminate = true;
	  if(it == "main")
	    can_be_eliminate = false;
	  for(auto it2 : calling_dependency)
	    {
	      for(auto it3 : it2.second)
		{
		  if(it3 == it)
		    can_be_eliminate = false;
		}
	    }
	  if(can_be_eliminate)
	    {
	      // printf("Eliminating unused function %s\n", it.c_str());
	      // fflush(stdout);
	      myAssert(function_range.count(it),ASSERT_EXIT_CODE_205);
	      LinearIR.erase(function_range[it].first,
			     std::next(function_range[it].second));
	    }
	}
    }

    // Pass 7. Conditional Folding
    {
      for(auto p = LinearIR.begin();
    	  p != LinearIR.end();
    	  ++p)
    	{
    	  auto& ptr = *p;
    	  if(typeid(*ptr) == typeid(IROper_t))
    	    {
    	      auto& op = *(IROper_t*)ptr;
    	      if(op.oper == OPER_CMP && op.dest.get_static_type() == STATIC_IMM &&
    		 op.ops1.get_static_type() == STATIC_IMM)
    		{
		  printf("Eval const CMP at:");
		  op.print(std::cerr,-1);
    		  int eval_result = _eval_const_cmp(op);
    		  auto q = std::next(p);
		  op = IROper_t(OPER_NOP);
    		  while(1)
    		    {
		      // if(q == LinearIR.end())
		      // 	break;
		      myAssert(q != LinearIR.end(),ASSERT_EXIT_CODE_206);
    		      myAssert(typeid(**q) == typeid(IROper_t),ASSERT_EXIT_CODE_207);
    		      auto& cond_use_op = *(IROper_t*)*q;
    		      switch(cond_use_op.oper)
    			{
#define COND_JUMP(COND, inary)			\
			  case COND:		\
			    {			\
			    if(eval_result inary 0)		\
			      cond_use_op.oper = OPER_JMP;	\
			    else				\
			      cond_use_op = IROper_t(OPER_NOP);	\
			    goto _condjmp_while_fin;		\
			    }
			  COND_JUMP(OPER_JEQ, ==);
			  COND_JUMP(OPER_JNE, !=);
			  COND_JUMP(OPER_JGT, >);
			  COND_JUMP(OPER_JLT, <);
			  COND_JUMP(OPER_JGE, >=);
			  COND_JUMP(OPER_JLE, <=);			  
#undef COND_JUMP
#define COND_MOV(COND, inary)						\
			  case COND:					\
			    {						\
			      auto r =std::next(q);			\
			      myAssert(r != LinearIR.end(),ASSERT_EXIT_CODE_208);		\
			      auto& cond_use_op2 = *(IROper_t*)*r;	\
			      if(eval_result inary 0)			\
				{					\
				  cond_use_op.oper = OPER_MOVE;		\
				  cond_use_op2 = IROper_t(OPER_NOP);	\
				}					\
			      else					\
				{					\
				  cond_use_op = IROper_t(OPER_NOP);	\
				  cond_use_op2.oper = OPER_MOVE;	\
				}					\
			      goto _condjmp_while_fin;			\
			    }
			  COND_MOV(OPER_MOVEQ, ==);
			  COND_MOV(OPER_MOVNE, !=);
			  COND_MOV(OPER_MOVGT, >);
			  COND_MOV(OPER_MOVLT, <);
			  COND_MOV(OPER_MOVGE, >=);
			  COND_MOV(OPER_MOVLE, <=);			  
			default:
			  {
			    q++;
			    continue;
			  }
    			}

    		    }
		_condjmp_while_fin:
		  ;
    		}
    	    }
    	}

    }
}


std::list<IROper_t*> Tree_to_Line(IRList_t* li)
{ // Collect [li]'s elements into a register
  std::list<IR_t*> tmp_li;
  li->collect(tmp_li);
  std::list<IROper_t*> retn_list;
  for(auto it : tmp_li)
    {
      if(typeid(*it) != typeid(IROper_t))
	continue;
      myAssert(typeid(*it) == typeid(IROper_t),ASSERT_EXIT_CODE_209);
      retn_list.push_back((IROper_t*)it);
    }
  return retn_list;
}


bool Invariant_Code_Motion(std::list<IR_t*>& ir_outer,
			   std::list<IR_t*>& ir_inner)
{
  bool changed = false;
  // Pass 1. Find all variables it referenced
  std::set<std::string> ref_vars;
  
  for(auto p = ir_inner.begin();
      p != ir_inner.end();
      ++p)
	{
	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_210);
	  IROper_t& irop = *(IROper_t*)(*p);
	  myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_211);

#define COUNT_REFS(opern, i)					\
	  if(OPERATION_REGISTERS.at(irop.oper)[i] == 0		\
	     /* Is a Reference */				\
	     && opern.get_static_type() == STATIC_LOCAL_VAR)	\
	    {							\
	      auto liter = opern.name;				\
	      ref_vars.insert(liter);				\
	    }
	  COUNT_REFS(irop.dest, 0);
	  COUNT_REFS(irop.ops1, 1);
	  COUNT_REFS(irop.ops2, 2);
	  COUNT_REFS(irop.ops3, 3);
#undef COUNT_REFS
	}

  // Pass 2. If one of these var is assigned inside the block, erase it from our set
  for(auto p = ir_inner.begin();
      p != ir_inner.end();
      ++p)
	{
	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_212);
	  IROper_t& irop = *(IROper_t*)(*p);
	  myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_213);
	  if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 && // Is Writing
	     irop.dest.get_static_type() == STATIC_LOCAL_VAR)
	    {
	      if(ref_vars.count(irop.dest.name))
		{
		  ref_vars.erase(irop.dest.name);
		}
	    }
	}

  // Pass 3. If an IR only refers to unchanged local vars, move it out from the block
 for(auto p = ir_inner.begin();
     p != ir_inner.end();
      ++p)
 	{
 	  myAssert(typeid(*(*p)) == typeid(IROper_t),ASSERT_EXIT_CODE_214);
 	  IROper_t& irop = *(IROper_t*)(*p);
	  // 1. Check if a instruction is scheduable
	  if(!check_scheduable(irop))
	    continue;

	  // 2. Check if a instruction's ops is never rewrite
#define CHECK_REWRITE(opern, i)					\
	  if(OPERATION_REGISTERS.at(irop.oper)[i] == 0		\
	     /* Is a Reference */				\
	     && opern.get_static_type() == STATIC_LOCAL_VAR)	\
	    {							\
	      auto liter = opern.name;				\
	      if(!ref_vars.count(liter))			\
		{						\
		  continue;					\
		}						\
	    }
	  CHECK_REWRITE(irop.dest, 0);
	  CHECK_REWRITE(irop.ops1, 1);
	  CHECK_REWRITE(irop.ops2, 2);
	  CHECK_REWRITE(irop.ops3, 3);
#undef CHECK_REWRITE
	  // if(irop.out_current_loop)
	  //   continue;
	  myAssert(irop.dest.get_static_type() == STATIC_LOCAL_VAR,ASSERT_EXIT_CODE_215);
	  IROper_t* op2 = new IROper_t(irop);
	  op2->out_current_loop = 1;
	  ir_outer.push_back(op2);
	  // std::cerr<<("Outer push back:");
	  // irop.print(std::cerr, -1);
	  if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
	     irop.dest.name != "" &&
	     irop.dest.name[0] == '%')
	    {
	      phi_vars.insert(irop.dest.name);
	    }
	  
	  irop = IROper_t(OPER_NOP);
	  changed = 1;
 	}
  return changed;
}

void IRLine_Optimize_PostFlow(std::list<IR_t*>& LinearIR, std::list<IR_t*>& Fake_LinearIR)
{
  // Pass 1. If a function's return value is not used in any function call, eliminate it.
  {
    // Pass 1.1, Collect all function that their return value is used
    std::set<std::string> retn_value_used;
    {
      auto p = Fake_LinearIR.begin();
      while(1)
	{
	  if(typeid(*(IROper_t*)*p) != typeid(IROper_t))
	    {
	      p++;
	      if(p == Fake_LinearIR.end())
		break;
	      continue;
	    }
	  IROper_t irop = *(IROper_t*)*p;
	  if(irop.oper == OPER_CALL)
	    {
	      retn_value_used.insert(irop.ops1.name);
	    }
	  p++;
	  if(p == Fake_LinearIR.end())
	    break;
	}
    }
    
    // Pass 1.2, For each function, if its return value is not used, eliminate RETN into RET
    {
      auto fp_begin = LinearIR.begin();
      bool in_func = false;
      std::string function_name;
      for(auto p = LinearIR.begin();
	  p != LinearIR.end();
	  ++p)
	{
	  auto& ptr = *p;
	  if(typeid(*ptr) == typeid(IROper_t))
	    {
	      auto& op = *(IROper_t*)ptr;
	      if(op.oper == OPER_FUNC)
		{
		  myAssert(op.dest.name != "",ASSERT_EXIT_CODE_216);
		  function_name = op.dest.name;
		  fp_begin = p;
		  in_func = true;
		}
	      else if(op.oper == OPER_ENDF)
		{
		  myAssert(function_name != "",ASSERT_EXIT_CODE_217);
		  myAssert(in_func == true, ASSERT_EXIT_CODE_67);
		  if(!retn_value_used.count(function_name) &&
		     function_name != "main")
		    Function_RETN_to_RET(fp_begin, p);
		  in_func = false;
		}
	    }
	}

    }
  }
}



void _irline_Flow_Optimize(std::list<basic_block_t*>& blks)
{

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
		myAssert(OPERATION_REGISTERS.count(irop.oper),ASSERT_EXIT_CODE_218);
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
				// Delete SET_ARG for this call
				auto r = q;
				if(r != (*p)->begin)
				  r--;
				while(1)
				  {

				    myAssert(typeid(*(IROper_t*)(*r)) == typeid(IROper_t), ASSERT_EXIT_CODE_88);
				    auto& r_irop = *(IROper_t*)(*r);
				    if(r_irop.oper == OPER_SET_ARG)
				      {
					r_irop.valid = 0;
				      }
				    else
				      {
					break;
				      }
				    if(r == (*p)->begin)
				      break;
				    else
				      {
					r--;
				      }
				  }
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
		myAssert(typeid(*(IROper_t*)*q) == typeid(IROper_t),ASSERT_EXIT_CODE_219);
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

std::list<IR_t*> IRLine_Flow_Optimize(std::list<IR_t*>::iterator begin,
				      std::list<IR_t*>::iterator end)
{
  IROper_t* ptr =(IROper_t*) (*begin);
  myAssert(typeid(*ptr) == typeid(IROper_t), ASSERT_EXIT_CODE_69);
  myAssert(ptr->oper == OPER_FUNC, ASSERT_EXIT_CODE_70);
  

  std::list<IR_t*> irs(begin,end);
  std::list<basic_block_t*> blks;

  // Pass 1. Create Basic block scopes
  {
    // Collect all labels as jump targets
    std::set<std::string> target_labels;

    // Pass 1.1, Scan all [JMP/Jcc] to find separate points
    for(auto p = irs.begin();
	p != irs.end();
	++p)
      {
	if(typeid(*(*p)) != typeid(IROper_t))
	  continue;
	IROper_t& irop = *(IROper_t*)(*p);
	if(UNCONDITION_BRANCH_OPERATIONS.count(irop.oper) ||
	   CONDITION_BRANCH_OPERATIONS.count(irop.oper))
	  // Is a Branch Operation
	  {
	    if(irop.oper != OPER_RET &&
	       irop.oper != OPER_RETN)
	      { // Jump with a label
		myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_71);
		target_labels.insert(irop.dest.name);
	      }
	    // Collect target location
	    ++p;
	    p = irs.insert(p, new IRSep_t);
	  }
      }

    // Pass 1.2, Scan all [Labels] as jump target
    for(auto p = irs.begin();
	p != irs.end();
	++p)
      {
	if(typeid(*(*p)) != typeid(IROper_t))
	  continue;
	IROper_t& irop = *(IROper_t*)(*p);
	if(irop.oper == OPER_LABEL)
	  {
	    myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_72);
	    if(target_labels.count(irop.dest.name))
	      {
		p = irs.insert(p, new IRSep_t);
		p++; // Skip this [LABEL]
	      }
	  }
      }

    // Pass 1.3, Collect all blocks
    auto block_begin = irs.begin();
    for(auto p = irs.begin();
	p != irs.end();
	++p)
      {
	if(typeid(*(*p)) == typeid(IRSep_t))
	  {
	    auto block_end = p;
	    basic_block_t* bp = new basic_block_t(block_begin, block_end);
	    blks.push_back(bp);
	    block_begin = p;
	    std::advance(block_begin,1);
	  }
      }
    // Pass 1.4, Eliminate empty blocks
    for(auto p = blks.begin();
	p != blks.end();
	++p)
      {
	if((*p)->begin == (*p)->end)
	  {
	    p = blks.erase(p);
	    p--;
	    // Warning: vector::begin() - 1 is UNDEFINED, hence, if p == begin, p-- is not defined
	    // But the first block won't be empty.
	  }
      }
    // Pass 1.5, Add blocks' JUMP constraints
    for(auto p = blks.begin();
    	p != blks.end();
    	++p)
      {
    	auto end_iropp = std::prev((*p)->end);
    	auto& end_irop = *(IROper_t*) *end_iropp;

    	if(CONDITION_BRANCH_OPERATIONS.count(end_irop.oper))
	  // for CONDITIONAL JUMPs
    	  {
    	    myAssert(std::next(p) != blks.end(), ASSERT_EXIT_CODE_73);
    	    basic_block_t::AddEdge(*p, *(std::next(p)));
    	    auto jmp_target = end_irop.dest.name;
	    basic_block_t* target = NULL;
#define FIND_TARGET_LABEL(jmp_target, target)			\
	    {							\
	      for(auto q = blks.begin();			\
		  q != blks.end();				\
		  ++q)						\
		{						\
		  auto qiropp = *(*q)->begin;			\
		  myAssert(typeid(*qiropp) == typeid(IROper_t), ASSERT_EXIT_CODE_74);	\
		  auto& qirop = *(IROper_t*) qiropp;		\
		  if(qirop.oper == OPER_LABEL &&		\
		     qirop.dest.name == jmp_target)		\
		    {						\
		      target = *q;				\
		    }						\
		}						\
	    }
	    
	    FIND_TARGET_LABEL(jmp_target, target);
	    myAssert(target != NULL, ASSERT_EXIT_CODE_75);
	    basic_block_t::AddEdge(*p, target);
	  }
	else if(UNCONDITION_BRANCH_OPERATIONS.count(end_irop.oper))
	  {
	    if(end_irop.oper == OPER_JMP)
	      {
		auto jmp_target = end_irop.dest.name;
		basic_block_t* target = NULL;
		FIND_TARGET_LABEL(jmp_target, target);
		myAssert(target != NULL, ASSERT_EXIT_CODE_76);
		basic_block_t::AddEdge(*p, target);		
	      }
	    else
	      {
		// For RETURNs, do nothing
	      }
	  }
	else // Ordinary Instructions
	  {
	    myAssert(std::next(p) != blks.end(), ASSERT_EXIT_CODE_77);
    	    basic_block_t::AddEdge(*p, *(std::next(p)));	    
	  }
      }
#undef FIND_TARGET_LABEL
    // Pass 1.6, Eliminate blocks that is not referred (not include first block)

    // 1.6.1 Remove from [blks]
    // Warning: The first block won't be un-referred
    std::set<basic_block_t*> erased;
    bool changed = false;
    while(1)
      {
	for(auto p = std::next(blks.begin());
	    p != blks.end();
	    ++p)
	  {
	    myAssert(p != blks.begin(), ASSERT_EXIT_CODE_78);
	    bool operate_remove = true;
	    for(auto it : (*p)->in_edges)
	      {
		if(! erased.count(it))
		  {
		    operate_remove = false;
		    break;
		  }
	      }
	    if(operate_remove)
	      {
		changed = true;
		erased.insert(*p);
		p = blks.erase(p);
		p--;
	      }
	  }
	if(changed)
	  {
	    changed = false;
	  }
	else
	  {
	    break;
	  }
      }
    // 1.6.2 Remove from each basicblock's edge list
    for(auto p = blks.begin();
	p != blks.end();
	++p)
      {
#define REMOVE_UNUSED_BLKS(li)			\
	{					\
	  auto q = li.begin();			\
	  while(q != li.end())			\
	    {					\
	      if(erased.count(*q))		\
		{				\
		  q = li.erase(q);		\
		  continue;			\
		}				\
	      else				\
		{				\
		  ++q;				\
		}				\
	    }					\
	}
	REMOVE_UNUSED_BLKS((*p)->in_edges);
	REMOVE_UNUSED_BLKS((*p)->out_edges);
#undef REMOVE_UNUSED_BLKS
      }

    
    
  }
  // Pass 2. Live Variable Analyze for each block
  {
    // Pass 2.1 Find [UEVar] and [VarKill] for each block
    for(auto p = blks.begin();
	p != blks.end();
	++p)
      {
	 (*p)->get_register_dependency();
	 (*p)->liveOut.clear();
      }
    
    // Pass 2.2 Find [LiveOut] for each block
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

  // Pass 3. Optimize on BasicBlock by DataFlow Analysis
  {
    // Pass 3.1, detach each block against [irs], it's no needed anymore
    for(auto it : blks)
      {
	std::list<IR_t*>* tmpList = new std::list<IR_t*>(it->begin, it->end);
	// tmpList->push_back((IR_t*)new IROper_t(OPER_BLK_SEP));
	it->begin = tmpList->begin();
	it->end = tmpList->end();
      }

    // Pass 3.2, Allocate a start block at the beginning of all blocks,
    // and a end block at the end of all blocks
    std::list<IR_t*>* tmpList = new std::list<IR_t*>();
    basic_block_t* blk_entry = new basic_block_t(tmpList->begin(), tmpList->begin());
    basic_block_t* blk_exit = new basic_block_t(tmpList->begin(), tmpList->begin());
    for(auto it : blks)
      {
    	if(it->in_edges.empty())
    	  {
    	    it->in_edges.push_back(blk_entry);
    	    blk_entry->out_edges.push_back(it);
    	  }
    	if(it->out_edges.empty())
    	  {
    	    it->out_edges.push_back(blk_exit);
    	    blk_exit->in_edges.push_back(it);
    	  }
      }
    blks.push_front(blk_entry);
    blks.push_back(blk_exit);

    // Pass 3.3, Optimize
    _irline_Flow_Optimize(blks);

  }
  // Pass 5, Reconstruct IRLine
  std::list<IR_t*> retn;
  for(auto it : blks)
    {
      retn.insert(retn.end(), it->begin, it->end);
    }
  return retn;
}


// std::list<IR_t*> Try_Unroll_Loop()
