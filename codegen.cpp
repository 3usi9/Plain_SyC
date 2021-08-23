#include "codegen.h"
#include "common.h"
#include "flow_optimize.h"
#include "asm_optimize.h"
#include<typeinfo>
#include<algorithm>
#include<stack>
#include<cctype>
///////////////////////////////////////////////
///////U T I L I T I E S///////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern int print_ASMLine;
extern int print_OptASMLine;
extern int optimize_flag;		
extern int print_FlowGraph;
extern int print_optFlowGraph;
extern int print_Register;
extern std::map<std::string, int> argcount;
extern std::set<std::string> phi_vars;
extern void print_nocolor(std::ostream& out,
			  const std::string& in);

std::map<std::string, int> offset_indicator;
int current_offset = 0;
// Indicate current absoulte offset

std::string rename_operand(operand_t op)
{
  if(op.type == OPERAND_IMM)
    {
      return COLOR_RED "#" COLOR_GREEN +std::to_string(op.imm);
    }
  myAssert(op.type == OPERAND_VAR, ASSERT_EXIT_CODE_60);
    {
      if(op.name[0] == '$')
	{
	  if(op.name[1] == '&') // Array
	    return COLOR_RED "__ARRAY__" COLOR_YELLOW + op.name.substr(2);
	  else // Variable
	    return COLOR_RED "__VAR__" COLOR_YELLOW + op.name.substr(1);
	}
      else
	{
	  return COLOR_YELLOW + op.name;
	}
    }
}

std::string rename_operand(std::string s)
{
  if(s[0] == '$')
    {
      if(s[1] == '&') // Array
	return COLOR_RED "__ARRAY__" COLOR_YELLOW + s.substr(2);
      else // Variable
	return COLOR_RED "__VAR__" COLOR_YELLOW + s.substr(1);
    }
  else
    {
      return COLOR_YELLOW + s;
    }
}

bool is_special_register(std::string name)
{
  if(name == "sp" ||
     name == "lr" ||
     name == "pc")
    return true;

  return false;
}

bool is_register(std::string name)
{
  if(name.size() < 2)
    return false;
  if(name[0] == 'r' && isdigit(name[1]))
    return true;
  if(is_special_register(name))
    return true;
  return false;
}
int var_to_int(const std::string& var, int exit_code)
{
  
  if(var == "")
    exit(exit_code);

  myAssert(var[0] == '%' || var[0] == '@' || var[0] == '^', ASSERT_EXIT_CODE_61);
  int cnt = 0;
  while(!isdigit(var[cnt]))
    cnt++;
  return std::stoi(var.substr(cnt));
}

_asm_rhs_t::_asm_rhs_t(MNEMONIC_TYPE mnemo,
		       operand_t ops1,
		       operand_t ops2,
		       operand_t ops3) :
  mnemo(mnemo), ops1(ops1), ops2(ops2), ops3(ops3)
{
}

bool _asm_rhs_t::operator<(const _asm_rhs_t &y) const
{
  if(this->mnemo != y.mnemo)
    return this->mnemo < y.mnemo;
  myAssert(this->mnemo == y.mnemo,ASSERT_EXIT_CODE_109);

  if(this->ops1 != y.ops1)
    return this->ops1 < y.ops1;
  myAssert(this->ops1 == y.ops1,ASSERT_EXIT_CODE_110);

  if(this->ops2 != y.ops2)
    return this->ops2 < y.ops2;
  myAssert(this->ops2 == y.ops2,ASSERT_EXIT_CODE_111);

  if(this->ops3 != y.ops3)
    return this->ops3 < y.ops3;
  myAssert(this->ops3 == y.ops3,ASSERT_EXIT_CODE_112);
  return false;
}

bool _asm_rhs_t::operator==(const _asm_rhs_t &y) const
{
    return ((!(*this<y)) && !(y<*this));
}

bool _asm_rhs_t::operator!=(const _asm_rhs_t &y) const
{
  return !((*this) == y);
}

///////////////////////////////////////////////
///////B A S I C///////////////////////////////
//////////////B L O C K////////////////////////
///////////////////////////////////////////////
basic_block_t::basic_block_t(std::list<IR_t*>::iterator begin,
			     std::list<IR_t*>::iterator end)
  : begin(begin), end(end), __has_ts(0)
{
  static int blkid = 0;
  this->id = blkid++;
}

bool basic_block_t::has_timestamp()
{
  return this->__has_ts;
}

int basic_block_t::assign_timestamp(int cur_time)
{
  if(this->__has_ts)
    myAssert(0, ASSERT_EXIT_CODE_62);
  this->__has_ts = true;
  for(auto p = this->begin;
      p != this->end;
      ++p)
    {
      myAssert(typeid(*(IROper_t*)*p) == typeid(IROper_t), ASSERT_EXIT_CODE_63);
      auto& irop = *(IROper_t*)*p;
      irop.timestamp = cur_time++;
    }
  return cur_time;
}

void basic_block_t::print(std::ostream &out)
{
  {
    std::stringstream ss;
    ss<<COLOR_WHITE "Basic Block " COLOR_CYAN + std::to_string(this->id);

    ss<< COLOR_YELLOW ": {";
    bool first = true;
    for(auto it : this->in_edges)
      {
	if(first)
	  first = false;
	else
	  ss<<COLOR_YELLOW ",";
	ss<< COLOR_CYAN << it->id;
      }
    ss<<COLOR_YELLOW "}" COLOR_RED " -> ";
    ss<< COLOR_CYAN <<this->id;
    ss<<COLOR_RED " -> " COLOR_YELLOW "{";
    first = true;
    for(auto it : this->out_edges)
      {
	if(first)
	  first = false;
	else
	  ss<<COLOR_YELLOW ",";
	ss<< COLOR_CYAN << it->id;
      }
    ss<<COLOR_YELLOW "}";
    IRNote_t note(ss.str());
    note.print(out, 0);
  }
#define PROVIDE_AND_REQUIRE(set, message)	\
  {						\
    std::stringstream ss;			\
    ss<<COLOR_WHITE message COLOR_CYAN;		\
    IRNote_t note(ss.str());			\
    note.print(out, 0);				\
    ss.clear();					\
    ss.str("");					\
    bool first = true;				\
    int cnt = 0;				\
    for(auto it : set)				\
      {						\
	if(first)				\
	  first = false;			\
	else					\
	  {					\
	    ss<<COLOR_YELLOW ",";		\
	    if(cnt % 5 == 0)			\
	      {					\
		IRNote_t note(ss.str());	\
		note.print(out, 0);		\
		ss.clear();			\
		ss.str("");			\
	      }					\
	  }					\
	ss<<COLOR_CYAN<<it;			\
	cnt++;					\
      }						\
    if(first)					\
      {						\
	ss<<COLOR_GREEN "NONE";			\
	IRNote_t note(ss.str());		\
	note.print(out, 0);			\
	ss.str("");				\
      }						\
    if(ss.str() != "")				\
      {						\
	IRNote_t note(ss.str());		\
	note.print(out, 0);			\
      }						\
  }

  // PROVIDE_AND_REQUIRE(this->provide_variables, "Provides: ");
  // PROVIDE_AND_REQUIRE(this->require_variables, "Requires: ");
  // PROVIDE_AND_REQUIRE(this->liveIn, "LiveIn: ");
  PROVIDE_AND_REQUIRE(this->liveOut, "LiveOut: ");
  PROVIDE_AND_REQUIRE(this->AvailIn, "AvailIn: ");
  PROVIDE_AND_REQUIRE(this->RegliveOut, "RegLiveOut: ");
  PROVIDE_AND_REQUIRE(this->RegliveIn, "RegLiveIn: ");
#undef PROVIDE_AND_REQUIRE
  // local eliminate & last_reference
  // {
  //   IRNote_t note(COLOR_WHITE "Local Eliminate:");
  //   note.print(out, 0);
  //   bool first = true;
  //   for(auto it : this->last_reference)
  //     {
  // 	if(first) first = false;
  // 	std::stringstream ss;
  // 	ss << COLOR_CYAN <<it.first << COLOR_RED " -> ";
  // 	(*it.second)->print(ss, -1);
  // 	IRNote_t note(ss.str());
  // 	std::stringstream othss;
  // 	note.print(othss, 0);
  // 	std::string s = othss.str();
  // 	out<< s.substr(0, s.size() - 1);
  //     }
  //   if(first)
  //     {
  // 	std::stringstream ss;
  // 	ss<<COLOR_GREEN "NONE";			
  // 	IRNote_t note(ss.str());		
  // 	note.print(out, 0);			
  //     }
  // }
  for(auto it = this->begin;
      it != this->end;
      ++it)
    {
      (*it)->print(out, 2);
      // LEVEL 1
    }
  IRSep_t sp;
  sp.print(out, 0);
}
void basic_block_t::AddEdge(basic_block_t* from,
			    basic_block_t* to)
{
  from->out_edges.push_back(to);
  to  -> in_edges.push_back(from);

}

void basic_block_t::get_register_dependency()
{
  this->UEVar.clear();
  this->VarKill.clear();
  for(auto it = this->begin;
      it != this->end;
      ++it)
    {
      myAssert(typeid(*(IROper_t*)*it) == typeid(IROper_t), ASSERT_EXIT_CODE_64);
      auto& irop = *(IROper_t*)*it;
      myAssert(OPERATION_REGISTERS.count(irop.oper), ASSERT_EXIT_CODE_65);

#define PROCESS_DEFINE_REFERENCE(oper, action)		\
      if(oper.type == OPERAND_VAR &&			\
	 ((oper.name[0] == '%'&&oper.name[1] != '&') ||	\
		 (oper.name[0] == '@' &&		\
		  var_to_int(oper.name, ASSERT_EXIT_CODE_233)<4		\
		  ) || oper.name[0] == '~'))		\
	{						\
	  if(action == 1)				\
	    {						\
	      this->VarKill.insert(oper.name);		\
	    }						\
	  else						\
	    {						\
	      if(!this->VarKill.count(oper.name))	\
		{					\
		  this->UEVar.insert(oper.name);	\
		}					\
	    }						\
	}

      operand_t oper;
      int  action;

      // First, consider references
      oper = irop.ops1;
      action = OPERATION_REGISTERS.at(irop.oper)[1];
      PROCESS_DEFINE_REFERENCE(oper, action);

      oper = irop.ops2;
      action = OPERATION_REGISTERS.at(irop.oper)[2];
      PROCESS_DEFINE_REFERENCE(oper, action);

      oper = irop.ops3;
      action = OPERATION_REGISTERS.at(irop.oper)[3];
      PROCESS_DEFINE_REFERENCE(oper, action);

      // Then, consider defines(or reference too)
      oper = irop.dest;
      action = OPERATION_REGISTERS.at(irop.oper)[0];
      PROCESS_DEFINE_REFERENCE(oper, action);
      
#undef PROCESS_DEFINE_REFERENCE
    }
}

std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Killed_Position(IROper_t& irop)
{
  switch(irop.oper)
    {
    case OPER_STACK_ALLOC:
      {
	myAssert(irop.dest.rewrited_name != "",ASSERT_EXIT_CODE_123);
	myAssert(irop.dest.rewrited_name[0] == '^',ASSERT_EXIT_CODE_1);
	return
	  {false,
	   { {irop.dest.rewrited_name, &irop.dest.rewrited_name}  }
	  };
      }
    case OPER_LOAD:          case OPER_MOVE:
    case OPER_PHI_MOV:       case OPER_ADD:
    case OPER_SUB:           case OPER_MUL:
    case OPER_DIV:           case OPER_SHL:
    case OPER_SHR:           case OPER_SAR:
    case OPER_ADD_OFFSET:    case OPER_LOAD_ARG:
      {
	if(irop.dest.rewrited_name != "")
	  return
	    {true,
	     { {irop.dest.rewrited_name, &irop.dest.rewrited_name}  }
	    };
	else
	  {
	    return {true, {{}}};
	  }
      }
    case OPER_MOVEQ:    case OPER_MOVNE:
    case OPER_MOVGT:    case OPER_MOVLT:
    case OPER_MOVGE:    case OPER_MOVLE:
    case OPER_ADDLT:    case OPER_SHRGE:
    case OPER_SHRLT:    case OPER_SARGE:
    case OPER_SARLT:
      {
	myAssert(irop.dest.rewrited_name != "",ASSERT_EXIT_CODE_2);
	return
	  {false,
	   { {irop.dest.rewrited_name, &irop.dest.rewrited_name}  }
	  };
	
      }
    case OPER_STORE:    case OPER_FUNC:
    case OPER_ENDF:     case OPER_RETN:
    case OPER_RET:      case OPER_JMP:
    case OPER_CMP:      case OPER_JEQ:
    case OPER_JNE:      case OPER_JGT:
    case OPER_JLT:      case OPER_JGE:
    case OPER_JLE:      case OPER_NOP:
    case OPER_LABEL:    case OPER_SPACE:
    case OPER_WORD:     case OPER_BLK_SEP:
      {
	return {true, {}};
      }

    case OPER_SET_ARG:
      {
	myAssert(irop.dest.get_runtime_type() == RUNTIME_IMM,ASSERT_EXIT_CODE_3);
	switch(irop.dest.imm)
	  {
	  case 0:
	    return {false, { {"#0", NULL} }};
	  case 1:
	    return {false, { {"#1", NULL} }};
	  case 2:
	    return {false, { {"#2", NULL} }};
	  case 3:
	    return {false, { {"#3", NULL} }};
	  default:
	    return {false, { } };
	  }
	myAssert(0,ASSERT_EXIT_CODE_4);
      }
    case OPER_CALL:    case OPER_CALL_NRET:
      {
      	return {false, {{"#0",NULL},{"#1",NULL},
			{"#2",NULL},{"#3",NULL}} };
      }
    }
  myAssert(0,ASSERT_EXIT_CODE_5); 
}

std::set<std::string> Get_Killed_vars(IROper_t& irop)
{
  std::set<std::string> retn;
  auto st = Get_Killed_Position(irop);
  for(auto it : st.second)
    {
      retn.insert(it.first);
    }
  return retn;
}

std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Referenced_Position(IROper_t& irop)
{
    switch(irop.oper)
    {
    case OPER_STACK_ALLOC:
      {
	return
	  {false, {}};
      }
    case OPER_LOAD:          case OPER_MOVE:
    case OPER_PHI_MOV:       case OPER_ADD:
    case OPER_SUB:           case OPER_MUL:
    case OPER_DIV:           case OPER_SHL:
    case OPER_SHR:           case OPER_SAR:
    case OPER_MOVEQ:         case OPER_MOVNE:
    case OPER_MOVGT:         case OPER_MOVLT:
    case OPER_MOVGE:         case OPER_MOVLE:
    case OPER_ADDLT:         case OPER_SHRGE:
    case OPER_SHRLT:         case OPER_SARGE:
    case OPER_SARLT:         case OPER_ADD_OFFSET:
    case OPER_SET_ARG:       case OPER_LOAD_ARG:      
      {
	std::vector<std::pair<std::string, std::string*>> ve;
#define CHECK_REWRITE(oper)						\
	if(oper.rewrited_name != "")					\
	  {								\
	    ve.push_back({oper.rewrited_name, &oper.rewrited_name});	\
	  }
	CHECK_REWRITE(irop.ops1);
	CHECK_REWRITE(irop.ops2);
	CHECK_REWRITE(irop.ops3);
	return {true, ve};
      }
    case OPER_STORE:    case OPER_RETN:
    case OPER_RET:      
    case OPER_CMP:      
      {
	std::vector<std::pair<std::string, std::string*>> ve;
	CHECK_REWRITE(irop.dest);
	CHECK_REWRITE(irop.ops1);
	CHECK_REWRITE(irop.ops2);
	CHECK_REWRITE(irop.ops3);
	return {true, ve};

      }
    case OPER_FUNC:    case OPER_ENDF:
    case OPER_JMP:     case OPER_JEQ:
    case OPER_JNE:      case OPER_JGT:
    case OPER_JLT:      case OPER_JGE:
    case OPER_JLE:      case OPER_NOP:
    case OPER_LABEL:    case OPER_SPACE:
    case OPER_WORD:     case OPER_BLK_SEP:
      {
	return {true, {}};
      }
    case OPER_CALL_NRET:
      {
	myAssert(argcount.count(irop.dest.name),ASSERT_EXIT_CODE_6);
	if(argcount[irop.dest.name] == 0)
	  return {false, {}};
	else if(argcount[irop.dest.name] == 1)
	  return {false, {{"#0",NULL}}};
	else if(argcount[irop.dest.name] == 2)
	  return {false, {{"#0",NULL},{"#1",NULL}}};
	else if(argcount[irop.dest.name] == 3)
	  return {false, {{"#0",NULL},{"#1",NULL},
			  {"#2",NULL}}};
	else
	  return {false, {{"#0",NULL},{"#1",NULL},
			  {"#2",NULL},{"#3",NULL}}};
      }

    case OPER_CALL:
      {
	myAssert(argcount.count(irop.ops1.name),ASSERT_EXIT_CODE_7);
	if(argcount[irop.ops1.name] == 0)
	  return {false, {}};
	else if(argcount[irop.ops1.name] == 1)
	  return {false, {{"#0",NULL}}};
	else if(argcount[irop.ops1.name] == 2)
	  return {false, {{"#0",NULL},{"#1",NULL}}};
	else if(argcount[irop.ops1.name] == 3)
	  return {false, {{"#0",NULL},{"#1",NULL},
			  {"#2",NULL}}};
	else
	  return {false, {{"#0",NULL},{"#1",NULL},
			  {"#2",NULL},{"#3",NULL}}};
      }
    }
    myAssert(0,ASSERT_EXIT_CODE_8);
#undef CHECK_REWRITE
}

std::set<std::string> Get_Referenced_vars(IROper_t& irop)
{
  std::set<std::string> retn;
  auto st = Get_Referenced_Position(irop);
  for(auto it : st.second)
    {
      retn.insert(it.first);
    }
  return retn;

}

void basic_block_t::get_postalloc_reg_dependency()
{
  this->RegUEVar.clear();
  this->RegVarKill.clear();
  for(auto it = this->begin;
      it != this->end;
      ++it)
    {
      myAssert(typeid(*(IROper_t*)*it) == typeid(IROper_t),ASSERT_EXIT_CODE_9);
      auto& irop = *(IROper_t*)*it;
      auto killed = Get_Killed_vars(irop);
      auto referenced = Get_Referenced_vars(irop);
      for(auto it : referenced)
	{
	  if(!this->RegVarKill.count(it))
	    this->RegUEVar.insert(it);
	}
      for(auto it : killed)
	{
	  this->RegVarKill.insert(it);
	}
    }
}


bool basic_block_t::compute_liveout()
{
  std::set<std::string> newliveOut;
  for(auto it : this->out_edges)
    {
      auto negVarKill = mySetDifference(it->liveOut, it->VarKill);
      auto to_be_union = mySetUnion(it->UEVar, negVarKill);
      newliveOut = mySetUnion(newliveOut, to_be_union);
    }
  if(newliveOut == this->liveOut)
    {
      return false; // Not modified
    }
  else
    {
      this->liveOut = newliveOut;
      return true; // Modified
    }
}

bool basic_block_t::compute_Regliveout()
{
  std::set<std::string> newliveOut;
  for(auto it : this->out_edges)
    {
      auto negVarKill = mySetDifference(it->RegliveOut, it->RegVarKill);
      auto to_be_union = mySetUnion(it->RegUEVar, negVarKill);
      newliveOut = mySetUnion(newliveOut, to_be_union);
    }
  if(newliveOut == this->RegliveOut)
    {
      return false; // Not modified
    }
  else
    {
      this->RegliveOut = newliveOut;
      return true; // Modified
    }
}

void basic_block_t::Finalize_RegliveIn()
{
  this->RegliveIn = mySetUnion(this->RegUEVar,
			    mySetDifference(this->RegliveOut, this->RegVarKill));
}

///////////////////////////////////////////////
///////I N T E R F E N C E/////////////////////
//////////////G R A P H////////////////////////
///////////////////////////////////////////////

// void interfence_graph_t::AddEdge(std::string u,
// 				 std::string v)
// {
//   if(Edge[u].count(v))
//     {
//       myAssert(Edge[v].count(u), ASSERT_EXIT_CODE_66);
//       return;
//     }
//   Edge[u].insert(v);
//   Edge[v].insert(u);
//   printf("AddEdge: %s -> %s\n",u.c_str(), v.c_str());
// }




void asm_context_t::make_ASM(std::list<IR_t *>::iterator begin,
			   std::list<IR_t *>::iterator end,
			     std::list<instr_t*>& InstrList, bool is_fake)
{
  // Before all, output license(hah) and arch
  std::stringstream ss;
  ss << R"(
#######################################################
###Generated by PlainSyc, a plain SysC Compiler########
##### presented by bzq @ hust##########################
#######################################################
.arch armv8-a
.arm
.fpu vfp
.macro mov32I, reg, val
    movw \reg, #:lower16:\val
    movt \reg, #:upper16:\val
.endm
)";
  InstrList.push_back(new instr_t(MNEMO_RAW, operand_t(ss.str())));

  auto fp_begin = begin;
  bool in_func = false;
  for(auto p = begin;
      p != end;
      ++p)
    {
      auto& ptr = *p;
      if(typeid(*ptr) == typeid(IRData_t))
      	{
	  data_output(*(IRData_t*)ptr, InstrList);
      	}
      else if(typeid(*ptr) == typeid(IRLabel_t))
	{
	  label_output(*(IRLabel_t*)ptr, InstrList);
	}
      else if(typeid(*ptr) == typeid(IROper_t))
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
	      this->make_func_ASM(fp_begin, p, InstrList, is_fake);
	      in_func = false;
	    }
	  else if(op.oper == OPER_WORD || op.oper == OPER_SPACE)
	    {
	      oper_output(op, InstrList);
	    }
	  else
	    {
	      continue;
	    }
	}
      else if(typeid(*ptr) == typeid(IRNote_t))
	{
	  if(in_func == false)
	    note_output(*(IRNote_t*)ptr, InstrList);
	}
      else
	{
	  myAssert(0, ASSERT_EXIT_CODE_68);
	}
    }
  return;
}

void asm_context_t::make_func_ASM(std::list<IR_t*>::iterator begin,
				  std::list<IR_t*>::iterator end,
				  std::list<instr_t*>& InstrList, bool is_fake)

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


//     // Pass 2.3, Find Local Variable as (LiveIn[Blk] + Provides[Blk]) - (LiveOut[Blk])
//     // Those variables is Eliminated in current basicblock
//     for(auto p = blks.begin();
//     	p != blks.end();
//     	++p)
//       {
//     	(*p)->local_eliminated_variables =
//     	  mySetDifference(mySetUnion((*p)->liveIn, (*p)->provide_variables), (*p)->liveOut);
//       }


//     // Pass 2.4, Find locale eliminated points
//     for(auto p = blks.begin();
// 	p != blks.end();
// 	++p)
//       {
// 	std::set<std::string> variables_to_eliminate = (*p)->local_eliminated_variables;
// 	auto q = (*p)->end;
// 	q--;
// 	// Each block has at least one instruction, hence we can decrease the iterator
// 	while(!variables_to_eliminate.empty())
// 	  {
// 	    myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_79);
// 	    auto& irop = *(IROper_t*)(*q);

// #define FIND_REF(oper)							\
// 	    {								\
// 	      if(oper.type == OPERAND_VAR &&				\
// 		 oper.name[0] == '%')					\
// 		{							\
// 		  if(variables_to_eliminate.count(oper.name))		\
// 		    {							\
// 		      (*p)->last_reference.insert({oper.name, q});	\
// 		      variables_to_eliminate.erase(oper.name);		\
// 		    }							\
// 		}							\
// 	    }
// 	    FIND_REF(irop.dest);
// 	    FIND_REF(irop.ops1);
// 	    FIND_REF(irop.ops2);
// 	    FIND_REF(irop.ops3);
// #undef FIND_REF

// 	    if(q == (*p)->begin)
// 	      {
// 		if(!variables_to_eliminate.empty())
// 		    myAssert(0, ASSERT_EXIT_CODE_80); // Something wrong
// 	      }
// 	    else
// 	      {
// 		q--;
// 	      }
// 	  }
      // }
    
  }

  if(print_FlowGraph && !is_fake)
    {
      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  (*p)->print(std::cerr);
	  std::cerr<<std::endl<<std::endl;
	}
    }

  // Pass 3. Optimize on BasicBlock by DataFlow Analysis
  {
    // Pass 3.1, detach each block against [irs], it's no needed anymore
    for(auto it : blks)
      {
	std::list<IR_t*>* tmpList = new std::list<IR_t*>(it->begin, it->end);
	tmpList->push_back((IR_t*)new IROper_t(OPER_BLK_SEP));
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
    if(optimize_flag == 1)
      Flow_Optimize(blks);

    // // Pass 3.4, Reorganize the block using DFS 
    // std::stack<basic_block_t*> dfsS;
    // int cur_time = 1;
    // dfsS.push(*blks.begin());
    // std::set<int> inq;
    // inq.insert((*blks.begin())->id);

    // while(!dfsS.empty())
    //   {
    // 	basic_block_t* bp = dfsS.top();
    // 	dfsS.pop();
    // 	cur_time = bp->assign_timestamp(cur_time);
    // 	for(auto p = bp->out_edges.rbegin();
    // 	    p != bp->out_edges.rend();
    // 	    ++p)
    // 	  {
    // 	    if(inq.count((*p)->id))
    // 	      continue;
    // 	    dfsS.push(*p);
    // 	    inq.insert((*p)->id);
    // 	  }
    //   }

    // for(auto p = blks.begin();
    // 	p != blks.end();
    // 	++p)
    //   {
    // 	(*p)->print(std::cerr);
    //   }

  }


  if(print_optFlowGraph && !is_fake)
    {
      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  (*p)->print(std::cerr);
	  std::cerr<<std::endl<<std::endl;
	}
    }
  
  // Pass 4. Register Allocation using [Linear Scan Register Allocation]
  //   Assume LiveRange for a variable %k is itself. (No Split)
  //   Reference: [Linear Scan Register Allocation,
  //                  Massimiliano Poletto & Vivek Sarkar,
  //                  ACM Trans on PL and Systems, 1999]

  register_ctx_t regctx;
  // [regctx] shall be used in Pass 5.

  {

    // Pass 4.1, Allocate timestamp for each basic block,
    // using DFS, hah
    int cur_time = 1;
    for(auto p = blks.begin();
	p != blks.end();
	++p)
    {
      cur_time = (*p)->assign_timestamp(cur_time);
    }
    
    // Pass 4.1+[Optimize], Find load targets and [bind samely]
    for(auto p = blks.begin(); p != blks.end(); ++p)
	for(auto q = (*p)->begin; q != (*p)->end; q++)
	  {
	    myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_81);
	    auto& irop = *(IROper_t*)(*q);
	    if(irop.oper == OPER_LOAD_ARG)
	      {
		myAssert(irop.ops1.type == OPERAND_VAR, ASSERT_EXIT_CODE_82);
		myAssert(irop.ops1.name[0] == '@', ASSERT_EXIT_CODE_83);
		regctx.loadarg_target.insert({irop.dest.name, var_to_int(irop.ops1.name, ASSERT_EXIT_CODE_234)});
	      }
	    if(irop.oper == OPER_CALL)
	      {
		myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_84);
		myAssert(irop.ops1.type == OPERAND_VAR, ASSERT_EXIT_CODE_85);
		myAssert(irop.dest.name[0] == '%', ASSERT_EXIT_CODE_86);
		if(irop.ops1.name == "__aeabi_idivmod")
		  regctx.loadarg_target.insert({irop.dest.name, 1});
		else
		  regctx.loadarg_target.insert({irop.dest.name, 0});
		// Return value is saved in r0
	      }
	  }
    
    // Pass 4.2, Collect each registers' latest and first reference time
    std::set<std::string> defined;
    for(auto p = blks.begin(); p != blks.end(); ++p)
	for(auto q = (*p)->begin; q != (*p)->end; q++)
	  {
	    myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_87);
	    auto& irop = *(IROper_t*)(*q);

#define LATEST(oper)							\
	    {								\
									\
	      if(oper.type == OPERAND_VAR &&				\
		 ((oper.name[0] == '%'&&oper.name[1] != '&') ||		\
		  (oper.name[0] == '@' &&				\
		   var_to_int(oper.name, ASSERT_EXIT_CODE_235)<4	\
		   ) || oper.name[0] == '~'))				\
		{							\
		  if(regctx.latest.count(oper.name))			\
		    {							\
		      regctx.latest[oper.name] =			\
			std::max(regctx.latest[oper.name],		\
				 irop.timestamp);			\
		    }							\
		  else							\
		    {							\
		      regctx.latest.insert({oper.name, irop.timestamp}); \
		    }							\
		}							\
	    }
	    LATEST(irop.dest);
	    LATEST(irop.ops1);
	    LATEST(irop.ops2);
	    LATEST(irop.ops3);
#undef LATEST
#define FIRSTT(oper)						\
	    if(oper.type == OPERAND_VAR &&			\
	       ((oper.name[0] == '%'&&oper.name[1] != '&') ||	\
		(oper.name[0] == '@' &&				\
		 var_to_int(oper.name, ASSERT_EXIT_CODE_236)<4			\
		 ) || oper.name[0] == '~'))			\
	      {							\
		if(!defined.count(oper.name))			\
		  {						\
		    defined.insert(oper.name);			\
		    regctx.time_to_var.insert({irop.timestamp,	\
			  {oper.name,{q, *p}}});		\
		    regctx.var_to_blk.insert({oper.name, *p});	\
		  }						\
	      }
	    FIRSTT(irop.dest);
	    FIRSTT(irop.ops1);
	    FIRSTT(irop.ops2);
	    FIRSTT(irop.ops3);
#undef FIRSTT
	  }



    // Pass 4.3, Calculate UNION [BBk+1, ..., BBn]
    //           to find if a variable can be expired
    {
      std::set<std::string> current_liveOut;
      for(auto p = blks.rbegin();
	  p != blks.rend();
	  ++p)
	{
	  regctx.post_liveOut.insert({*p, current_liveOut});
	  current_liveOut = mySetUnion(current_liveOut, (*p)->liveOut);
	}
    }

    // Pass 4.4, Check if there exists a function call
    //           for each variable's live range
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


	    // printf("LiveNow:");
	    // 	for(auto it : liveNow)
	    // 	  {
	    // 	    std::cout<<it<<" ";
	    // 	  }
	    // 	std::cout<<"\n";
	    // Remove Define from LiveNow
	    if(OPERATION_REGISTERS.at(irop.oper)[0] == 1 &&
	       irop.dest.type == OPERAND_VAR &&
	       liveNow.count(irop.dest.name))
	      liveNow.erase(irop.dest.name);

	    if(irop.oper == OPER_CALL || irop.oper == OPER_CALL_NRET)
	      {
		// printf("LiveNow ADDS:");
		// for(auto it : liveNow)
		//   {
		//     std::cout<<it<<" ";
		//   }
		// std::cout<<"\n";
		for(auto it : liveNow)
		  {
		    regctx.has_function_call.insert(it);
		  }
	      }
	    if(irop.oper == OPER_SET_ARG)
	      {
		myAssert(irop.dest.type == OPERAND_IMM, ASSERT_EXIT_CODE_89);
		if(irop.dest.imm < 4 && irop.ops1.type == OPERAND_VAR)
		  // r0 - r3
		  {
		    // printf("Hit Insert for %s -> max = %d\n", irop.ops1.name.c_str(), irop.dest.imm);
		    regctx.max_regcnt.insert({irop.ops1.name, irop.dest.imm});
		  }
	      }
	    // Insert Reference into LiveNow
#define INSERT_TO_LIVENOW(ops)			\
	    if(ops.type == OPERAND_VAR)		\
	      {					\
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
    // Pass 4.5, Allocate Register for Arguments
    // Before all, allocate #0, #1, #2 and #3 to @0, @1, @2 and @3
    for(auto it : regctx.time_to_var)
      {
	int             var_ts   = it.first;
	std::string     var_name = it.second.first;
	auto            var_ir   = it.second.second.first;
	basic_block_t*  var_blk  = it.second.second.second;

	if(var_name[0] == '@') // Arguments
	  {
	    regctx.occupy_reg(var_to_int(var_name, ASSERT_EXIT_CODE_237), var_name);
	  }
      }
    
    // Pass 4.6, Allocate Registers
    // We divide the allocation into two granularity: LiveRange level and Block level
    // For LOAD_ARG, we should try to assign correspond register, to create MOV #0 <- #0
    // and eliminate these assignments.
    //
    basic_block_t* previous_blk = NULL;
    for(auto it : regctx.time_to_var)
      {
	int             var_ts   = it.first;
	std::string     var_name = it.second.first;
	auto            var_ir   = it.second.second.first;
	basic_block_t*  var_blk  = it.second.second.second;
	regctx.expire_old_intervals(var_ts, var_blk);
	if(var_name[0] == '@') // Already allocated at Pass 4.5
	  {
	    continue;
	  }
	myAssert(var_name[0] == '%' || var_name[0] == '~', ASSERT_EXIT_CODE_90);
	if(previous_blk == NULL) // first-time
	  previous_blk = var_blk;
	else // Check boundary
	  {
	    if(previous_blk != var_blk) // reaches the block bondary
	      {
		previous_blk = var_blk;
		regctx.expire_end_of_blk(var_blk);
	      }
	  }

	// Allocate register
	int start = 0;
	if(regctx.has_function_call.count(var_name))
	  start = 4;
	int reg_idx = -1;
	if((reg_idx = regctx.find_free_reg(start, var_name)) != -1)
	  // Found a register
	  {
	    myAssert(reg_idx != -1, ASSERT_EXIT_CODE_91);
	    regctx.occupy_reg(reg_idx, var_name);
	  }
	else // Need to spill
	  {
	  _re_spill:
	    std::string spill_var = regctx.find_spill_var(start);
	    myAssert(regctx.latest.count(var_name), ASSERT_EXIT_CODE_92);
	    myAssert(regctx.latest.count(spill_var), ASSERT_EXIT_CODE_93);
	    if(phi_vars.count(var_name)) // Try to allocate a register
	      {
		myAssert(regctx.find_free_reg(start, var_name) == -1, ASSERT_EXIT_CODE_94);
		myAssert(regctx.active_name_to_reg.count(spill_var), ASSERT_EXIT_CODE_95);
		int spill_reg_idx = regctx.active_name_to_reg[spill_var];
		myAssert(spill_reg_idx >= start, ASSERT_EXIT_CODE_98);
		// printf("Spill %s -> %d\n", spill_var.c_str(), spill_reg_idx);
		regctx.spill_var(spill_var);
		reg_idx = regctx.find_free_reg(start, var_name);
		if(reg_idx < 0) // The Spilled var founded is INVALID
		  {
		    goto _re_spill;
		  }
		myAssert(reg_idx >= 0, ASSERT_EXIT_CODE_97);
		myAssert(reg_idx == spill_reg_idx, ASSERT_EXIT_CODE_96);
		regctx.occupy_reg(reg_idx, var_name);
	      }
	    else if(regctx.latest[var_name] < regctx.latest[spill_var])
	      // Spill the var founded
	      {
		myAssert(regctx.find_free_reg(start, var_name) == -1, ASSERT_EXIT_CODE_94);
		myAssert(regctx.active_name_to_reg.count(spill_var), ASSERT_EXIT_CODE_95);
		int spill_reg_idx = regctx.active_name_to_reg[spill_var];
		myAssert(spill_reg_idx >= start, ASSERT_EXIT_CODE_98);
		// printf("Spill %s -> %d\n", spill_var.c_str(), spill_reg_idx);
		regctx.spill_var(spill_var);
		reg_idx = regctx.find_free_reg(start, var_name);
		if(reg_idx < 0) // The Spilled var founded is INVALID
		  {
		    goto _re_spill;
		  }
		myAssert(reg_idx >= 0, ASSERT_EXIT_CODE_97);

		myAssert(reg_idx == spill_reg_idx, ASSERT_EXIT_CODE_96);
		regctx.occupy_reg(reg_idx, var_name);
	      }
	    else // Spill yourself
	      {
		regctx.occupy_mem(var_name);
	      }
	  }
      }
  }

  std::list<instr_t*>::iterator InstrListPosition = InstrList.end();
  // used in Post-PassScan
  
  // Pass 5. Generate ASM
  {
    // Pass 5.1, Calculate Procedures' Stack Size
    // Pass 5.1.1, Collect [CALL], [STACK_ALLOC] and [SET_ARG]
    //             1. If there is a [CALL] inside procedure,
    //                we should store [R14/LR]
    //             2. If there is a [STACK_ALLOC], we should reserve
    //                stack for local array, and rename to correspond offset
    //             3. If there is a [SET_ARG], we should reserve
    //                stack for argument passing

    bool has_function_call = false;
    bool first_instruction = true;
    // Expose [has_function_call] to below Pass 5.3
    {
      int& stack_lr_size  = regctx.stack_lr_size;
      int& stack_var_size = regctx.stack_var_size;
      int& stack_arg_size = regctx.stack_arg_size;
    
      myAssert(stack_lr_size == 0, ASSERT_EXIT_CODE_97);
      myAssert(stack_arg_size == 0, ASSERT_EXIT_CODE_98);
      // CodeGen Stage shouldn't modify these, only use [stack_var_size]
      if(regctx.r14_used)
	stack_lr_size = 4;
      
      for(auto p = blks.begin(); p != blks.end(); ++p)
	for(auto q = (*p)->begin; q != (*p)->end; q++)
	  {
	    myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_99);
	    auto irop = *(IROper_t*)(*q);
	    if(irop.oper == OPER_STACK_ALLOC)
	      {
		myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_100);
		myAssert(irop.ops1.type == OPERAND_IMM, ASSERT_EXIT_CODE_101);
		myAssert(irop.dest.name[0] == '%', ASSERT_EXIT_CODE_102);
		myAssert(irop.dest.name[1] == '&', ASSERT_EXIT_CODE_103);
		// update regctx's correspondence
		// regctx.local_array_mem_table.insert({irop.dest.name, "^"+std::to_string(regctx.stack_var_size)});
		regctx.var_alloc_table.insert({irop.dest.name, "^"+std::to_string(regctx.stack_var_size)});
		regctx.stack_var_size += irop.ops1.imm;
		continue;
	      }
	    if(irop.oper == OPER_CALL || irop.oper == OPER_CALL_NRET)
	      {
		stack_lr_size = 4;
		has_function_call = true;
	      }
	    if(irop.oper == OPER_SET_ARG)
	      {
		myAssert(irop.dest.type == OPERAND_IMM, ASSERT_EXIT_CODE_104);
		stack_arg_size = std::max(stack_arg_size,
					  (irop.dest.imm - 3) * 4); // #0-#3 is in reg
	      }
	    
	  }
    }
    // Pass 5.1.2, Collect Memory Guard
    // Guard non-volatile registers #4-#max_regs
    {
      for(int i = 4; i < regctx.max_regs; i++)
	{
	  if(regctx.__touched[i]) // [i] is modified
	    {
	      regctx.guard_map.insert({i, regctx.stack_guard_size});
	      regctx.stack_guard_size += 4;
	    }
	}
      // Guard [r11] (might not needed).
      regctx.guard_map.insert({11, regctx.stack_guard_size});
      regctx.stack_guard_size += 4;
      // The guarding memory offset is always of ascending order
    }


    int stack_arg_base     = 0;
    int stack_var_base     = regctx.stack_arg_size;
    int stack_guard_base   = stack_var_base + regctx.stack_var_size;
    int stack_lr_base      = stack_guard_base + regctx.stack_guard_size;
    int stack_passarg_base = stack_lr_base + regctx.stack_lr_size;
    
    // Pass 5.2, Write changes to IR
    for(auto p = blks.begin(); p != blks.end(); ++p)
      for(auto q = (*p)->begin; q != (*p)->end; q++)
	{
	  myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_105);
	  auto& irop = *(IROper_t*)(*q);
#define REWRITE_NAME(oper)						\
	  if(oper.type == OPERAND_VAR &&				\
	     regctx.var_alloc_table.count(oper.name))			\
	    {								\
	      oper.rewrited_name = regctx.var_alloc_table[oper.name];	\
	    }
	  REWRITE_NAME(irop.dest);
	  REWRITE_NAME(irop.ops1);
	  REWRITE_NAME(irop.ops2);
	  REWRITE_NAME(irop.ops3);
#undef REWRITE_NAME
	  // Special Treat for [Arguments]
	  if(irop.ops1.name[0] == '@' &&
	     irop.ops1.rewrited_name == "")
	    {
	      
	      auto& source_oper = irop.ops1;
	      myAssert(isdigit(source_oper.name[1]), ASSERT_EXIT_CODE_106);
	      int index = var_to_int(source_oper.name, ASSERT_EXIT_CODE_238);
	      myAssert(index > 3, ASSERT_EXIT_CODE_107);
	      int offset = stack_passarg_base + 4*(index-4) - stack_var_base;
	      // Not so good...
	      source_oper.rewrited_name = "^"+std::to_string(offset);
	    }
	}


    { // Pass 5.2+, Validation Check Pass
      if(print_Register && !is_fake)
	{
	  for(auto p = blks.begin();
	      p != blks.end();
	      ++p)
	    {
	      (*p)->print(std::cerr);
	      std::cerr<<std::endl<<std::endl;
	    }
	}
    }

    if(optimize_flag == 1)
      { // Pass 5.2++, Reorganize Register structure, try to allocate more registers
	PostAlloc_Flow_Optimize(blks);
      }
    // Pass 5.3, Translate into instruction flow
    {
      
      for(auto p = blks.begin(); p != blks.end(); ++p)
	for(auto q = (*p)->begin; q != (*p)->end; q++)
	  {
	    myAssert(typeid(*(IROper_t*)(*q)) == typeid(IROper_t), ASSERT_EXIT_CODE_108);
	    auto& irop = *(IROper_t*)(*q);
	    {
	      std::stringstream ss;
	      irop.print(ss, -1);
	      std::string t = ss.str();
	      t = t.substr(0, t.size() - 1);
	      InstrList.push_back(new instr_t(MNEMO_NOTE, t));
	    }

	    switch(irop.oper)
	      {
	      case OPER_FUNC: // Start-of-func
		{
		  // Define Symbol
		  std::stringstream ss;
		  ss << ".text"   << std::endl
		     << ".global " << irop.dest.name << std::endl
		     << ".type "  << irop.dest.name << ", %function" << std::endl
		     << irop.dest.name << ":";
		  InstrList.push_back(new instr_t(MNEMO_RAW, operand_t(ss.str())));
		  myAssert(first_instruction == true,ASSERT_EXIT_CODE_10);
		  first_instruction = false;
		  InstrListPosition = std::prev(InstrList.end());
		  // Reserve Stack
		  int stack_size =
		    regctx.stack_lr_size +
		    regctx.stack_guard_size +
		    regctx.stack_var_size +
		    regctx.stack_arg_size;

		  
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  "LR Size = " COLOR_CYAN + std::to_string(regctx.stack_lr_size)));
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  "Guard Size = " COLOR_CYAN + std::to_string(regctx.stack_guard_size)));
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  "VarAlloc Size = " COLOR_CYAN + std::to_string(regctx.stack_var_size)));
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  "SetArg Size = " COLOR_CYAN + std::to_string(regctx.stack_arg_size)));
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  "Total Size = " COLOR_CYAN + std::to_string(stack_size)));
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  std::string("Stack StorePoint")));
		  InstrList.push_back(new instr_t(MNEMO_PASSNOTE,
						  std::string("SP_SP " + std::to_string(stack_size))));
		  
						  
		  // Guard Variables
		  // LR
		  if(has_function_call || regctx.r14_used)
		    {
		      InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("Guard " COLOR_CYAN "LR")));
		      AsmGenerator::Store_Stack(operand_t("lr"), stack_lr_base, regctx, InstrList);
		    }
		  // Other Regular Registers

		  if(regctx.guard_map.size() < 4)
		    // Fastcall
		    {
		      for(auto it : regctx.guard_map)
			{
			  if(it.first == 11) // Special treat r11
			    {
			      InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("R11 Guardpoint")));
			      InstrList.push_back(new instr_t(MNEMO_PASSNOTE, std::string("R11_GP " + std::to_string(stack_guard_base + it.second))));
			      continue;
			    }
			  InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("Guard " COLOR_CYAN "r" + std::to_string(it.first))));
			  AsmGenerator::Store_Stack(operand_t("r"+std::to_string(it.first)), stack_guard_base + it.second, regctx, InstrList);
			}
		    }
		  else
		    // Ordinary Call
		    { // [r11] is directly stored and restored, not putting into guardpoint
		      // First, calculate offset, store in r12
		      if(AsmGenerator::check_fit_12(stack_guard_base))
			{
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  stack_guard_base));
			  // [r12] = [sp] + Offset
			}
		      else
			{
			  // First, load this immediate into [r12]
			  AsmGenerator::Load_Reg("r12", stack_guard_base, regctx, InstrList);

			  // Then, Calculate (r12+sp)
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  std::string("r12")));
			}

		      // Then, use STM to guard the variables into correspond location
		      std::string guards = "{";
		      bool first = true;
		      for(auto it : regctx.guard_map)
			{
			  if(first)
			    {
			      first = false;
			    }
			  else
			    {
			      guards += ",";
			    }
			  guards += "r" + std::to_string(it.first);
			}
		      guards += "}";
		      InstrList.push_back(new instr_t(MNEMO_STM,std::string("r12"),guards));
		    }
		  
		  InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("Function " COLOR_PURPLE +
									  irop.dest.name +
									  COLOR_GREEN "'s Body")));
		  // myAssert(0, ASSERT_EXIT_CODE_109);
		  break;
		}
	      case OPER_LOAD_ARG:  // Load Argument
	      case OPER_MOVE:      // Move
	      case OPER_PHI_MOV:   // PHI Move
	      case OPER_MOVEQ:     // Conditional Moves
	      case OPER_MOVNE:
	      case OPER_MOVGT:
	      case OPER_MOVLT:
	      case OPER_MOVGE:
	      case OPER_MOVLE:
		{
		  // Special Case: If Source and Target is the same, no need to move,
		  // Only valid for local variables

		  if(irop.dest.get_runtime_type() == RUNTIME_LOCAL_REG_VAR &&
		     irop.ops1.get_runtime_type() == RUNTIME_LOCAL_REG_VAR)
		    {
		      myAssert(irop.dest.rewrited_name != "",ASSERT_EXIT_CODE_11);
		      myAssert(irop.ops1.rewrited_name != "",ASSERT_EXIT_CODE_12);
		      if(irop.dest.rewrited_name == irop.ops1.rewrited_name)
		  	{
		  	  break;
		  	}
		    }
		  if(irop.dest.get_runtime_type() == RUNTIME_LOCAL_STACK_VAR &&
		     irop.ops1.get_runtime_type() == RUNTIME_LOCAL_STACK_VAR)
		    {
		      myAssert(irop.dest.rewrited_name != "",ASSERT_EXIT_CODE_13);
		      myAssert(irop.ops1.rewrited_name != "",ASSERT_EXIT_CODE_14);
		      if(irop.dest.rewrited_name == irop.ops1.rewrited_name)
		  	{
		  	  break;
		  	}
		    }

		  myAssert(irop.dest.name != "r11", ASSERT_EXIT_CODE_110);
		  myAssert(irop.dest.name != "r12", ASSERT_EXIT_CODE_111);
		  if(irop.dest.type != OPERAND_VAR)
		    {
		      printf("Assertion Failed at:");
		      irop.print(std::cerr, -1);
		    }
		  myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_112);
		  // Target might be [Register]/ [Stack Offset]/ [Global Array]/ [Global Var]
		  std::map<OPERATION_TYPE, std::string> condition_strs =
		    {
		      {OPER_LOAD_ARG, ""},
		      {OPER_MOVE, ""},
		      {OPER_PHI_MOV, ""},
		      {OPER_MOVEQ, "EQ"},
		      {OPER_MOVNE, "NE"},
		      {OPER_MOVGT, "GT"},
		      {OPER_MOVLT, "LT"},
		      {OPER_MOVGE, "GE"},
		      {OPER_MOVLE, "LE"},
		    };
		  if(irop.dest.rewrited_name != "")
		    {
		      // Target is Register
		      if(irop.dest.rewrited_name[0] == '#')
			{
			  AsmGenerator::Load_Reg("r"+irop.dest.rewrited_name.substr(1),
						 irop.ops1,
						 regctx,
						 InstrList,
						 condition_strs[irop.oper]);
			  break;
			}

		      if(irop.dest.rewrited_name[0] == '^')
			// Target is Mem Reference
			{
			  AsmGenerator::Store_Stack(irop.ops1,
						    stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_239),
						    regctx, InstrList,condition_strs[irop.oper]);
			  break;
			}
		      myAssert(0, ASSERT_EXIT_CODE_113);
		    }
		  else // Not-Rewritten
		    {
		      myAssert(irop.dest.name[0] == '$', ASSERT_EXIT_CODE_114);
		      if(irop.dest.name[1] == '&')
			{
			  // Move into array is not allowed
			  myAssert(0, ASSERT_EXIT_CODE_115);
			  break;
			}
		      else
			{
			  // Target is global variable
			  AsmGenerator::Store_Globl_Var(irop.ops1,
							irop.dest.name,
							regctx,
							InstrList,
							condition_strs[irop.oper]);

			  break;
			}
		      myAssert(0, ASSERT_EXIT_CODE_116);
		    }
		  break;
		}

	      case OPER_SET_ARG:
		// [Argument] Might be [Reg], [MemRef],
		// [LocalArr, GloblArr] or [RefArr]
		{
		  myAssert(irop.dest.type == OPERAND_IMM, ASSERT_EXIT_CODE_117);
		  if(irop.ops1.type == OPERAND_IMM ||
		     (irop.ops1.type == OPERAND_VAR &&
		      irop.ops1.name[1] != '&' &&
		      irop.ops1.name[0] != '~')
		     )
		    {
		      if(irop.dest.imm < 4) // Register Pass
			{


			  AsmGenerator::Load_Reg("r"+std::to_string(irop.dest.imm),
						 irop.ops1, regctx, InstrList);
			}
		      else // Stack Pass
			{
			  AsmGenerator::Store_Stack(irop.ops1, stack_arg_base + 4*(irop.dest.imm - 4), regctx, InstrList);
			}
		      break;
		    }
		  else if(irop.ops1.name[0] == '~') // ArrayRef
		    {
		      myAssert(irop.ops1.rewrited_name != "", ASSERT_EXIT_CODE_118);
		      if(irop.ops1.rewrited_name[0] == '#') // Source is in Register
			{
			  if(irop.dest.imm < 4) // Register Pass
			    {
			      AsmGenerator::Load_Reg("r"+std::to_string(irop.dest.imm),
						     irop.ops1, regctx, InstrList);
			      break;
			    }
			  else // Stack Pass
			    {
			      AsmGenerator::Store_Stack(irop.ops1, stack_arg_base + 4*(irop.dest.imm - 4), regctx, InstrList);
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_119);
			}
		      if(irop.ops1.rewrited_name[0] == '^') // Source is in Memory
			{
			  bool r12_occ = false;
			  std::string ops1_target = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			  // Assure [ops1] is saved in register
			  if(irop.dest.imm < 4) // Register Pass
			    {


			      AsmGenerator::Load_Reg("r"+std::to_string(irop.dest.imm),
						     ops1_target, regctx, InstrList);
			      break;
			    }
			  else // Stack Pass
			    {
			      AsmGenerator::Store_Stack(ops1_target, stack_arg_base + 4*(irop.dest.imm - 4), regctx, InstrList);
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_120);
			}
		    }
		  else // Local or Global Array
		    {

		      if(irop.ops1.name[0] == '$')
			// Global Array
			{
			  if(irop.dest.imm < 4) // Register Pass
			    {
			      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							      "r"+std::to_string(irop.dest.imm),
							      irop.ops1.name));
			      break;
			    }
			  else // Stack Pass
			    {
			      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							      std::string("r12"),
							      irop.ops1.name));
			      AsmGenerator::Store_Stack(std::string("r12"), stack_arg_base + 4*(irop.dest.imm - 4), regctx, InstrList);
			      break;
			    }			  
			}
		      if(irop.ops1.name[0] == '%') // Local Array
			{
			  myAssert(irop.ops1.rewrited_name[0] == '^', ASSERT_EXIT_CODE_121);
			  std::string target_reg = "";
			  if(irop.dest.imm < 4) // Register Pass
			    target_reg = "r"+std::to_string(irop.dest.imm);
			  else // Stack Pass
			    target_reg = "r12";
			  if(target_reg == "r11")
			    regctx.r11_guarded = true;
			  
			  int offset = stack_var_base + var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_240);
			  if(AsmGenerator::check_fit_12(offset))
			    {
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_reg,
							      std::string("sp"),
							      offset));
			    }
			  else
			    // target_reg <- offset
			    // target_reg <- target_reg + sp
			    {
			      AsmGenerator::Load_Reg(target_reg, offset, regctx, InstrList);
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_reg,
							      std::string("sp"),
							      target_reg));
			    }

			  if(irop.dest.imm < 4) // Register Pass
			    {
			      myAssert(target_reg == "r"+std::to_string(irop.dest.imm), ASSERT_EXIT_CODE_122);
			      break;
			    }
			  else // Stack Pass
			    {
			      myAssert(target_reg == "r12", ASSERT_EXIT_CODE_123);
			      AsmGenerator::Store_Stack(std::string("r12"), stack_arg_base + 4*(irop.dest.imm - 4), regctx, InstrList);
			      break;
			    }			  
			  myAssert(0, ASSERT_EXIT_CODE_124);
			}
		      printf("Assertion Failed at:");
		      irop.print(std::cerr, -1);
		      myAssert(0, ASSERT_EXIT_CODE_125);
			
		    }
		  // myAssert(0, ASSERT_EXIT_CODE_126); // not tested yet
		  break;
		}
	      case OPER_STACK_ALLOC: // Allocate Stack for Arrays
		{
		  // Need to do nothing.
		    break;
		}
	      case OPER_STORE:
		{
		  // STORE [dest+ops1] <- ops2
		  // [dest] might be a register/ MemOffSet/ GloblArray/ RefArray/ LocalArr
		  // ops1 is always a immediate(NOT!)
		  // ops2 might be anything, MemOffset, GloblVar, 
		  // myAssert(irop.ops1.type == OPERAND_IMM, ASSERT_EXIT_CODE_127);
		  
		  // First, consider [dest]
		  if(irop.dest.rewrited_name != "")
		    {
		      // [dest] is Register, but we shouldn't
		      //    modify this register
		      // Store target address into r12
		      if(irop.dest.rewrited_name[0] == '#')
			{
			  if(irop.ops1.type == OPERAND_IMM)
			    // [ops1] is Immediate
			    {
			      if(AsmGenerator::check_fit_mem_offset(irop.ops1.imm))
				// fit STR's memory range
				{
				  AsmGenerator::Store_Register_Offset(irop.ops2,
								      "r"+irop.dest.rewrited_name.substr(1),
								      irop.ops1.imm,
								      regctx,
								      InstrList);
				  break;
				} 
			      else
				// not fit, need to load first
				{
				  AsmGenerator::Load_Reg("r12", irop.ops1.imm, regctx, InstrList);
				  // InstrList.push_back(new instr_t(MNEMO_ADD,
				  // 				  operand_t("r12"),
				  // 				  "r"+irop.dest.rewrited_name.substr(1),
				  // 				  operand_t("r12")));
				  AsmGenerator::Store_Register_RegOffset(irop.ops2, "r"+irop.dest.rewrited_name.substr(1),"r12" , regctx, InstrList);
				  break;
				}
			    }
			  else
			    // [ops1] is not Immediate
			    {
			      myAssert(irop.ops1.type == OPERAND_VAR, ASSERT_EXIT_CODE_128);
			      bool r12_occ = false;
			      std::string ops1_assure = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      if(ops1_assure == "r11")
				regctx.r11_guarded = true;
			      // AsmGenerator::Load_Reg("r12", irop.ops1, regctx, InstrList);
			      // InstrList.push_back(new instr_t(MNEMO_ADD,
			      // 				      operand_t("r12"),
			      // 				      "r"+irop.dest.rewrited_name.substr(1),
			      // 				      operand_t("r12")));
			      AsmGenerator::Store_Register_RegOffset(irop.ops2, "r"+irop.dest.rewrited_name.substr(1), ops1_assure, regctx, InstrList);
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_129);
			}
		      
		      if(irop.dest.name[1] == '&')
			// [dest] is a local Array
			{
			  myAssert(irop.dest.name[0] == '%', ASSERT_EXIT_CODE_130);
			  myAssert(irop.dest.rewrited_name[0] == '^', ASSERT_EXIT_CODE_131);
			  int offset = stack_var_base
			    + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_241)
			    + irop.ops1.imm;

			  if(irop.ops1.type == OPERAND_IMM)
			    // ops1 in Immediate, add addresses
			    {
			      AsmGenerator::Store_Stack(irop.ops2,
							offset,
							regctx, InstrList);
			      break;
			    }
			  else
			    // ops1 is Not Immediate
			    // dest is MemRef
			    {
			      if(AsmGenerator::check_fit_12(offset))
				{
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  offset));
				  // [r12] = [sp] + Offset
				}
			      else
				{
				  // First, load this immediate into [r12]
				  AsmGenerator::Load_Reg("r12", offset, regctx, InstrList);

				  // Then, Calculate (r12+sp)
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  std::string("r12")));
								  
				}
			      // load [ops2]'s value into "r11"
			      bool occ = true;
			      std::string ops1_reg = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, occ);
			      // AsmGenerator::Load_Reg("r11", irop.ops1, regctx, InstrList);
			      if(ops1_reg == "r11")
				regctx.r11_guarded = true;
			      // offset is saved in [r12]
			      // InstrList.push_back(new instr_t(MNEMO_ADD,
			      // 				      operand_t("r12"),
			      // 				      ops1_reg,
			      // 				      operand_t("r12")));
			      AsmGenerator::Store_Register_RegOffset(irop.ops2,"r12", ops1_reg, regctx, InstrList);
			      break;
			    }
			  
			}
		      if(irop.dest.name[0] == '~')
			// [dest] is a reference array
			{
			  myAssert(irop.dest.rewrited_name != "", ASSERT_EXIT_CODE_132);
			  bool r12_occ = false;
			  std::string dest_addr = AsmGenerator::Assure_Reg(irop.dest, regctx, InstrList, r12_occ);
			  myAssert(dest_addr != "r11", ASSERT_EXIT_CODE_133);
			  // ops1 is Immediate
			  if(irop.ops1.type == OPERAND_IMM)
			    {
			      if(AsmGenerator::check_fit_mem_offset(irop.ops1.imm))
				// fit STR's memory range
				{
				  AsmGenerator::Store_Register_Offset(irop.ops2,
								      dest_addr,
								      irop.ops1.imm,
								      regctx,
								      InstrList);
				  break;
				} 
			      else
				// not fit, need to load first
				{
				  std::string tmp_reg = r12_occ?"r11" : "r12";
				  AsmGenerator::Load_Reg(tmp_reg, irop.ops1.imm, regctx, InstrList);
				  if(tmp_reg == "r11")
				    regctx.r11_guarded = true;
				  // [r12] = [dest_addr] + [r11]
				  // InstrList.push_back(new instr_t(MNEMO_ADD,
				  // 				  operand_t("r12"),
				  // 				  dest_addr,
				  // 				  operand_t("r11")));
				  AsmGenerator::Store_Register_RegOffset(irop.ops2, dest_addr, tmp_reg, regctx, InstrList);
				  break;
				}
			    }
			  else
			    // [ops1] is NOT immediate
			    {
			      std::string ops1_reg = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      if(ops1_reg == "r11")
				regctx.r11_guarded = true;
			      // offset is saved in [r12]
			      // InstrList.push_back(new instr_t(MNEMO_ADD,
			      // 				      operand_t("r12"),
			      // 				      ops1_reg,
			      // 				      dest_addr));
			      AsmGenerator::Store_Register_RegOffset(irop.ops2, dest_addr, ops1_reg, regctx, InstrList);
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_134);
			  
			}		      
		      printf("myAssert(0, ASSERT_EXIT_CODE_135) occurred when trying to translate:");
		      irop.print(std::cerr, -1);
		      myAssert(0, ASSERT_EXIT_CODE_136);
		      // debug
		      if(irop.dest.rewrited_name[0] == '^')
			// [dest] is Mem Reference (GloblVar)
			{
			  if(irop.ops1.type == OPERAND_IMM)
			    // ops1 in Immediate
			    {
			      AsmGenerator::Store_Stack(irop.ops2,
							stack_var_base
							+ var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_242)
							+ irop.ops1.imm,
							regctx, InstrList);
			      break;
			    }
			  else
			    // ops1 is Not Immediate
			    // dest is MemRef
			    {
			      AsmGenerator::Load_Reg("r12", irop.dest, regctx, InstrList);
			      bool r12_occ = true;
			      std::string ops1_reg = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      if(ops1_reg == "r11")
				regctx.r11_guarded = true;
			      // offset is saved in [r12]
			      // InstrList.push_back(new instr_t(MNEMO_ADD,
			      // 				      operand_t("r12"),
			      // 				      operand_t("r11"),
			      // 				      operand_t("r12")));
			      AsmGenerator::Store_Register_RegOffset(irop.ops2, "r12", ops1_reg, regctx, InstrList);
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_137);
			}
		      myAssert(0, ASSERT_EXIT_CODE_138);
		    }
		  else // Not-Rewritten
		    {
		      myAssert(irop.dest.name[0] == '$', ASSERT_EXIT_CODE_139);
		      if(irop.dest.name[1] == '&')
			// [dest] is Global Array
			{
			  // Load Address into r12
			  InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							  operand_t("r12"),
							  irop.dest.name));
			  if(irop.ops1.type == OPERAND_IMM)
			    // op1 is Immediate
			    {
			      // Calculate real offset
			      if(AsmGenerator::check_fit_mem_offset(irop.ops1.imm))
				// fit [ADD]'s immediate field
				{
				  AsmGenerator::Store_Register_Offset(irop.ops2,
								      "r12",
								      irop.ops1.imm,
								      regctx,
								      InstrList);
				  break;
				}
			      else
				// not fit, need to load first
				// [r12] saved the array's address,
				// [r11] saved the offset
				{
				  std::string tmp_reg = "r11";
				  regctx.r11_guarded = true;
				  AsmGenerator::Load_Reg("r11", irop.ops1.imm, regctx, InstrList);
				  // InstrList.push_back(new instr_t(MNEMO_ADD,
				  // 				  operand_t("r12"),
				  // 				  operand_t("r11"),
				  // 				  operand_t("r12")));
				  AsmGenerator::Store_Register_RegOffset(irop.ops2, "r12", "r11", regctx, InstrList);
				  break;
				}
			    }
			  else
			    // op1 is Not Immediate
			    // [dest] is global array, offset is saved in [r12]
			    {
			      myAssert(irop.ops1.type == OPERAND_VAR, ASSERT_EXIT_CODE_140);
			      bool r12_occ = true;
			      std::string ops1_reg = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      if(ops1_reg == "r11")
				regctx.r11_guarded = true;
			      // InstrList.push_back(new instr_t(MNEMO_ADD,
			      // 				      operand_t("r12"),
			      // 				      operand_t("r11"),
			      // 				      operand_t("r12")));
			      AsmGenerator::Store_Register_RegOffset(irop.ops2, "r12", ops1_reg, regctx, InstrList);
			      break;
			      
			    }
			  myAssert(0, ASSERT_EXIT_CODE_141);
			}
		      else
			{
			  // global variable shouldn't use STORE
			  myAssert(0, ASSERT_EXIT_CODE_142);
			  break;
			}
		      myAssert(0, ASSERT_EXIT_CODE_143);
		    }
		  
		}
	      case OPER_LOAD:
		{
		  // Load DEST <- [ops1 + ops2]
		  // ops1 is always array (global or local)
		  // ops2 is always immediate(FALSE)
		  // ops2 can't be array
		  // DEST is always renamed.
		  myAssert(irop.dest.rewrited_name != "", ASSERT_EXIT_CODE_144);
		  // myAssert(irop.ops2.type == OPERAND_IMM, ASSERT_EXIT_CODE_145);
		  if(irop.ops2.name[0] == '$' || irop.ops2.name[0] == '%')
		    myAssert(irop.ops2.name[1] != '&', ASSERT_EXIT_CODE_146);

		  if(irop.ops1.rewrited_name == "") // Global Array
		    {
		      myAssert(irop.ops1.name[0] == '$' &&
			       irop.ops1.name[1] == '&', ASSERT_EXIT_CODE_147);
		      // [dest] is Register
		      if(irop.dest.rewrited_name[0] == '#')
			// [dest] is register
			// [ops1] is global array
			{
			  if(irop.ops2.type == OPERAND_IMM)
			    {
			      auto liter = "r"+irop.dest.rewrited_name.substr(1);
			      // load array's address into [dest]
			      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							      liter,
							      irop.ops1.name));
			      if(AsmGenerator::check_fit_mem_offset(irop.ops2.imm))
				// fit [LDR]'s immediate field
				{
				  InstrList.push_back(new instr_t(MNEMO_LDR,
								  liter,
								  "["+liter+",#"+std::to_string(irop.ops2.imm)+"]"
								  ));
				  break;
				}
			      else
				//not fit, need to load first
				{
				  std::string tmp_reg = (liter == "r12")?"r11":"r12";
				  if(tmp_reg == "r11")
				    {
				      regctx.r11_guarded = true;
				    }
			  
				  AsmGenerator::Load_Reg(tmp_reg, irop.ops2.imm, regctx, InstrList);
				  /*
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  liter,
								  liter,
								  tmp_reg));
				  */
				  InstrList.push_back(new instr_t(MNEMO_LDR,
								  liter,
								  "["+liter+","+tmp_reg+"]"
								  ));
				  break;
				}
			    }
			  else
			    // [dest] is register, [ops1] is global array, [ops2] is NOT Immediate
			    {
			      std::string liter = "r12";
			      // load array's address into "r12"
			      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							      liter,
							      irop.ops1.name));
			      // load OPS2 into register
			      std::string tmp_reg = (liter == "r12")?"r11":"r12";
			      if(tmp_reg == "r11")
				{
				  regctx.r11_guarded = true;
				}
			  
			      AsmGenerator::Load_Reg(tmp_reg, irop.ops2, regctx, InstrList);
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      liter,
							      liter,
							      tmp_reg));
			      */
			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      "r"+irop.dest.rewrited_name.substr(1),
							      "["+liter+","+tmp_reg+"]"
							      ));
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_148);
			}
		      

		      if(irop.dest.rewrited_name[0] == '^')
			// [dest] is Mem Reference,
			// [op1]  is global array
			{
			  auto liter = std::string("r12");
			  // load array's address into [dest]
			  InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
							  liter,
							  irop.ops1.name));
			  if(irop.ops2.type == OPERAND_IMM)
			    {
			      if(AsmGenerator::check_fit_mem_offset(irop.ops2.imm))
				// fit [LDR]'s immediate field
				{
				  InstrList.push_back(new instr_t(MNEMO_LDR,
								  liter,
								  "["+liter+",#"+std::to_string(irop.ops2.imm)+"]"
								  ));
				  AsmGenerator::Store_Stack(liter,stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_243), regctx, InstrList);
				  break;
				}
			      else
				//not fit, need to load first
				{
				  std::string tmp_reg = (liter == "r12")?"r11":"r12";
				  if(tmp_reg == "r11")
				    {
				      regctx.r11_guarded = true;
				    }
			  
				  AsmGenerator::Load_Reg(tmp_reg, irop.ops2.imm, regctx, InstrList);
				  /*
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  liter,
								  liter,
								  tmp_reg));
				  */
				  InstrList.push_back(new instr_t(MNEMO_LDR,
								  liter,
								  "["+liter+","+tmp_reg+"]"
								  ));
				  AsmGenerator::Store_Stack(liter,stack_var_base +  var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_244), regctx, InstrList);
				  break;
				}
			      myAssert(0, ASSERT_EXIT_CODE_149);
			    }
			  else
			    // [dest] is MemRef, [ops1] is (saved) in register "r12"
			    // [ops2] is NOT immediate
			    {
			      std::string tmp_reg = (liter == "r12")?"r11":"r12";
			      if(tmp_reg == "r11")
				{
				  regctx.r11_guarded = true;
				}
			      AsmGenerator::Load_Reg(tmp_reg, irop.ops2, regctx, InstrList);
			      // load [ops2] into tmp_reg
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      liter,
							      liter,
							      tmp_reg));
			      */
			      // calculate [ops1] + [ops2] into [liter]

			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      liter,
							      "["+liter+","+tmp_reg+"]"
							      ));
			      // load value into [liter]
			      
			      AsmGenerator::Store_Stack(liter,stack_var_base +  var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_245), regctx, InstrList);
			      // store [liter] into target memRef
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_150);
			}
		      myAssert(0, ASSERT_EXIT_CODE_151);
		    }
		  else if(irop.ops1.name[0] == '%' &&
			     irop.ops1.name[1] == '&')
		    { // [ops1] is [local] array
		      myAssert(irop.ops1.name[0] == '%' &&
			       irop.ops1.name[1] == '&', ASSERT_EXIT_CODE_152);
		      myAssert(irop.ops1.rewrited_name[0] == '^', ASSERT_EXIT_CODE_153);
		      
		      if(irop.dest.rewrited_name[0] == '#')
			// [dest] is register
			// [ops1] is MemRef (array location)
			// dest <- [op1 + op2]
			{
			  if(irop.ops2.type == OPERAND_IMM)
			    {
			      // Load target base into [dest]
			      int offset = var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_246)
				+ irop.ops2.imm;
			      operand_t ops11;
			      ops11.rewrited_name = "^" + std::to_string(offset);
			      AsmGenerator::Load_Reg("r"+irop.dest.rewrited_name.substr(1), ops11, regctx, InstrList);
			      break;
			    }
			  else
			    // [ops2] is NOT immediate
			    {

			      // load [ops1]'s address into "r12"
			      int offset = var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_247) + stack_var_base;
			      // real address is [sp+offset]
			      // Manual Load
			      if(AsmGenerator::check_fit_12(offset))
				{
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  offset));
				  // [r12] = [sp] + Offset
				}
			      else
				{
				  // First, load this immediate into [r12]
				  AsmGenerator::Load_Reg("r12", offset, regctx, InstrList);

				  // Then, Calculate (r12+sp)
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  std::string("r12")));
								  
				}
			      // load [ops2]'s value into "r11"
			      std::string target_liter = "r11";
			      // [ops2] can't be array
			      std::string reg2_source = target_liter;
			      if(irop.ops2.rewrited_name != "" &&
				 irop.ops2.rewrited_name[0] == '#') // [ops2] is register
				reg2_source = "r"+irop.ops2.rewrited_name.substr(1);
			      else // others
				{
				  AsmGenerator::Load_Reg(reg2_source, irop.ops2, regctx, InstrList);
				}
			      if(reg2_source == "r11")
				regctx.r11_guarded = true;

			      // Calculate [ops1+ops2] into [target_liter]
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_liter,
							      reg2_source,
							      std::string("r12")));
			      */
			      // Load address into [dest]
			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      "r"+irop.dest.rewrited_name.substr(1),
							      "["+reg2_source+","+"r12"+"]"
							      ));
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_154);
			}

		      if(irop.dest.rewrited_name[0] == '^')
			// [dest] is Mem Reference
			// [op1]  is MemRef
			{

			  if(irop.ops2.type == OPERAND_IMM)
			    {
			      // Load target base into [dest]
			      std::string liter = "r12";
			      int offset = var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_248)
				+ irop.ops2.imm;
			      operand_t ops11;
			      ops11.rewrited_name = "^" + std::to_string(offset);
			      AsmGenerator::Load_Reg(liter, ops11, regctx, InstrList);
			      AsmGenerator::Store_Stack(liter, stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_249), regctx, InstrList);
			      break;
			    }
			  else // [dest] is MemRef, [op1] is MemRef, [op2] is NOT Immediate
			    {

			      // load [ops1]'s address into "r12"
			      int offset = var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_250) + stack_var_base;
			      // real address is [sp+offset]
			      // Manual Load
			      if(AsmGenerator::check_fit_12(offset))
				{
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  offset));
				  // [r12] = [sp] + Offset
				}
			      else
				{
				  // First, load this immediate into [r12]
				  AsmGenerator::Load_Reg("r12", offset, regctx, InstrList);

				  // Then, Calculate (r12+sp)
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  std::string("r12"),
								  std::string("sp"),
								  std::string("r12")));
								  
				}
			      // load [ops1+ops2]'s value into "r11"
			      std::string target_liter = "r11";
			      // [ops2] can't be array
			      if(target_liter == "r11")
				regctx.r11_guarded = true;

			      std::string reg2_source = target_liter;
			      if(irop.ops2.rewrited_name != "" &&
				 irop.ops2.rewrited_name[0] == '#') // [ops2] is register
				reg2_source = "r"+irop.ops2.rewrited_name.substr(1);
			      else // others
				{
				  AsmGenerator::Load_Reg(reg2_source, irop.ops2, regctx, InstrList);
				}

			      // Calculate [ops1+ops2] into [target_liter]
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_liter,
							      reg2_source,
							      std::string("r12")));
			      */
			      // Load value into "r11"
			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      target_liter,
							      "["+reg2_source+","+"r12"+"]"
							      ));
			      AsmGenerator::Store_Stack(target_liter, stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_251), regctx, InstrList);			      
			      break;
			    }

			  myAssert(0, ASSERT_EXIT_CODE_155);
			}
		      myAssert(0, ASSERT_EXIT_CODE_156); 
		    }
		  else
		    { // [ops1] is [Reference] Array
		      // rewritten name might be [MemRef] or [Register]
		      myAssert(irop.ops1.name[0] == '~', ASSERT_EXIT_CODE_157);
		      myAssert(irop.ops1.rewrited_name != "", ASSERT_EXIT_CODE_158);

		      if(irop.dest.rewrited_name[0] == '#')
			// [dest] is register
			// [ops1] is referenced array(value is array location)
			// dest <- [op1 + op2]
			{
			  if(irop.ops2.type == OPERAND_IMM)
			    {
			      // register <- [ops1 + Immediate]
			      // load ops1 into dest
			      bool r12_occ = (irop.dest.rewrited_name == "#12")?true:false;
			      std::string ops1_target = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      if(ops1_target == "r11")
				regctx.r11_guarded = true;
			      
			      if(AsmGenerator::check_fit_mem_offset(irop.ops2.imm))
				{
				  AsmGenerator::Load_Memory_Offset(irop.dest, ops1_target, irop.ops2.imm, regctx, InstrList);
				  break;
				}
			      else // not fit, need to load
				{
				  std::string tmp_reg = ops1_target == "r12"?"r11":"r12";
				  if(tmp_reg == "r11")
				    regctx.r11_guarded = true;
				  AsmGenerator::Load_Reg(tmp_reg, irop.ops2.imm, regctx, InstrList);
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  operand_t("r12"),
								  tmp_reg,
								  ops1_target));
				  AsmGenerator::Load_Memory_Offset(irop.dest, "r12", 0, regctx, InstrList);
				  break;
				}
			    }
			  else
			    // [ops2] is NOT immediate
			    // register <- [ops1 + ops2]
			    {
			      // load [ops1]'s address into "r12"
			      AsmGenerator::Load_Reg("r12", irop.ops1, regctx, InstrList);
			      // load [ops2]'s value into "r11"
			      std::string target_liter = "r11";
			      // [ops2] can't be array

			      std::string reg2_source = target_liter;
			      if(irop.ops2.rewrited_name != "" &&
				 irop.ops2.rewrited_name[0] == '#') // [ops2] is register
				reg2_source = "r"+irop.ops2.rewrited_name.substr(1);
			      else // others
				{
				  AsmGenerator::Load_Reg(reg2_source, irop.ops2, regctx, InstrList);
				}

			      if(reg2_source == "r11")
				regctx.r11_guarded = true;
			      
			      // Calculate [ops1+ops2] into [target_liter]
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_liter,
							      reg2_source,
							      std::string("r12")));
			      */
			      // Load address into [dest]
			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      "r"+irop.dest.rewrited_name.substr(1),
							      "["+reg2_source+","+"r12"+"]"
							      ));
			      break;
			    }
			  myAssert(0, ASSERT_EXIT_CODE_159);
			}

		      if(irop.dest.rewrited_name[0] == '^')
			// [dest] is Mem Reference
			// [op1]  is [ArrayRef]
			{

			  if(irop.ops2.type == OPERAND_IMM)
			    // [MemRef] <- [ArrayRef + Immediate]
			    {
			      // load ops1 into ops1_target
			      bool r12_occ = false;
			      std::string ops1_target = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occ);
			      myAssert(ops1_target != "r11", ASSERT_EXIT_CODE_160);
			      std::string tmp_target = "r11";
			      regctx.r11_guarded = true;
			      
			      if(AsmGenerator::check_fit_mem_offset(irop.ops2.imm))
				{
				  AsmGenerator::Load_Memory_Offset(tmp_target, ops1_target, irop.ops2.imm, regctx, InstrList);
				  AsmGenerator::Store_Stack(tmp_target, stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_252), regctx, InstrList);
				  break;
				}
			      else // not fit, need to load
				{
				  AsmGenerator::Load_Reg(tmp_target, irop.ops2.imm, regctx, InstrList);
				  InstrList.push_back(new instr_t(MNEMO_ADD,
								  operand_t("r12"),
								  tmp_target,
								  ops1_target));
				  // Load value into [r12]
				  AsmGenerator::Load_Memory_Offset(operand_t("r12"), "r12", 0, regctx, InstrList);
				  // store
				  AsmGenerator::Store_Stack(operand_t("r12"), stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_253), regctx, InstrList);
				  break;
				}
			    }			    
			  else // [dest] is MemRef, [op1] is ArrayRef, [op2] is NOT Immediate
			    {

			      // load [ops1]'s address into "r12"
			      AsmGenerator::Load_Reg("r12", irop.ops1, regctx, InstrList);			
			      // load [ops1+ops2]'s value into "r11"
			      std::string target_liter = "r11";
			      // [ops2] can't be array
			      if(target_liter == "r11")
				regctx.r11_guarded = true;

			      std::string reg2_source = target_liter;
			      if(irop.ops2.rewrited_name != "" &&
				 irop.ops2.rewrited_name[0] == '#') // [ops2] is register
				reg2_source = "r"+irop.ops2.rewrited_name.substr(1);
			      else // others
				{
				  AsmGenerator::Load_Reg(reg2_source, irop.ops2, regctx, InstrList);
				}

			      // Calculate [ops1+ops2] into [target_liter]
			      /*
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      target_liter,
							      reg2_source,
							      std::string("r12")));
			      */
			      // Load value into "r11"
			      InstrList.push_back(new instr_t(MNEMO_LDR,
							      target_liter,
							      "["+reg2_source+","+"r12"+"]"
							      ));
			      AsmGenerator::Store_Stack(target_liter, stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_254), regctx, InstrList);			      
			      break;
			    }

			  myAssert(0, ASSERT_EXIT_CODE_161);
			}
		      myAssert(0, ASSERT_EXIT_CODE_162); 
		    }
		  break;
		}
	      case OPER_ADDLT:
	      case OPER_SHRGE:
	      case OPER_SHRLT:
	      case OPER_SARGE:
	      case OPER_SARLT:
	      case OPER_ADD:
	      case OPER_SUB:
	      case OPER_DIV:
	      case OPER_MUL:
	      case OPER_SHL:
	      case OPER_SHR:
	      case OPER_SAR:
		// Assembly only allow [Register] or [Immediate] in IMM12
		// [dest] might be [reg], [MemRef], CAN'T be [global_var], or [Immediate]
		// [ops1] might be [reg], [global_var], [MemRef], [Immediate], [GloblArr], [LocalArr]
		// [ops2] might be [reg], [global_var], [MemRef], [Immediate]
		// [ADD], [SUB] can accept an [IMM12] immediate value
		// [MUL] and [SDIV] do NOT accept [Immediate]
		// [SHL]/[SHR]/[SAR] (ASR, LSL, LSR) can accept an [IMM5] value
		{
		  // <[OPER], <[ALLOW_IMM2], checker>
		  std::map< OPERATION_TYPE, std::pair<bool, bool(*)(int)> > oper_property =
		    {
		      {OPER_ADD, {true, AsmGenerator::check_fit_12}},
		      {OPER_ADDLT, {true, AsmGenerator::check_fit_12}},
		      {OPER_SUB, {true, AsmGenerator::check_fit_12}},
		      {OPER_DIV, {false, NULL}},
		      {OPER_MUL, {false, NULL}},
		      {OPER_SHL, {true, AsmGenerator::check_fit_5}},
		      {OPER_SHR, {true, AsmGenerator::check_fit_5}},
		      {OPER_SAR, {true, AsmGenerator::check_fit_5}},
		      {OPER_SHRLT, {true, AsmGenerator::check_fit_5}},
		      {OPER_SHRGE, {true, AsmGenerator::check_fit_5}},
		      {OPER_SARLT, {true, AsmGenerator::check_fit_5}},
		      {OPER_SARGE, {true, AsmGenerator::check_fit_5}},
		    };
		  std::map< OPERATION_TYPE, MNEMONIC_TYPE > oper_to_mnemonic =
		    {
		      {OPER_ADD, MNEMO_ADD},
		      {OPER_ADDLT, MNEMO_ADD},
		      {OPER_SUB, MNEMO_SUB},
		      {OPER_DIV, MNEMO_SDIV},
		      {OPER_MUL, MNEMO_MUL},
		      {OPER_SHL, MNEMO_LSL},
		      {OPER_SHR, MNEMO_LSR},
		      {OPER_SHRLT, MNEMO_LSR},
		      {OPER_SHRGE, MNEMO_LSR},
		      {OPER_SARLT, MNEMO_ASR},
		      {OPER_SARGE, MNEMO_ASR},
		      {OPER_SAR, MNEMO_ASR},
		    };
		  std::map< OPERATION_TYPE, std::string > oper_to_cond =
		    {
		      {OPER_ADDLT, "LT"},
		      {OPER_SHRLT, "LT"},
		      {OPER_SHRGE, "GE"},
		      {OPER_SARLT, "LT"},
		      {OPER_SARGE, "GE"},
		    };		  
		  // If [OPER] is ADD, [ops1] is IMM, [ops2] is not IMM, swap [ops1] and [ops2]
		  if((irop.oper == OPER_ADD || irop.oper == OPER_ADDLT || irop.oper == OPER_MUL) && irop.ops1.type == OPERAND_IMM && irop.ops2.type != OPERAND_IMM)
		    {
		      std::swap(irop.ops1, irop.ops2);
		    }
		  // First, assure [ops1] is always in register
		  bool r12_occupied = false;
		  std::string op1 = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occupied);
		  std::string op2;
		  // Then, see if [ops2] can be accepted as Immediate
		  myAssert(oper_property.count(irop.oper), ASSERT_EXIT_CODE_163);
		  if(irop.ops2.type == OPERAND_IMM &&
		     oper_property[irop.oper].first == true &&
		     oper_property[irop.oper].second(irop.ops2.imm) == true)
		    // Can be used in Immediate Field
		    {
		      op2 = "#"+std::to_string(irop.ops2.imm);
		    }
		  else
		    { // Directly Load
		      op2 = AsmGenerator::Assure_Reg(irop.ops2, regctx, InstrList, r12_occupied);
		    }

		  // Then, consider [dest]
		  myAssert(irop.dest.rewrited_name != "", ASSERT_EXIT_CODE_164);
		  myAssert(oper_to_mnemonic.count(irop.oper),ASSERT_EXIT_CODE_15);
		  if(irop.dest.rewrited_name[0] == '#') // [dest] is Register
		    {
		      auto liter = "r"+irop.dest.rewrited_name.substr(1);
		      InstrList.push_back(new instr_t(oper_to_mnemonic[irop.oper], liter, op1, op2));
		      InstrList.back()->ref_count = irop.ref_count;
		      // printf("Pushing back RefCount = %d for ", irop.ref_count);
		      // irop.print(std::cerr, -1);
		      if(oper_to_cond.count(irop.oper))
			{
			  InstrList.back()->condition_string = oper_to_cond[irop.oper];
			}
		      break;
		    }
		  if(irop.dest.rewrited_name[0] == '^') // [dest] is MemRef
		    {
		      // r12 can be released, and need not to store original value
		      InstrList.push_back(new instr_t(oper_to_mnemonic[irop.oper], operand_t("r12"), op1, op2));
		      InstrList.back()->ref_count = irop.ref_count;
		      if(oper_to_cond.count(irop.oper))
			{
			  InstrList.back()->condition_string = oper_to_cond[irop.oper];
			}
		      AsmGenerator::Store_Stack(operand_t("r12"), stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_255), regctx, InstrList);
		      break;
		    }
		  myAssert(0, ASSERT_EXIT_CODE_165);
		  break;
		}
	      case OPER_ADD_OFFSET:
		// [dest] might be [MemRef] or [Register]
		// [ops1] is always Array (Global/ Local/ Reference)
		// [ops2] might be [Immediate], [GloblVar], [Register], [MemRef]
		{
		  // printf("OPS1=");
		  // irop.ops1.print(std::cerr);
		  myAssert(irop.ops1.get_runtime_type() == RUNTIME_GLOBL_ARR ||
			 irop.ops1.get_runtime_type() == RUNTIME_LOCAL_ARR ||
			   irop.ops1.get_runtime_type() == RUNTIME_REF_ARR, ASSERT_EXIT_CODE_166);
		  // Store [ops1]'s address in "r12"
		  if(irop.ops1.get_runtime_type() == RUNTIME_GLOBL_ARR)
		    {
		      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
						      std::string("r12"),
						      irop.ops1.name));
		    }
		  else if(irop.ops1.get_runtime_type() == RUNTIME_LOCAL_ARR)
		    {
		      int offset = var_to_int(irop.ops1.rewrited_name, ASSERT_EXIT_CODE_256) + stack_var_base;
		      if(AsmGenerator::check_fit_12(offset))
			{
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  offset));
			  // [r12] = [sp] + Offset
			}
		      else
			{
			  // First, load this immediate into [r12]
			  AsmGenerator::Load_Reg("r12", offset, regctx, InstrList);

			  // Then, Calculate (r12+sp)
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  std::string("r12")));
			}
		    }
		  else
		    {
		      myAssert(irop.ops1.get_runtime_type() == RUNTIME_REF_ARR, ASSERT_EXIT_CODE_167);
		      AsmGenerator::Load_Reg("r12", irop.ops1, regctx, InstrList);
		    }

		  // Then, Consider [dest]
		  if(irop.dest.get_runtime_type() == RUNTIME_LOCAL_REG_VAR)
		    {
		      if(irop.ops2.get_runtime_type() == RUNTIME_IMM)
			// [dest] is register
			// [ops2] is immediate
			{
			  if(AsmGenerator::check_fit_12(irop.ops2.imm))
			    // Can be used in Immediate Field
			    {
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      std::string("r"+irop.dest.rewrited_name.substr(1)),
							      std::string("r12"),
							      irop.ops2.imm));
			      break;
			    }
			  else
			    // First, load into [dest], then, add
			    {
			      auto liter =  std::string("r"+irop.dest.rewrited_name.substr(1));
			      AsmGenerator::Load_Reg(liter, irop.ops2.imm, regctx, InstrList);
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      liter,
							      std::string("r12"),
							      liter));
			      break;
			    }

			}
		      else
			// [dest] is register,
			// [ops2] is NOT immediate
			{
			  // store [ops2] in [ops2_src]
			  bool r12_used = true;
			  std::string ops2_src = AsmGenerator::Assure_Reg(irop.ops2, regctx, InstrList, r12_used);
			  // Add
			  auto liter =  std::string("r"+irop.dest.rewrited_name.substr(1));
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  liter,
							  std::string("r12"),
							  ops2_src));
			  break;
			  
			}
		    }

		  if(irop.dest.get_runtime_type() == RUNTIME_LOCAL_STACK_VAR)
		    {
		      // [dest] is stack, [ops1] is Array, but [ops2]'s rewrited name is Empty
		      // myAssert(irop.ops2.rewrited_name != "", ASSERT_EXIT_CODE_233);
		      
		      int offset = var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_257);
		      // [dest]'s var stack offset
		      regctx.r11_guarded = true;
		      if(irop.ops2.get_runtime_type() == RUNTIME_IMM)
			// [dest] is stack offset
			// [ops2] is immediate
			{

			  if(AsmGenerator::check_fit_12(irop.ops2.imm))
			    // Can be used in Immediate Field
			    {
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      std::string("r11"),
							      std::string("r12"),
							      irop.ops2.imm));
			      AsmGenerator::Store_Stack(std::string("r11"), stack_var_base + offset, regctx, InstrList);
			      break;
			    }
			  else
			    // First, load into "r11", then, add
			    {
			      auto liter =  std::string("r11");
			      AsmGenerator::Load_Reg(liter, irop.ops2.imm, regctx, InstrList);
			      InstrList.push_back(new instr_t(MNEMO_ADD,
							      liter,
							      std::string("r12"),
							      liter));
			      AsmGenerator::Store_Stack(std::string("r11"), stack_var_base + offset, regctx, InstrList);
			      break;
			    }

			}
		      else
			// [dest] is stack offset,
			// [ops2] is NOT immediate
			{
			  // store [ops2] in [ops2_src]
			  bool r12_used = true;
			  std::string ops2_src = AsmGenerator::Assure_Reg(irop.ops2, regctx, InstrList, r12_used);
			  // Add
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r11"),
							  std::string("r12"),
							  ops2_src));
			  AsmGenerator::Store_Stack(std::string("r11"), stack_var_base + offset, regctx, InstrList);
			  break;
			  
			}		      
		    }
		  myAssert(0, ASSERT_EXIT_CODE_168);
		}
	      case OPER_CMP:
		// Compare two numbers [dest] and [ops1]
		// [dest] might be [Register], [MemRef], [GloblVar]
		//        won't be [GloblArray], [Immediate]
		// [ops1] might be [Register], [MemRef], [GloblVar], [Immediate]
		//        won't be [GloblArray]
		{
		  myAssert(irop.dest.type != OPERAND_IMM, ASSERT_EXIT_CODE_169);
		  if(irop.dest.name[0] == '$')
		    myAssert(irop.dest.name[1] != '&', ASSERT_EXIT_CODE_170);
		  if(irop.ops1.name[0] == '$')
		    myAssert(irop.ops1.name[1] != '&', ASSERT_EXIT_CODE_171);
		  // First, assure [dest] is always in register
		  bool r12_occupied = false;
		  std::string op1 = AsmGenerator::Assure_Reg(irop.dest, regctx, InstrList, r12_occupied);
		  std::string op2;
		  // Then, see if [ops1] can be accepted as Immediate
		  if(irop.ops1.type == OPERAND_IMM &&
		     AsmGenerator::check_fit_12(irop.ops1.imm))
		    // Can be used in Immediate Field
		    {
		      op2 = "#"+std::to_string(irop.ops1.imm);
		    }
		  else
		    { // Directly Load
		      op2 = AsmGenerator::Assure_Reg(irop.ops1, regctx, InstrList, r12_occupied);
		    }
		  InstrList.push_back(new instr_t(MNEMO_CMP, op1, op2));		  
		  break;
		}

	      case OPER_JMP:
	      case OPER_JEQ:
	      case OPER_JNE:
	      case OPER_JGT:
	      case OPER_JLT:
	      case OPER_JGE:
	      case OPER_JLE:
		// JMP [dest->target]
		// [dest] is always [var/label type]
		{
		  std::map<OPERATION_TYPE, std::string> condition_strs =
		    {
		      {OPER_JMP, ""  },
		      {OPER_JEQ, "EQ"},
		      {OPER_JNE, "NE"},
		      {OPER_JGT, "GT"},
		      {OPER_JLT, "LT"},
		      {OPER_JGE, "GE"},
		      {OPER_JLE, "LE"},
		    };
		  InstrList.push_back(new instr_t(MNEMO_B, irop.dest));
		  InstrList.back()->condition_string = condition_strs[irop.oper];
		  break;
		}
	      case OPER_CALL_NRET:
		{
		  myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_172);
		  // myAssert(irop.dest.rewrited_name != "", ASSERT_EXIT_CODE_173);
		  // Special for [starttime]
		  if(irop.dest.name == "starttime")
		    {
		      irop.dest.name = "_sysy_starttime";
		      InstrList.push_back(new instr_t(MNEMO_MOV, std::string("r0"), 0));
		    }
		  if(irop.dest.name == "stoptime")
		    {
		      InstrList.push_back(new instr_t(MNEMO_MOV, std::string("r0"), 0));
		      irop.dest.name = "_sysy_stoptime";
		    }

		  InstrList.push_back(new instr_t(MNEMO_BL, irop.dest));
		  break;
		}
	      case OPER_CALL:
		// CALL [dest], [ops1]
		// [dest] might be [Regiser/MemRef], won't be [Imm/GloblVar/GloblArray]
		// [ops1] is always a label name
		{
		  myAssert(irop.dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_172);
		  myAssert(irop.dest.rewrited_name != "", ASSERT_EXIT_CODE_173);
		  // First, Call the function
		  // Special for [starttime]
		  if(irop.ops1.name == "starttime")
		    {
		      irop.ops1.name = "_sysy_starttime";
		      InstrList.push_back(new instr_t(MNEMO_MOV, std::string("r0"), 0));
		    }
		  if(irop.ops1.name == "stoptime")
		    {
		      irop.ops1.name = "_sysy_stoptime";
		      InstrList.push_back(new instr_t(MNEMO_MOV, std::string("r0"), 0));
		    }
		  InstrList.push_back(new instr_t(MNEMO_BL, irop.ops1));


		  if(irop.dest.rewrited_name[0] == '#') // Register
		    {
		      operand_t ops1;
		      if(irop.ops1.name == "__aeabi_idivmod") // Special for [MOD]
			{
			  ops1.rewrited_name = "#1";
			}
		      else
			{
			  ops1.rewrited_name = "#0";
			}
		      AsmGenerator::Load_Reg("r"+irop.dest.rewrited_name.substr(1),
					     ops1, regctx, InstrList);
		      break;
		    }
		  if(irop.dest.rewrited_name[0] == '^') // MemRef
		    {
		      if(irop.ops1.name == "__aeabi_idivmod") // Special for [MOD]
			{
			  AsmGenerator::Store_Stack(operand_t("r1"),
						    stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_258),
						    regctx, InstrList);
			}
		      else
			{
			  AsmGenerator::Store_Stack(operand_t("r0"),
						    stack_var_base + var_to_int(irop.dest.rewrited_name, ASSERT_EXIT_CODE_259),
						    regctx, InstrList);
			}
		      break;
		    }
		  myAssert(0, ASSERT_EXIT_CODE_174);
		}
	      case OPER_RET:
	      case OPER_RETN:
		// Return value is saved in [dest] (might be NULL)
		// Return value might be [Register/MemRef], [Immediate], [GloblVar]
		//              won't be [GloblArray]
		{
		  // Consider Return Value
		  if(irop.dest.type != OPERAND_NOT_DEFINED)
		    {
		      myAssert(irop.oper == OPER_RETN, ASSERT_EXIT_CODE_175);
		      // Load Return Value to r0
		      AsmGenerator::Load_Reg("r0", irop.dest, regctx, InstrList);
		    }
		  else
		    {
		      myAssert(irop.oper == OPER_RET, ASSERT_EXIT_CODE_176);
		    }
		  // Restore non-volatile registers
		  // LR
		  if(has_function_call || regctx.r14_used)
		    {
		      InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("Restore " COLOR_CYAN "LR")));
		      operand_t ops1;
		      int offset = stack_lr_base - stack_var_base;
		      ops1.rewrited_name = "^" + std::to_string(offset);
		      AsmGenerator::Load_Reg("lr", ops1, regctx, InstrList);
		    }
		  // Other Regular Registers
		  if(regctx.guard_map.size() < 4)
		    // Fastcall
		    {
		      for(auto it : regctx.guard_map)
			{
			  if(it.first == 11) // Special treat r11
			    {
			      InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("R11 RestorePoint")));
			      InstrList.push_back(new instr_t(MNEMO_PASSNOTE, std::string("R11_RP " + std::to_string(stack_guard_base + it.second))));
			      continue;
			    }
			  InstrList.push_back(new instr_t(MNEMO_NOTE, std::string("Restore " COLOR_CYAN "r" + std::to_string(it.first))));
			  operand_t ops1;
			  int offset = stack_guard_base - stack_var_base + it.second;
			  ops1.rewrited_name = "^" + std::to_string(offset);
			  AsmGenerator::Load_Reg("r"+std::to_string(it.first), ops1, regctx, InstrList);
			}
		      
		    }
		  else
		    // Ordinary Call
		    { // [r11] is directly stored and restored, not putting into guardpoint
		      // First, calculate offset, store in r12
		      if(AsmGenerator::check_fit_12(stack_guard_base))
			{
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  stack_guard_base));
			  // [r12] = [sp] + Offset
			}
		      else
			{
			  // First, load this immediate into [r12]
			  AsmGenerator::Load_Reg("r12", stack_guard_base, regctx, InstrList);

			  // Then, Calculate (r12+sp)
			  InstrList.push_back(new instr_t(MNEMO_ADD,
							  std::string("r12"),
							  std::string("sp"),
							  std::string("r12")));
			}

		      // Then, use STM to guard the variables into correspond location
		      std::string guards = "{";
		      bool first = true;
		      for(auto it : regctx.guard_map)
			{
			  if(first)
			    {
			      first = false;
			    }
			  else
			    {
			      guards += ",";
			    }
			  guards += "r" + std::to_string(it.first);
			}
		      guards += "}";
		      InstrList.push_back(new instr_t(MNEMO_LDM,std::string("r12"),guards));
		    }

		  // Restore Stack
		  int stack_size =
		    regctx.stack_lr_size +
		    regctx.stack_guard_size +
		    regctx.stack_var_size +
		    regctx.stack_arg_size;
		  InstrList.push_back(new instr_t(MNEMO_NOTE,
						  std::string("Stack RestorePoint")));
		  InstrList.push_back(new instr_t(MNEMO_PASSNOTE,
						  std::string("SP_RP " + std::to_string(stack_size))));

		  // Restore Control Flow
		  InstrList.push_back(new instr_t(MNEMO_MOV,
						  std::string("pc"),
						  std::string("lr")));

		  break;
		}
	      case OPER_LABEL:
		{
		  InstrList.push_back(new instr_t(MNEMO_LABEL,
						  irop.dest.name));
		  break;
		}
	      case OPER_SPACE:
	      case OPER_WORD:
		{
		  // Should not occurred here
		  myAssert(0, ASSERT_EXIT_CODE_177);
		}
	      case OPER_NOP:
	      case OPER_ENDF:
		{
		  // Do Nothing
		  break;
		}
	      case OPER_BLK_SEP:
		{
		  InstrList.push_back(new instr_t(MNEMO_PASSNOTE,
						  std::string("BLK_SEP")));
		  break;
		}
	      default:
	      	{
	      	  break;
	      	}
	      }
	  }
    }
  }

  
  // Pass 6. Post-generate Processing
  {
    myAssert(InstrListPosition != InstrList.end(),ASSERT_EXIT_CODE_16);
    // Pass 6.1, Guard and Restore [r11]
    for(auto p = InstrListPosition;
	p != InstrList.end();
	++p)
      {
	if( (*p)->mnemonic == MNEMO_PASSNOTE) // Passnotes
	  {
	    auto& liter = (*p)->dest.name;
	    myAssert(liter != "", ASSERT_EXIT_CODE_178);
	    std::list<instr_t*> tstList;
	    register_ctx_t tstCtx;	    
	    if(liter.substr(0,6) == "R11_GP")
	      // R11 Guard Point
	      {
		int offset = std::stoi(liter.substr(7));
		if(regctx.r11_guarded == true) // Need to guard [r11]
		  {
		    AsmGenerator::Store_Stack(operand_t("r11"), offset, tstCtx, tstList);
		  }
		goto _passnote_out;
	      }
	    
	    if(liter.substr(0,6) == "R11_RP")
	      // R11 Restore Point
	      {
		int offset = std::stoi(liter.substr(7));
		if(regctx.r11_guarded == true) // Need to restore [r11]
		  {
		    operand_t ops1;
		    ops1.rewrited_name = "^" + std::to_string(offset);
		    AsmGenerator::Load_Reg("r11", ops1, tstCtx, tstList);
		  }
		goto _passnote_out;
	      }

	    if(liter.substr(0,5) == "SP_SP")
	      {
		int stack_size = std::stoi(liter.substr(6));
		myAssert(stack_size > 0,ASSERT_EXIT_CODE_17);
		if(stack_size == 4 && regctx.r11_guarded == false) // No need to guard anything
		  {
		    goto _passnote_out;
		  }
		
		if(AsmGenerator::check_fit_12(stack_size))
		  {
		    tstList.push_back(new instr_t(MNEMO_SUB,
						    std::string("sp"),
						    std::string("sp"),
						    stack_size));
		    }
		  else // Not fit
		    {
		      AsmGenerator::Load_Reg("r12", stack_size, tstCtx, tstList);
		      tstList.push_back(new instr_t(MNEMO_SUB,
						      std::string("sp"),
						      std::string("sp"),
						      std::string("r12")));
		    }
		goto _passnote_out;
	      }
	    if(liter.substr(0,5) == "SP_RP")
	      {
		int stack_size = std::stoi(liter.substr(6));
		myAssert(stack_size > 0,ASSERT_EXIT_CODE_18);
		if(stack_size == 4 && regctx.r11_guarded == false) // No need to guard anything
		  {
		    goto _passnote_out;
		  }
		  
		if(AsmGenerator::check_fit_12(stack_size))
		  {
		    tstList.push_back(new instr_t(MNEMO_ADD,
						  std::string("sp"),
						  std::string("sp"),
						  stack_size));
		  }
		else // Not fit
		  {
		    AsmGenerator::Load_Reg("r12", stack_size, tstCtx, tstList);
		    tstList.push_back(new instr_t(MNEMO_ADD,
						  std::string("sp"),
						  std::string("sp"),
						  std::string("r12")));
		  }
		goto _passnote_out;


	      }
	    continue;
	  _passnote_out:
	    p = InstrList.insert(std::next(p), tstList.begin(), tstList.end());
	  }
      }


    // Pass 6.2, Scan and identifying Immediates
    
    for(auto p : InstrList)
      {
	auto oper = p->dest;
#define TEST_IMMEDIATE(oper)				\
	if(oper.name != "" &&				\
	   oper.name[0] == '#')				\
	  {						\
	    oper.imm = std::stoi(oper.name.substr(1));	\
	    oper.name = "";				\
	    oper.type = OPERAND_IMM;			\
	  }
	TEST_IMMEDIATE(p->dest);
	TEST_IMMEDIATE(p->ops1);
	TEST_IMMEDIATE(p->ops2);
	TEST_IMMEDIATE(p->ops3);
      }

    // Pass 6.3, Eliminate Useless Movement
      {
	auto p = InstrList.begin();
	while(p != InstrList.end())
	  {
	    if((*p)->mnemonic == MNEMO_MOV && (*p)->condition_string == "")
	      {
		if(
		   (*p)->dest.type == OPERAND_VAR &&
		   (*p)->ops1.type == OPERAND_VAR &&
		   (*p)->dest.name == (*p)->ops1.name)
		  {
		    p = InstrList.erase(p);
		  }
		else
		  {
		    p++;
		  }
	      }
	    else
	      {
		p++;
	      }
	  }
      }
      if(print_ASMLine && !is_fake)
      	{
      	  for(auto p = InstrListPosition;
      	      p != InstrList.end(); ++p)
      	    {
      	      (*p)->print(std::cout);
      	    }
      	}
      
      // Pass 6.4, Assembly Optimization
      if(optimize_flag == 1)
	{
	  if(!is_fake)
	    {
	      for(int i = 0; i < 20; i++)
	      	{
	      	  // std::cerr<<"Pass: "<<i<<std::endl;
	      	  ASM_Optimize(InstrListPosition, InstrList.end(), InstrList, i >10?is_fake:true);
	      	}
	    }
	}

      // Pass 6.5, Calculate each instruction's offset indication
      {
	std::map<MNEMONIC_TYPE, int > instr_len =
	  {
	    {MNEMO_RAW, 0},      // Raw Assembly
	    {MNEMO_ADD, 4},      // Addition
	    {MNEMO_SUB, 4},      // Subtraction
	    {MNEMO_SDIV, 4},      // Signed Divide
	    {MNEMO_MOV, 4},      // Register Move
	    {MNEMO_MOV32, 8},    // 32-bits Immediate Move
	    {MNEMO_LDR, 4},      // Load  MemRef
	    {MNEMO_STR, 4},      // Store MemRef
	    {MNEMO_LDR_GLB, 8},  // Load Global Reference
	    {MNEMO_MUL, 4},      // Multiple
	    {MNEMO_LSL, 4},      // Logical Shift Left
	    {MNEMO_LSR, 4},      // Logical Shift Right
	    {MNEMO_ASR, 4},      // Arith   Shift Right
	    {MNEMO_MLA, 4},      // MLA
	    {MNEMO_MLS, 4},      // MLS
	    {MNEMO_LDM, 4},      // LDM
	    {MNEMO_STM, 4},      // STM
	    {MNEMO_CMP, 4},      // Compare

	    {MNEMO_B, 4},        // Branch
	    {MNEMO_BL,4},        // Branch and Link
	    {MNEMO_NOP, 0},      // No Operation
	    {MNEMO_LABEL, 0},    // Labels
	    {MNEMO_NOTE, 0},     // Comments
	    {MNEMO_PASSNOTE, 0}, // Specified Notes
	    {MNEMO_BLK_SEP, 0},  // Block Separator
	  };
	for(auto p = InstrList.begin();
	    p != InstrList.end();
	    ++p)
	  {
	    myAssert(instr_len.count((*p)->mnemonic),ASSERT_EXIT_CODE_19);
	    (*p)->absolute_offset = current_offset;
	    current_offset += instr_len[(*p)->mnemonic];
	  }
      }
  }
}

void asm_context_t::oper_output(IROper_t& oper, std::list<instr_t*>& InstrList)
{
  std::stringstream ss;
  switch(oper.oper)
    {
    case OPER_WORD:
      ss<<COLOR_CYAN ".word\t" COLOR_GREEN<<oper.dest.imm;
      current_offset += 4;
      break;
    case OPER_SPACE:
      if(oper.dest.imm != 0)
	{
	  ss<<COLOR_CYAN ".space\t" COLOR_GREEN<<oper.dest.imm;
	  current_offset += oper.dest.imm;
	}
      break;
    default:
      break;
    }
  if(ss.str() != "")
    InstrList.push_back(new instr_t(MNEMO_RAW, operand_t(ss.str())));  
}

void asm_context_t::label_output(IRLabel_t& lab, std::list<instr_t*>& InstrList)
{
  if(lab.type == DATA_AREA_BEGIN)
    {
      std::stringstream ss;
      ss<<std::endl;
      // Refresh
      ss<<COLOR_CYAN ".data" << std::endl;
      ss<<COLOR_CYAN ".global "
	<<COLOR_PURPLE<< rename_operand(lab.label_name)<<std::endl
	<<COLOR_PURPLE << rename_operand(lab.label_name)<< COLOR_YELLOW <<":";
      offset_indicator.insert({lab.label_name, current_offset});
      // printf("Offset_indicator.insert(%s)\n", lab.label_name.c_str());
      InstrList.push_back(new instr_t(MNEMO_RAW, operand_t(ss.str())));
      InstrList.push_back(new instr_t(MNEMO_NOTE, operand_t(lab.label_name + " at offset "+ std::to_string(current_offset))));
    }
}

void asm_context_t::data_output(IRData_t& dat, std::list<instr_t*>& InstrList)
{
  myAssert(0, ASSERT_EXIT_CODE_180);
  // Should not reach here
}

void asm_context_t::note_output(IRNote_t &note, std::list<instr_t*>& InstrList)
{
  std::stringstream ss;
  ss <<COLOR_GREEN "## "<< note.note;
  InstrList.push_back(new instr_t(MNEMO_RAW, operand_t(ss.str())));  
}

void register_ctx_t::expire_end_of_blk(basic_block_t* blkp)
{
  std::set<std::string> to_be_dropped;
  for(auto it : active_name_to_reg)
    {
      auto var_name = it.first;
      int  reg_idx  = it.second;
      myAssert(latest.count(var_name), ASSERT_EXIT_CODE_181);
      myAssert(var_alloc_table.count(var_name), ASSERT_EXIT_CODE_182);
      myAssert(var_alloc_table[var_name] == "#"+std::to_string(reg_idx), ASSERT_EXIT_CODE_183);
      int  exp_time = latest[var_name];

      // Might be expired, check if Current Block's LiveOut
      //  contains this variable
      if(blkp->liveOut.count(var_name)) // Can't expire
	continue;

      // Check if All blocks after current block contains this variable
      myAssert(post_liveOut.count(blkp), ASSERT_EXIT_CODE_184);
      if(post_liveOut[blkp].count(var_name))
	continue;

      // Check if this variable is referenced in this block
      myAssert(typeid( *((IROper_t*)(*blkp->begin)) ) == typeid(IROper_t), ASSERT_EXIT_CODE_185);
      if(exp_time >= ((IROper_t*)(*blkp->begin))->timestamp)
	continue;

      to_be_dropped.insert(var_name);
    }
  
  for(auto it : to_be_dropped)
    {
      auto var_name = it;
      myAssert(active_name_to_reg.count(var_name), ASSERT_EXIT_CODE_186);
      int  reg_idx  = active_name_to_reg[var_name];
      myAssert(active_reg_to_name.count(reg_idx), ASSERT_EXIT_CODE_187);
      myAssert(active_reg_to_name[reg_idx] == var_name, ASSERT_EXIT_CODE_188);
      myAssert(__availablility[reg_idx] == false, ASSERT_EXIT_CODE_189);
      active_name_to_reg.erase(var_name);
      active_reg_to_name.erase(reg_idx);
      __availablility[reg_idx] = true;
    }
}

void register_ctx_t::expire_old_intervals(int cur_time, basic_block_t* blkp)
{
  std::set<std::string> to_be_dropped;
  for(auto it : active_name_to_reg)
    {
      auto var_name = it.first;
      int  reg_idx  = it.second;
      myAssert(latest.count(var_name), ASSERT_EXIT_CODE_190);
      myAssert(var_alloc_table.count(var_name), ASSERT_EXIT_CODE_191);
      myAssert(var_alloc_table[var_name] == "#"+std::to_string(reg_idx), ASSERT_EXIT_CODE_192);
      int  exp_time = latest[var_name];

      if(cur_time < exp_time) // Do NOT expire
	continue;

      // Might be expired, check if Current Block's LiveOut
      //  contains this variable
      myAssert(var_to_blk.count(var_name), ASSERT_EXIT_CODE_193);

      if(blkp->liveOut.count(var_name)) // Can't expire
	continue;

      // Check if All blocks after current block contains this variable
      myAssert(post_liveOut.count(blkp), ASSERT_EXIT_CODE_194);
      if(post_liveOut[blkp].count(var_name))
	continue;

      // Okay, we can expire this variable, add it into drop list
      to_be_dropped.insert(var_name);
      // printf("Drop %s->%d\n", var_name.c_str(), reg_idx);
    }
  for(auto it : to_be_dropped)
    {

      auto var_name = it;
      myAssert(active_name_to_reg.count(var_name), ASSERT_EXIT_CODE_195);
      int  reg_idx  = active_name_to_reg[var_name];
      myAssert(active_reg_to_name.count(reg_idx), ASSERT_EXIT_CODE_196);
      myAssert(active_reg_to_name[reg_idx] == var_name, ASSERT_EXIT_CODE_197);
      myAssert(__availablility[reg_idx] == false, ASSERT_EXIT_CODE_198);
      active_name_to_reg.erase(var_name);
      active_reg_to_name.erase(reg_idx);
      __availablility[reg_idx] = true;
    }
}

bool register_ctx_t::test_availability(int reg_idx)
{
  myAssert(reg_idx < max_regs || reg_idx == 14, ASSERT_EXIT_CODE_199) ;
  return this->__availablility[reg_idx];
}

int register_ctx_t::find_free_reg(int start, std::string var_name)
{
  if(start < 4)
    // special routine
    {
      myAssert(!has_function_call.count(var_name), ASSERT_EXIT_CODE_200);
      if(this->max_regcnt.count(var_name)) // hax maximum limit
	{
	  for(int i = max_regcnt[var_name];
	      i >= start;
	      i--)
	    {
	      // printf("Testing %d\n", i);
	      if(test_availability(i))
		return i;
	    }
	  // fail to allocate registers, go to normal routine
	  // but we got a hard restriction: range between (max_regcnt, 4) is forbidden.
	  start = 4;
	}
    }
  bool has_likely = false;
  if(this->loadarg_target.count(var_name) &&
     this->loadarg_target[var_name] < 4)
    // [var_name] is binded to a LOAD_ARG instruction
    // try to assign loadarg_target with same parameter register
    {
      int likely_reg_idx = this->loadarg_target[var_name];
      has_likely = true;
      if(start > likely_reg_idx) // we require allocation from [start]
	goto normal_routine; // fail

      if(!test_availability(likely_reg_idx)) // not available
	goto normal_routine;

      return likely_reg_idx; // Okay
    }
  
 normal_routine:
  if(has_likely)
    {
      // First consider #4, to avoid occupy
      //  other LIKELY[#0-#3]
      for(int i = std::max(4,start); i < max_regs; i++)
	{
	  if(test_availability(i))
	    return i;
	}
      for(int i = start; i < max_regs; i++)
	{
	  if(test_availability(i))
	    return i;
	}
    }
  else
    {
      for(int i = start; i < max_regs; i++)
	{
	  if(test_availability(i))
	    return i;
	}
    }
  // Special: If start < 4, no function call occurred, try to
  // allocate [r14/LR] to this variable
  if(start < 4)
    {
      if(test_availability(14))
	{
	  return 14;
	}
    }

  return -1; // Could not found
}

void register_ctx_t::occupy_reg(int reg_idx, std::string var_name)
// occupy [#reg_idx] with [var_name]
{
  // std::cerr<<"occupy_reg: "<<reg_idx<<" "<<var_name<<std::endl;
  myAssert(reg_idx < max_regs || reg_idx == 14, ASSERT_EXIT_CODE_201);
  if(reg_idx == 14)
    this->r14_used = true;
  myAssert(test_availability(reg_idx) == true, ASSERT_EXIT_CODE_202);
  __availablility[reg_idx] = false;
  __touched[reg_idx] = true;
  active_name_to_reg.insert({var_name, reg_idx });
  active_reg_to_name.insert({reg_idx , var_name});

  // Push rule into [var_alloc_table]
  myAssert(!var_alloc_table.count(var_name), ASSERT_EXIT_CODE_203);
  var_alloc_table.insert({var_name, "#"+std::to_string(reg_idx)});
  
}

void register_ctx_t::occupy_mem(std::string var_name)
// give [var_name] a new memory location
{
  myAssert(!var_alloc_table.count(var_name), ASSERT_EXIT_CODE_204);
  var_alloc_table.insert({var_name, "^" + std::to_string(this->stack_var_size)});
  this->stack_var_size += 4;
}

std::string register_ctx_t::find_spill_var(int start)
{
  std::string spilled_var = "";
  int latest_timestamp = -1;
  for(auto i : active_reg_to_name)
    {
      int         reg_idx  = i.first;
      std::string var_name = i.second;
      if(reg_idx < start)
	continue; // We shouldn't spill it
      myAssert(latest.count(var_name), ASSERT_EXIT_CODE_205);
      if(latest[var_name] > latest_timestamp)
	{
	  latest_timestamp = latest[var_name];
	  spilled_var = var_name;
	}
    }
  myAssert(spilled_var != "", ASSERT_EXIT_CODE_206);
  return spilled_var;
}

void register_ctx_t::spill_var(std::string name)
{
  myAssert(active_name_to_reg.count(name), ASSERT_EXIT_CODE_207);
  int reg_idx = active_name_to_reg[name];
  myAssert(test_availability(reg_idx) == false, ASSERT_EXIT_CODE_208);
  myAssert(active_reg_to_name.count(reg_idx), ASSERT_EXIT_CODE_209);
  myAssert(active_reg_to_name[reg_idx] == name, ASSERT_EXIT_CODE_210);
  // Remove from ACTIVE LIST
  active_reg_to_name.erase(reg_idx);
  active_name_to_reg.erase(name);

  // Modify allocation table
  myAssert(var_alloc_table.count(name), ASSERT_EXIT_CODE_211);
  myAssert(var_alloc_table[name][0] == '#', ASSERT_EXIT_CODE_212);
  var_alloc_table[name] = "^" + std::to_string(this->stack_var_size);
  __availablility[reg_idx] = true;
  this->stack_var_size += 4;
}

register_ctx_t::register_ctx_t() : __availablility(15, true),
				   __touched(15, false),
				   stack_lr_size(0),
				   stack_var_size(0),
				   stack_arg_size(0),
				   stack_guard_size(0),
				   r11_guarded(false),
				   r14_used(false)
{
  
}


////////////////////////////////////////////////////////////////////
////////A S S E M B L Y/////////////////////////////////////////////
//////////////////////G E N E R A T I N G///////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
instr_t::instr_t(MNEMONIC_TYPE mnemonic) :
  mnemonic(mnemonic), dest(operand_t()), ops1(operand_t()),
  ops2(operand_t()), ops3(operand_t()), ref_count(0), line_number(0)
{
}

instr_t::instr_t(MNEMONIC_TYPE mnemonic,
		   const operand_t& dest) :
  mnemonic(mnemonic), dest(dest), ops1(operand_t()),
  ops2(operand_t()), ops3(operand_t()), ref_count(0), line_number(0)
{
}

instr_t::instr_t(MNEMONIC_TYPE mnemonic,
		   const operand_t& dest,
		   const operand_t& ops1) :
  mnemonic(mnemonic), dest(dest), ops1(ops1),
  ops2(operand_t()), ops3(operand_t()), ref_count(0), line_number(0)
{
}

instr_t::instr_t(MNEMONIC_TYPE mnemonic,
		   const operand_t& dest,
		   const operand_t& ops1,
		   const operand_t& ops2) :
  mnemonic(mnemonic), dest(dest), ops1(ops1),
  ops2(ops2), ops3(operand_t()), ref_count(0), line_number(0)
{
}

instr_t::instr_t(MNEMONIC_TYPE mnemonic,
		   const operand_t& dest,
		   const operand_t& ops1,
		   const operand_t& ops2,
		   const operand_t& ops3) :
  mnemonic(mnemonic), dest(dest), ops1(ops1),
  ops2(ops2), ops3(ops3), ref_count(0), line_number(0)
{
}

_asm_rhs_t instr_t::get_rhs()
{
  return _asm_rhs_t(this->mnemonic, this->ops1, this->ops2, this->ops3);
}

std::string AsmGenerator::Assure_Reg(const operand_t &source_oper,
				     register_ctx_t &regctx,
				     std::list<instr_t *> &InstrList,
				     bool &r12_occupied)
{
  if(source_oper.rewrited_name != "" &&
     source_oper.rewrited_name[0] == '#') // source_oper is already a register
    {
      return "r" + source_oper.rewrited_name.substr(1);
    }
  std::string tmp_reg = r12_occupied?"r11":"r12";
  if(tmp_reg == "r11")
    regctx.r11_guarded = true;

  if(tmp_reg == "r12")
    r12_occupied = true;
  // Tell that [r12] is already occupied.
  AsmGenerator::Load_Reg(tmp_reg, source_oper, regctx, InstrList);
  return tmp_reg;
}

void AsmGenerator::Load_Reg(std::string target_reg,
			    const operand_t &source_oper,
			    register_ctx_t& regctx,
			    std::list<instr_t *> &InstrList,
			    std::string condition_string)
{
  // printf("Load Reg: %s <- [%s->%s,%d]\n",
  // 	 target_reg.c_str(),
  // 	 source_oper.name.c_str(),
  // 	 source_oper.rewrited_name.c_str(),
  // 	 source_oper.imm);
  myAssert(target_reg[0] == 'r' || is_special_register(target_reg), ASSERT_EXIT_CODE_213);
  if(target_reg == "r11")
      regctx.r11_guarded = true;
  if(source_oper.name[0] == 'r')
    {
      InstrList.push_back(new instr_t(MNEMO_MOV,
				      target_reg,
				      source_oper.name));
      return;
      
    }
  if(source_oper.rewrited_name != "") // Renamed
    {
      // If Source is an Register ( CASE #1 )
      if(source_oper.rewrited_name[0] == '#')
	{
	  auto liter = source_oper.rewrited_name;
	  InstrList.push_back(new instr_t(MNEMO_MOV,
					  target_reg,
					  "r"+liter.substr(1)));
	  InstrList.back()->condition_string = condition_string;
	  return;
	}

      // If Source is an Mem Reference ( CASE #2 )
      if(source_oper.rewrited_name[0] == '^')
	{
	  // myAssert(source_oper.name != "",ASSERT_EXIT_CODE_20);
	  // myAssert(source_oper.name.size() >= 2,ASSERT_EXIT_CODE_21);
	  if(source_oper.name.size() >= 2 && source_oper.name[1] == '&')
	    {
	      int variable_base = regctx.stack_arg_size;
	      int offset = var_to_int(source_oper.rewrited_name, ASSERT_EXIT_CODE_260) + variable_base;
	      if(AsmGenerator::check_fit_12(offset))
		{
		  InstrList.push_back(new instr_t(MNEMO_ADD,
						  target_reg,
						  std::string("sp"),
						  offset));
		  // [r12] = [sp] + Offset
		}
	      else
		{
		  // First, load this immediate into [r12]
		  AsmGenerator::Load_Reg(target_reg, offset, regctx, InstrList);

		  // Then, Calculate (r12+sp)
		  InstrList.push_back(new instr_t(MNEMO_ADD,
						  target_reg,
						  std::string("sp"),
						  target_reg));
		}
	      return;
	    }
	  else
	    {
	      int variable_base = regctx.stack_arg_size;

	      int offset = var_to_int(source_oper.rewrited_name, ASSERT_EXIT_CODE_260) + variable_base;
	      if(AsmGenerator::check_fit_mem_offset(offset))
		// In [LDR]'s range
		{
		  InstrList.push_back(new instr_t(MNEMO_LDR,
						  target_reg,
						  "[sp,#" + std::to_string(offset) +"]"));
		  InstrList.back()->condition_string = condition_string;
		  return;
		}
	      else
		// Out of [LDR]'s range
		{
		  // First,  load immediate into this register
		  AsmGenerator::Load_Reg_Immediate(target_reg, offset, InstrList);

		  /*
		  // Second, Calculate target address
		  InstrList.push_back(new instr_t(MNEMO_ADD,
		  target_reg,
		  std::string("sp"),
		  target_reg));
		  */
		  // Third,  Load address into register
		  InstrList.push_back(new instr_t(MNEMO_LDR,
						  target_reg,
						  "[sp," + target_reg + "]"));
		  InstrList.back()->condition_string = condition_string;
		  return;
		}
	  
	      myAssert(0, ASSERT_EXIT_CODE_214);
	      // Should not reach here.
	    }
	}

      myAssert(0, ASSERT_EXIT_CODE_215);
      // Should not reach here.
    }

  myAssert(source_oper.rewrited_name == "", ASSERT_EXIT_CODE_216);
  
  // If Source is an Immediate ( CASE #3 )
  if(source_oper.type == OPERAND_IMM)
    {
      AsmGenerator::Load_Reg_Immediate(target_reg, source_oper.imm, InstrList, condition_string);
      return;
    }

  myAssert(source_oper.type == OPERAND_VAR, ASSERT_EXIT_CODE_217);

  if(source_oper.name[0] != '$' &&
     source_oper.name[0] != '@')
    {
      printf("Assertion Failed Source Oper = ");
      operand_t tst = source_oper;
      tst.print(std::cerr);

    }
  myAssert(source_oper.name[0] == '$' ||
	   source_oper.name[0] == '@', ASSERT_EXIT_CODE_218); // Must be global or arg
  
  // If Source is a Global Array ( CASE #4 )
  if(source_oper.name[0] == '$' &&
     source_oper.name[1] == '&')
    {
      myAssert(condition_string == "", ASSERT_EXIT_CODE_219);
      myAssert(source_oper.name.size() > 2, ASSERT_EXIT_CODE_220);
      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
				      target_reg,
				      source_oper.name));
      return;
    }

  // If Source is a Global Var ( CASE #5 )
  myAssert(source_oper.name[1] != '&', ASSERT_EXIT_CODE_221);
  if(source_oper.name[0] == '$')
    {
      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
				      target_reg,
				      source_oper.name));
      InstrList.push_back(new instr_t(MNEMO_LDR,
				      target_reg,
				      "["+target_reg+"]"));
      InstrList.back()->condition_string = condition_string;
      return;
    }

  // If Source is an Argument ( CASE #6 )
  if(source_oper.name[0] == '@')
    // Function Arguments [SP+STACK_SIZE+4*(id-4)]
    {
      myAssert(isdigit(source_oper.name[1]), ASSERT_EXIT_CODE_222);
      int index = var_to_int(source_oper.name, ASSERT_EXIT_CODE_261);
      myAssert(index > 3, ASSERT_EXIT_CODE_223);
      int arg_base =
	regctx.stack_arg_size +
	regctx.stack_var_size +
	regctx.stack_guard_size +
	regctx.stack_lr_size;
	
      int offset = arg_base + 4*(index-4);
      // Arg @0 - @3 is saved in #0 - #3
      if(AsmGenerator::check_fit_mem_offset(offset))
	// In [LDR]'s range
	{
	  InstrList.push_back(new instr_t(MNEMO_LDR,
					  target_reg,
					  "[sp,#" + std::to_string(offset) +"]"));
	  InstrList.back()->condition_string = condition_string;
	  return;
	}
      else
	// Out of [LDR]'s range
	{
	  // First,  load immediate into this register
	  AsmGenerator::Load_Reg_Immediate(target_reg, offset, InstrList);
	  
	  // Second, Calculate target address
	  /*
	  InstrList.push_back(new instr_t(MNEMO_ADD,
					  target_reg,
					  std::string("sp"),
					  target_reg));
	  */
	  // Third,  Load address into register
	  InstrList.push_back(new instr_t(MNEMO_LDR,
					  target_reg,
					  "[" + std::string("sp")+","+ target_reg + "]"));
	  InstrList.back()->condition_string = condition_string;
	  return;
	}
      
      myAssert(0, ASSERT_EXIT_CODE_224);
    }

  myAssert(0, ASSERT_EXIT_CODE_225);
  // Should not reach here

}

void AsmGenerator::Store_Stack(const operand_t &source_oper,
			       int offset,
			       register_ctx_t &regctx,
			       std::list<instr_t *> &InstrList,
			       std::string condition_string)
{
  // printf("Trying to Store Stack: [%s,%d], offset = %d\n",source_oper.name.c_str(),
  // 	 source_oper.imm, offset);

  if(source_oper.rewrited_name != "") // Renamed
    {
      // If Source is an Register ( Case #1 )
      if(source_oper.rewrited_name[0] == '#')
	{
	  std::string liter = "r" + source_oper.rewrited_name.substr(1);
	  AsmGenerator::_store_register_to_stack(liter, offset, regctx, InstrList,condition_string);
	  return;

	}

      // If Source is a Mem Reference ( Case #2 )
      if(source_oper.rewrited_name[0] == '^')
	{
	  if(source_oper.name.size() >= 2 && source_oper.name[1] == '&')
	    {
	      int variable_base = regctx.stack_arg_size;
	      int offset2 = var_to_int(source_oper.rewrited_name, ASSERT_EXIT_CODE_260) + variable_base;
	      if(AsmGenerator::check_fit_12(offset2))
		{
		  InstrList.push_back(new instr_t(MNEMO_ADD,
		  				  std::string("r12"),
		  				  std::string("sp"),
		  				  offset2));
		  // [r12] = [sp] + Offset
		}
	      else
		{
		  // First, load this immediate into [r12]
		  AsmGenerator::Load_Reg("r12", offset2, regctx, InstrList);

		  // Then, Calculate (r12+sp)
		  InstrList.push_back(new instr_t(MNEMO_ADD,
						  std::string("r12"),
						  std::string("sp"),
						  std::string("r12")));
		}
	      AsmGenerator::_store_register_to_stack("r12" , offset, regctx, InstrList,condition_string);
	      return;
	    }
	  else
	    {
	      AsmGenerator::Load_Reg("r12", source_oper, regctx, InstrList);
	      AsmGenerator::_store_register_to_stack("r12" , offset, regctx, InstrList,condition_string);
	      return;
	    }
	}
      myAssert(0, ASSERT_EXIT_CODE_226);
    }

  myAssert(source_oper.rewrited_name == "", ASSERT_EXIT_CODE_227);


  // If Source is an Immediate ( CASE #3 )
  if(source_oper.type == OPERAND_IMM)
    {
      // Load immediate into r12
      AsmGenerator::Load_Reg("r12", source_oper, regctx, InstrList);
      // Store
      AsmGenerator::_store_register_to_stack("r12" , offset, regctx, InstrList,condition_string);      
      return;
    }
  myAssert(source_oper.type == OPERAND_VAR, ASSERT_EXIT_CODE_228);

  // Special Case: Source is an SPECIAL register [sp/lr/pc]
  // or Source is directly [r#]
  if(is_special_register(source_oper.name) || source_oper.name[0] == 'r')
    {
      AsmGenerator::_store_register_to_stack(source_oper.name, offset, regctx, InstrList,condition_string);
      return;
    }

 // If Source is a Global Array ( CASE #4 )
  if(source_oper.name[0] == '$' &&
     source_oper.name[1] == '&')
    {
      myAssert(condition_string == "", ASSERT_EXIT_CODE_229);
      myAssert(source_oper.name.size() > 2, ASSERT_EXIT_CODE_230);
      InstrList.push_back(new instr_t(MNEMO_LDR_GLB,
				      std::string("r12"),
				      source_oper.name));
      AsmGenerator::_store_register_to_stack("r12" , offset, regctx, InstrList,condition_string);
      return;
      // Not implemented yet.
    }

  // If Source is a Global Var ( CASE #5 )
  myAssert(source_oper.name[1] != '&', ASSERT_EXIT_CODE_231);
  if(source_oper.name[0] == '$')
    {
      AsmGenerator::Load_Reg("r12", source_oper, regctx, InstrList);
      AsmGenerator::_store_register_to_stack("r12", offset, regctx, InstrList,condition_string);      
      return;
      // Not implemented yet.
    }

  myAssert(0, ASSERT_EXIT_CODE_232);
}

void AsmGenerator::_store_register_to_stack(std::string reg_name,
					    int offset,
					    register_ctx_t& regctx,
					    std::list<instr_t*>& InstrList,
					    std::string condition_string)
{
  myAssert(reg_name[0] == 'r' || is_special_register(reg_name), ASSERT_EXIT_CODE_233);
  if(AsmGenerator::check_fit_mem_offset(offset)) // Fit into one instruction
    {
      InstrList.push_back(new instr_t(MNEMO_STR, reg_name, "[sp,#"+std::to_string(offset) + "]"));
      InstrList.back()->condition_string = condition_string;
      return;
    }
  else
    {
      std::string temp_reg = reg_name == "r12" ? "r11" : "r12";
      if(temp_reg == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg_Immediate(temp_reg, offset, InstrList);
      // Load [temp_reg] <- offset
      /*
      InstrList.push_back(new instr_t(MNEMO_ADD, temp_reg, std::string("sp"), temp_reg));
      */
      // Add [temp_reg] <- [SP + offset]
      InstrList.push_back(new instr_t(MNEMO_STR, reg_name, "["+ std::string("sp")+","+ temp_reg + "]"));
      // Store [ref:temp_reg] <- [reg_name]
      InstrList.back()->condition_string = condition_string;
      return;
    }
  myAssert(0, ASSERT_EXIT_CODE_234);
  // Should not reach here.
}

void AsmGenerator::Store_Globl_Var(const operand_t &source_oper,
				   std::string var_name,
				   register_ctx_t &regctx,
				   std::list<instr_t *> &InstrList,
				   std::string condition_string)
{
  myAssert(var_name[0] == '$', ASSERT_EXIT_CODE_235);
  myAssert(var_name[1] != '&', ASSERT_EXIT_CODE_236);
  std::string tmp_reg = source_oper.rewrited_name == "r12" ? "r11" : "r12";

  if(tmp_reg == "r11")
    regctx.r11_guarded = true;
  InstrList.push_back(new instr_t(MNEMO_LDR_GLB, tmp_reg, var_name));
  // Load Address into [tmp_reg]
  
  if(source_oper.rewrited_name != "") // Renamed
    {
      // If Source is an Register ( Case #1 )
      if(source_oper.rewrited_name[0] == '#')
	{
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  "r"+source_oper.rewrited_name.substr(1),
					  "["+ tmp_reg + "]"));
	  InstrList.back()->condition_string = condition_string;
	  return;
	}

      // If Source is a Mem Reference ( Case #2 )
      if(source_oper.rewrited_name[0] == '^')
	{
	  myAssert(tmp_reg == "r12", ASSERT_EXIT_CODE_237);
	  std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
	  if(tmp_reg == "r11")
	    regctx.r11_guarded = true;
	  AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  tmp2,
					  "["+ tmp_reg + "]"));
	  InstrList.back()->condition_string = condition_string;
	  return;
	}
      myAssert(0, ASSERT_EXIT_CODE_238);
    }

  myAssert(source_oper.rewrited_name == "", ASSERT_EXIT_CODE_239);


  // If Source is an Immediate ( CASE #3 )
  if(source_oper.type == OPERAND_IMM)
    {
      myAssert(tmp_reg == "r12", ASSERT_EXIT_CODE_240);
      std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+ tmp_reg + "]"));
      InstrList.back()->condition_string = condition_string;
      return;
    }
  myAssert(source_oper.type == OPERAND_VAR, ASSERT_EXIT_CODE_241);

  // Special Case: Source is an SPECIAL register [sp/lr/pc]
  // or Source is directly [r#]
  if(is_special_register(source_oper.name) || source_oper.name[0] == 'r')
    {
      myAssert(0, ASSERT_EXIT_CODE_242);
      // Not Implemented yet
      return;
    }

 // If Source is a Global Array ( CASE #4 )
  if(source_oper.name[0] == '$' &&
     source_oper.name[1] == '&')
    {
      myAssert(0, ASSERT_EXIT_CODE_243);
      // Should Not Reach Here
    }

  // If Source is a Global Var ( CASE #5 )
  myAssert(source_oper.name[1] != '&', ASSERT_EXIT_CODE_244);
  if(source_oper.name[0] == '$')
    {
      myAssert(tmp_reg == "r12", ASSERT_EXIT_CODE_245);
      std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+tmp_reg+"]"));
      InstrList.back()->condition_string = condition_string;
      return;
    }

  myAssert(0, ASSERT_EXIT_CODE_246);
  
}

void AsmGenerator::Store_Register_Offset(const operand_t &source_oper, std::string offset, int imm_offset, register_ctx_t &regctx, std::list<instr_t *> &InstrList)
{
  myAssert(offset[0] == 'r' || is_special_register(offset), ASSERT_EXIT_CODE_247);
  std::string tmp_reg = offset;

  if(tmp_reg == "r11")
    regctx.r11_guarded = true;
  
  if(source_oper.rewrited_name != "") // Renamed
    {
      // If Source is an Register ( Case #1 )
      if(source_oper.rewrited_name[0] == '#')
	{
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  "r"+source_oper.rewrited_name.substr(1),
					  "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
	  return;
	}

      // If Source is a Mem Reference ( Case #2 )
      if(source_oper.rewrited_name[0] == '^')
	{
	  std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
	  if(tmp_reg == "r11")
	    regctx.r11_guarded = true;
	  AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  tmp2,
					  "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
	  return;
	}
      myAssert(0, ASSERT_EXIT_CODE_248);
    }

  myAssert(source_oper.rewrited_name == "", ASSERT_EXIT_CODE_249);


  // If Source is an Immediate ( CASE #3 )
  if(source_oper.type == OPERAND_IMM)
    {
      std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
      return;
    }
  myAssert(source_oper.type == OPERAND_VAR, ASSERT_EXIT_CODE_250);

  // Special Case: Source is an SPECIAL register [sp/lr/pc]
  // or Source is directly [r#]
  if(is_special_register(source_oper.name) || source_oper.name[0] == 'r')
    {
      myAssert(0, ASSERT_EXIT_CODE_251);
      // Not Implemented yet
      return;
    }

 // If Source is a Global Array ( CASE #4 )
  if(source_oper.name[0] == '$' &&
     source_oper.name[1] == '&')
    {
      myAssert(0, ASSERT_EXIT_CODE_252);
      // Should Not Reach Here
    }

  // If Source is a Global Var ( CASE #5 )
  myAssert(source_oper.name[1] != '&', ASSERT_EXIT_CODE_253);
  if(source_oper.name[0] == '$')
    {
      std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+tmp_reg+std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
      return;
    }

  myAssert(0, ASSERT_EXIT_CODE_254);
  
}

void AsmGenerator::Store_Register_RegOffset(const operand_t &source_oper, std::string offset, std::string reg_offset, register_ctx_t &regctx, std::list<instr_t *> &InstrList)
{
  myAssert(offset[0] == 'r' || is_special_register(offset), ASSERT_EXIT_CODE_247);
  std::string tmp_reg = offset;
  bool r11_occ = false;
  bool r12_occ = false;
  if(tmp_reg == "r11")
    regctx.r11_guarded = true;
  if(offset == "r11" || reg_offset == "r11")
    {
      regctx.r11_guarded = true;
      r11_occ = true;
    }
  if(offset == "r12" || reg_offset == "r12")
    {
      r12_occ = true;
    }
  
  if(source_oper.rewrited_name != "") // Renamed
    {
      // If Source is an Register ( Case #1 )
      if(source_oper.rewrited_name[0] == '#')
	{
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  "r"+source_oper.rewrited_name.substr(1),
					  "["+ tmp_reg + "," + reg_offset + "]"));
	  return;
	}

      // If Source is a Mem Reference ( Case #2 )
      if(source_oper.rewrited_name[0] == '^')
	{
	  std::string tmp2 = "";
	  if(r12_occ)
	    {
	      if(r11_occ)
		{
		  InstrList.push_back(new instr_t(MNEMO_ADD, std::string("r12"), std::string("r11"), std::string("r12")));
		  AsmGenerator::Load_Reg("r11", source_oper, regctx, InstrList);
		  InstrList.push_back(new instr_t(MNEMO_STR,
						  std::string("r11"),
						  std::string("[r12]")));
		  return;
		}
	      else
		{
		  tmp2 = "r11";
		}
	    }
	  else
	    {
	      tmp2 = "r12";
	    }
	  // std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
	  if(tmp_reg == "r11")
	    regctx.r11_guarded = true;
	  if(tmp2 == "r11")
	    regctx.r11_guarded = true;

	  AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
	  InstrList.push_back(new instr_t(MNEMO_STR,
					  tmp2,
					  "["+ tmp_reg + "," + reg_offset + "]"));
	  return;
	}
      myAssert(0, ASSERT_EXIT_CODE_248);
    }

  myAssert(source_oper.rewrited_name == "", ASSERT_EXIT_CODE_249);


  // If Source is an Immediate ( CASE #3 )
  if(source_oper.type == OPERAND_IMM)
    {
      std::string tmp2 = "";
      if(r12_occ)
	{
	  if(r11_occ)
	    {
	      InstrList.push_back(new instr_t(MNEMO_ADD, std::string("r12"), std::string("r11"), std::string("r12")));
	      AsmGenerator::Load_Reg("r11", source_oper, regctx, InstrList);
	      InstrList.push_back(new instr_t(MNEMO_STR,
					      std::string("r11"),
					      std::string("[r12]")));
	      return;
	    }
	  else
	    {
	      tmp2 = "r11";
	    }
	}
      else
	{
	  tmp2 = "r12";
	}
      // std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      if(tmp2 == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+ tmp_reg + "," + reg_offset + "]"));
      return;
    }
  myAssert(source_oper.type == OPERAND_VAR, ASSERT_EXIT_CODE_250);

  // Special Case: Source is an SPECIAL register [sp/lr/pc]
  // or Source is directly [r#]
  if(is_special_register(source_oper.name) || source_oper.name[0] == 'r')
    {
      myAssert(0, ASSERT_EXIT_CODE_251);
      // Not Implemented yet
      return;
    }

 // If Source is a Global Array ( CASE #4 )
  if(source_oper.name[0] == '$' &&
     source_oper.name[1] == '&')
    {
      myAssert(0, ASSERT_EXIT_CODE_252);
      // Should Not Reach Here
    }

  // If Source is a Global Var ( CASE #5 )
  myAssert(source_oper.name[1] != '&', ASSERT_EXIT_CODE_253);
  if(source_oper.name[0] == '$')
    {
      std::string tmp2 = "";
      if(r12_occ)
	{
	  if(r11_occ)
	    {
	      InstrList.push_back(new instr_t(MNEMO_ADD, std::string("r12"), std::string("r11"), std::string("r12")));
	      AsmGenerator::Load_Reg("r11", source_oper, regctx, InstrList);
	      InstrList.push_back(new instr_t(MNEMO_STR,
					      std::string("r11"),
					      std::string("[r12]")));
	      return;
	    }
	  else
	    {
	      tmp2 = "r11";
	    }
	}
      else
	{
	  tmp2 = "r12";
	}
      // std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
      if(tmp_reg == "r11")
	regctx.r11_guarded = true;
      if(tmp2 == "r11")
	regctx.r11_guarded = true;
      AsmGenerator::Load_Reg(tmp2, source_oper, regctx, InstrList);
      InstrList.push_back(new instr_t(MNEMO_STR,
				      tmp2,
				      "["+ tmp_reg + "," + reg_offset + "]"));
      return;
    }

  myAssert(0, ASSERT_EXIT_CODE_254);
  
}

void AsmGenerator::Load_Memory_Offset(const operand_t &target_oper, std::string offset, int imm_offset, register_ctx_t &regctx, std::list<instr_t *> &InstrList)
// target_oper <- [reg_offset + imm_offset]
{
  myAssert(offset[0] == 'r' || is_special_register(offset), ASSERT_EXIT_CODE_255);
  std::string tmp_reg = offset;

  if(tmp_reg == "r11")
    regctx.r11_guarded = true;
  
  if(target_oper.rewrited_name != "") // Renamed
    {
      // If Target is an Register ( Case #1 )
      if(target_oper.rewrited_name[0] == '#')
	{
	  InstrList.push_back(new instr_t(MNEMO_LDR,
					  "r"+target_oper.rewrited_name.substr(1),
					  "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
	  return;
	}

      // If Target is a Mem Reference ( Case #2 )
      if(target_oper.rewrited_name[0] == '^')
	{
	  std::string tmp2 = tmp_reg == "r12" ? "r11" : "r12";
	  if(tmp_reg == "r11")
	    regctx.r11_guarded = true;
	  InstrList.push_back(new instr_t(MNEMO_LDR,
					  tmp2,
					  "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
	  Store_Stack(tmp2, regctx.stack_arg_size + var_to_int(target_oper.rewrited_name, ASSERT_EXIT_CODE_262), regctx, InstrList);
	  return;
	}
      myAssert(0, ASSERT_EXIT_CODE_256);
    }

  // Special Case:
  if(is_special_register(target_oper.name) || target_oper.name[0] == 'r')
    {
      InstrList.push_back(new instr_t(MNEMO_LDR,
				      target_oper.name,
				      "["+ tmp_reg + std::string((imm_offset != 0)?(",#" + std::to_string(imm_offset)):("")) + "]"));
      // Not Implemented yet
      return;
    }
  myAssert(0, ASSERT_EXIT_CODE_257);
  
}


void AsmGenerator::Load_Reg_Immediate(std::string target_reg, int value, std::list<instr_t *> &InstrList, std::string condition_string)
{
  if(-255 <= value && value < 65536)
    {
      InstrList.push_back(new instr_t(MNEMO_MOV,
				      target_reg,
				      value));
      InstrList.back()->condition_string = condition_string;
    }
  else
    {
      myAssert(condition_string == "", ASSERT_EXIT_CODE_258);
      InstrList.push_back(new instr_t(MNEMO_MOV32,
				      target_reg,
				      value));
      InstrList.back()->condition_string = condition_string;
    }
  
}

bool AsmGenerator::check_fit_5(int value)
{
  if( 0 <= value && value <= 31)
    return true;
  return false;
}


bool AsmGenerator::check_fit_12(int value)
{
  if( -255 <= value && value <= 255)
    return true;
  int absolute = (value>=0?value:-value);
  if(_check_rotate_12(absolute))
      return true;
  return false;
  
}

bool AsmGenerator::check_fit_mem_offset(int value)
{
  return -4096 < value && value < 4096;
}

bool AsmGenerator::_check_rotate_12(int value)
{
  for(int cnt = 0; cnt < 16; cnt++)
    {
      int tmp = AsmGenerator::_rotatel(value, cnt*2);
      if(tmp == (tmp&0xff))
	return true;
    }
  return false;
}

unsigned int AsmGenerator::_rotatel(const unsigned int value, int shift)
{
  if ((shift &= sizeof(value)*8 - 1) == 0)
    return value;
  return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

void instr_t::print(std::ostream& out)
{
  std::stringstream myout;
  if(this->mnemonic == MNEMO_RAW)
    {
      myout << COLOR_YELLOW << this->dest.name << std::endl;
      out << myout.str();
      return;
    }
  switch(this->mnemonic)
    {
#define PCASE(sym, lit)							\
      case sym:								\
 	{								\
	  myout <<"    " << COLOR_CYAN << lit				\
	      << condition_string;					\
	  for(int j = 0; j < 8 - condition_string.size() -  std::string(lit).size(); j++) \
	    myout <<" ";							\
	  break;							\
 	}
      PCASE(MNEMO_ADD,  "ADD");
      PCASE(MNEMO_SUB,  "SUB");
      PCASE(MNEMO_SDIV,  "SDIV");
      PCASE(MNEMO_MOV,  "MOV");
      PCASE(MNEMO_MOV32,"mov32I");
      PCASE(MNEMO_LDR,  "LDR");
      PCASE(MNEMO_STR,  "STR");
      PCASE(MNEMO_MUL,  "MUL");
      PCASE(MNEMO_LSL,  "LSL");
      PCASE(MNEMO_LSR,  "LSR");
      PCASE(MNEMO_ASR,  "ASR");
      PCASE(MNEMO_CMP,  "CMP");
      PCASE(MNEMO_B,    "B");
      PCASE(MNEMO_BL,   "BL" );
      PCASE(MNEMO_MLA,  "MLA");
      PCASE(MNEMO_MLS,  "MLS");
      PCASE(MNEMO_LDM,  "LDM");
      PCASE(MNEMO_STM,  "STM");
#undef PCASE
    case MNEMO_NOTE:
      {
	myout << COLOR_RED <<"## " << COLOR_GREEN << this->dest.name << std::endl;
	out << myout.str();
	return;
      }
    case MNEMO_PASSNOTE:
      {
	myout << COLOR_RED <<"##+" << COLOR_GREEN << this->dest.name << std::endl;
	out << myout.str();
	return;
      }
    case MNEMO_LABEL:
      {
	myout << COLOR_PURPLE << this->dest.name << COLOR_YELLOW ":" << std::endl;
	out << myout.str();
	return;
      }
    case MNEMO_RAW:
      {
	myAssert(0, ASSERT_EXIT_CODE_259);
      }
    case MNEMO_LDR_GLB:
      {
	myAssert(this->ops1.type == OPERAND_VAR, ASSERT_EXIT_CODE_260);
	myAssert(this->ops1.rewrited_name == "", ASSERT_EXIT_CODE_261);
	myAssert(this->ops1.name[0] == '$', ASSERT_EXIT_CODE_262);
	if(!offset_indicator.count(this->ops1.name))
	  {
	    printf("Assertion failed at %s\n", this->ops1.name.c_str());
	    fflush(stdout);
	  }
	myAssert(offset_indicator.count(this->ops1.name), ASSERT_EXIT_CODE_263);
	int offset = this->absolute_offset - offset_indicator[this->ops1.name];
	// if(!print_ASMLine)
	//   myAssert(offset >= 0, ASSERT_EXIT_CODE_264);
	if(offset < 4096) // Use LDR, =[...]
	  myout << COLOR_CYAN "    LDR     ";
	else // Use mov32I
	  myout << COLOR_CYAN "    mov32I  ";
	break;
      }
    case MNEMO_NOP:
      {
	myout << COLOR_RED <<"## " << COLOR_GREEN << "NOP" << std::endl;
	out << myout.str();
	return;
      }
    case MNEMO_BLK_SEP:
      {
	myout << COLOR_RED <<"## " << COLOR_GREEN << "BLK_SEP" << std::endl;
	out << myout.str();
	return;
      }
    }
  std::stringstream ss;
#define F(x, back, pred)			\
   if(this->x.type != OPERAND_NOT_DEFINED)	\
     {						\
       ss << back;				\
       ss << pred;				\
       ss << rename_operand(x);			\
     }						\
   else						\
     {						\
       goto _Asm_print_myout;			\
     }

  F(dest,  " ", "");
  if(this->mnemonic == MNEMO_LDR_GLB)
    {
      int offset = this->absolute_offset - offset_indicator[this->ops1.name];
      if(offset < 4096) // Use LDR, =[...]
      	{
      	  F(ops1, COLOR_RED ", ", "=");
      	}
      else // Use mov32I
	{
	  ss << ", " << rename_operand(this->ops1.name);
	}
    }
  else if(this->mnemonic == MNEMO_MOV32)
    {
      myAssert(ops1.type == OPERAND_IMM, ASSERT_EXIT_CODE_265);
      ss << ", " << COLOR_GREEN << ops1.imm;
    }
  else
    {
      F(ops1, COLOR_RED ", ", "");
    }
  F(ops2, COLOR_RED ", ", "");
  F(ops3, COLOR_RED ", ", "");

#undef F  
 _Asm_print_myout:
  myout << ss.str();
  if(this->line_number != 0)
    {

      std::stringstream tst;
      print_nocolor(tst, myout.str());
      int current_len = tst.str().size();
      int reside = 40 - current_len;
      if(reside <= 0)
	myout << " @ " << this->line_number;
      else
	{
	  for(int i = 0; i < reside; i++)
	    myout << " ";
	  myout << COLOR_GREEN " @ " << this->line_number;
	}
    }
  out<<myout.str()<<std::endl;
  
}
