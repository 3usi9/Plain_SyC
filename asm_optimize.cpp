#include "asm_optimize.h"
#include "asm_flow_optimize.h"
#include <stdint.h>
#include <queue>
extern int print_ASMFlow;
extern int print_OptASMFlow;
extern std::map<std::string, int> argcount;
/////////////////////////////////////////////////////
///////// U T I L I T I E S /////////////////////////
/////////////////////////////////////////////////////
std::list<instr_t*>::iterator skip_blanks(std::list<instr_t*>::iterator q,
					  std::list<instr_t*>::iterator end)
{
  while(q != end &&
	(
	 (*q)->mnemonic == MNEMO_RAW ||
	 (*q)->mnemonic == MNEMO_NOP ||
	 (*q)->mnemonic == MNEMO_NOTE ||
	 (*q)->mnemonic == MNEMO_PASSNOTE ||
	 (*q)->mnemonic == MNEMO_BLK_SEP
	 ))
    q++;
  return q;
}

std::list<instr_t*>::iterator skip_useless_blanks(std::list<instr_t*>::iterator q,
						 std::list<instr_t*>::iterator end)
{
  while(q != end &&
	(
	 (*q)->mnemonic == MNEMO_NOP ||
	 (*q)->mnemonic == MNEMO_NOTE ||
	 (*q)->mnemonic == MNEMO_PASSNOTE ||
	 (*q)->mnemonic == MNEMO_BLK_SEP
	 ))
    q++;
  return q;
}

#define DEF_REG_NAMES(reg_name)			\
  {reg_name, std::string(reg_name).size()}

std::vector< std::pair<std::string, int> > register_names =
  {
    DEF_REG_NAMES("r10"),
    DEF_REG_NAMES("r11"),
    DEF_REG_NAMES("r12"),
    DEF_REG_NAMES("r13"),
    DEF_REG_NAMES("r14"),
    DEF_REG_NAMES("r15"),
    DEF_REG_NAMES("sp"),
    DEF_REG_NAMES("lr"),
    DEF_REG_NAMES("pc"),
    DEF_REG_NAMES("r0"),
    DEF_REG_NAMES("r1"),
    DEF_REG_NAMES("r2"),
    DEF_REG_NAMES("r3"),
    DEF_REG_NAMES("r4"),
    DEF_REG_NAMES("r5"),
    DEF_REG_NAMES("r6"),
    DEF_REG_NAMES("r7"),
    DEF_REG_NAMES("r8"),
    DEF_REG_NAMES("r9"),
  };
#undef DEF_REG_NAMES

std::vector<std::string> get_registers_from_string(const std::string& in)
{
  if(in.size() >= 2 &&
     in[0] == '_' &&
     in[1] == '_')
    return {}; 
  if(in.size() >= 1 &&
     in[0] == '=')
    return {};
  if(in.size() >= 1 &&
     in[0] == '$')
    return {};
  // Global Variables, their name shouldn't been take into consideration
  
  std::vector<std::string> ret;
  for(int i = 0; i < in.size(); i++)
    {
    reg_test_pass:
      for(auto p : register_names)
      	{
      	  if(in.substr(i, p.second) == p.first)
      	    {
	      ret.push_back(p.first);
      	      i += p.second;
	      goto reg_test_pass;
      	    }
	}
    }
  return ret;
}

bool is_memory_access(operand_t oper)
{
  if(oper.name == "")
    return false;
  if(oper.name[0] != '[')
    return false;
  return true;
}

mem_offset_t::mem_offset_t(operand_t oper)
{
  myAssert(oper.name.size() >= 2,ASSERT_EXIT_CODE_147);
  myAssert(oper.name[0] == '[',ASSERT_EXIT_CODE_1);
  int pos = 1;
  auto& liter = oper.name;
  // Read first operand
  while(1)
    {
      if(pos >= liter.size())
	myAssert(0,ASSERT_EXIT_CODE_2);

      if(liter[pos] == ',' ||
	 liter[pos] == ']')
	break;
      pos++;
    }
  std::string current_base = liter.substr(1, pos-1);
  myAssert(is_register(current_base),ASSERT_EXIT_CODE_3);
  this->base = operand_t(current_base);
   if(liter[pos] == ']')
     {
       // printf(COLOR_YELLOW "Extracted informations from %s is [" COLOR_CYAN ,
       // 	      liter.c_str());
       // this->base.print(std::cout);
       // printf( COLOR_YELLOW "] + [" COLOR_CYAN);
       // this->offset.print(std::cout);
       // printf(COLOR_YELLOW "].\n" COLOR_NONE);
       this->offset = operand_t();
       return;
     }
   
   myAssert(liter[pos] == ',',ASSERT_EXIT_CODE_4);
  pos++;
  int npos = pos;
  while(1)
    {
      if(pos >= liter.size())
	myAssert(0,ASSERT_EXIT_CODE_5);

      if(liter[pos] == ',' ||
	 liter[pos] == ']')
	break;
      pos++;
    }
  
  std::string later_base = liter.substr(npos, pos - npos);
  if(!is_register(later_base))
    {
      myAssert(later_base.size() >= 2,ASSERT_EXIT_CODE_6);
      myAssert(later_base[0] == '#',ASSERT_EXIT_CODE_7);
      this->offset = operand_t(std::stoi(later_base.substr(1)));
    }
  else
    {
      this->offset = operand_t(later_base);
    }
  
  // printf(COLOR_YELLOW "Extracted informations from %s is [" COLOR_CYAN ,
  // 	 liter.c_str());
  // this->base.print(std::cout);
  // printf( COLOR_YELLOW "] + [" COLOR_CYAN);
  // this->offset.print(std::cout);
  // printf(COLOR_YELLOW "].\n" COLOR_NONE);
  return;
}

std::string mem_offset_t::get_literal()
{
  std::string out = "[";
  myAssert(is_register(this->base.name),ASSERT_EXIT_CODE_8);
  out += this->base.name;
  if(this->offset.get_static_type() == STATIC_NONE)
    {
      out += "]";
      return out;
    }
  if(this->offset.get_static_type() == STATIC_IMM)
    {
      out += ",#" + std::to_string(this->offset.imm) + "]";
      return out;
    }
  else
    {
      myAssert(is_register(this->offset.name),ASSERT_EXIT_CODE_9);
      out += "," + this->offset.name + "]";
      return out;
    }
  myAssert(0,ASSERT_EXIT_CODE_10);
}

void _asm_ExpressionSet_t::Invalidate_Instr(instr_t& asmop)
{
  auto kill_set = Get_Killed_vars(asmop);
  auto p = this->definition_expression.begin();
  while(1)
    {
      if(p == this->definition_expression.end())
	return;
      // Check if (*p) contains any killed var in [asmop]
      instr_t tmp_instr = *p->second.second;
      auto p_Killed_Set = Get_Killed_vars(tmp_instr);
      auto p_Referenced_Set = Get_Referenced_vars(tmp_instr);
      if(!mySetIntersect(kill_set, p_Killed_Set).empty() ||
	 !mySetIntersect(kill_set, p_Referenced_Set).empty())
	{
	  p = this->definition_expression.erase(p);
	}
      else
	{
	  p++;
	}
    }
}

int _asm_ExpressionSet_t::Get_Invalidate_Distance(std::string rewrite_var, int current_line)
{
  int dist = 2147483645;
  auto p = this->definition_expression.begin();
  while(1)
    {
      if(p == this->definition_expression.end())
	return dist;
      // Check if (*p) contains any killed var in [asmop]
      instr_t tmp_instr = *p->second.second;
      auto p_Killed_Set = Get_Killed_vars(tmp_instr);
      auto p_Referenced_Set = Get_Referenced_vars(tmp_instr);
      if(p_Killed_Set.count(rewrite_var) ||
	 p_Referenced_Set.count(rewrite_var))
	{
	  dist = std::min(dist, current_line - p->second.second->line_number);
	  p++;
	}
      else
	{
	  p++;
	}
    }
}

bool Can_Be_Cached(instr_t& asmop)
{
  if(asmop.condition_string != "")
    return false;
  if(asmop.dest.name == "pc")
    return false; // MOV PC, LR
  switch(asmop.mnemonic)
    {

    case MNEMO_ADD:         case MNEMO_SUB:
    case MNEMO_SDIV:        case MNEMO_MOV:     
    case MNEMO_MOV32:       case MNEMO_LDR_GLB: 
    case MNEMO_MUL:         case MNEMO_LSL:
    case MNEMO_LSR:         case MNEMO_ASR:
    case MNEMO_MLA:         case MNEMO_MLS:
      {

	return true;
      }
    case MNEMO_RAW:         case MNEMO_LDR:     
    case MNEMO_STR:         case MNEMO_LDM:     
    case MNEMO_STM:         case MNEMO_CMP:     
    case MNEMO_B:           case MNEMO_BL:      
    case MNEMO_NOP:         case MNEMO_LABEL:   
    case MNEMO_NOTE:        case MNEMO_PASSNOTE:
    case MNEMO_BLK_SEP: 
      {
	return false;
      }
    default:
      {
	myAssert(0,ASSERT_EXIT_CODE_11);
      }
    }
  myAssert(0,ASSERT_EXIT_CODE_12);
}



void _asm_ExpressionSet_t::Try_To_Rewrite(instr_t& asmop, LiveRangeSet_t& lrset)
{
  // printf("Try to rewriting:");
  // asmop.print(std::cerr);
  // First, Invalidate its killed vars
  this->Invalidate_Instr(asmop);
  
  // Then, Check if its [rhs] satisfy our requirements, not all [rhs] satisfies.
  if(Can_Be_Cached(asmop))
    {
      _asm_rhs_t asmop_rhs = asmop.get_rhs();
      myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_13);
      // If found previous assignment, try to combine them
      // Else, insert it into assignment list


      if(this->definition_expression.count(asmop_rhs)) // Might can be rewritten
      	{
      	  auto liter = this->definition_expression[asmop_rhs].first;
      	  instr_t* src_instr = this->definition_expression[asmop_rhs].second;
      	  myAssert(is_register(src_instr->dest.name),ASSERT_EXIT_CODE_14);
      	  myAssert(src_instr->dest.name == liter,ASSERT_EXIT_CODE_15);
      	  LiveRange_t* outer = lrset.get_Defined_LiveRange(src_instr->line_number,
      							   liter);
      	  LiveRange_t* inner = lrset.get_Defined_LiveRange(asmop.line_number, asmop.dest.name);
      	  // If able, replace
      	  // If not able, continue
      	  if(lrset.Check_Available_Replacable(inner, outer))
      	    {
      	      inner->rewrite_reference(outer->representative_reg_name);
      	    }
      	  else
      	    {
      	      return;
      	    }

      	}
      else // Should insert to definition_expression
      	{
      	  this->definition_expression.insert({asmop_rhs, {asmop.dest.name, &asmop}});
      	}
    }
  else // Cannot be cached, skip
    {
      return;
    }
}

void _asm_ExpressionSet_t::Proceed(asm_basic_block_t* blk, LiveRangeSet_t& lrset)
{
  for(auto p = blk->begin;
      p != blk->end;
      p++)
    {
      // this->Try_Rename_Register(**p, lrset);
      this->Try_To_Rewrite(**p, lrset);
    }
}
/////////////////////////////////////////////////////
///////// F L O W - A N A L Y Z E ///////////////////
/////////////////////////////////////////////////////
asm_basic_block_t::asm_basic_block_t(std::list<instr_t*>::iterator begin,
				     std::list<instr_t*>::iterator end)
  : begin(begin), end(end)
{
  static int blkid = 0;
  this->id = blkid++;
}

void asm_basic_block_t::print(std::ostream &out)
{
  {
    std::stringstream ss;
    ss<<COLOR_RED "ASM " COLOR_WHITE "Basic Block " COLOR_CYAN + std::to_string(this->id);

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
	myAssert(it != "",ASSERT_EXIT_CODE_16);			\
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
  // for(auto p : this->liveOut)
  //   {
  //     printf("Plain LiveOut: [%s]\n", p.c_str());
  //   }
  PROVIDE_AND_REQUIRE(this->liveOut, "LiveOut: ");
  PROVIDE_AND_REQUIRE(this->liveIn, "LiveIn: ");
  PROVIDE_AND_REQUIRE(this->AvailIn, "AvailIn: ");
  for(auto it = this->begin;
      it != this->end;
      ++it)
    {
      std::cerr<< COLOR_CYAN "├──";
      (*it)->print(std::cerr);
    }
  IRSep_t sp;
  sp.print(out, 0);
}

void asm_basic_block_t::AddEdge(asm_basic_block_t* from,
				asm_basic_block_t* to)
{
  from->out_edges.push_back(to);
  to  -> in_edges.push_back(from);
}

std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Killed_Position(instr_t& asmop)
{
    switch(asmop.mnemonic)
    {
      // [dest]
    case MNEMO_ADD:     case MNEMO_SUB:
    case MNEMO_SDIV:    
    case MNEMO_MOV32:   case MNEMO_LDR:
    case MNEMO_LDR_GLB: case MNEMO_MUL:
    case MNEMO_LSL:     case MNEMO_LSR:
    case MNEMO_ASR:     case MNEMO_MLA:
    case MNEMO_MLS:
      {
	myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_17);
	std::vector<std::pair<std::string, std::string*>> ve;
	ve.push_back({asmop.dest.name, &asmop.dest.name});
	return {true, ve};
      }
    case MNEMO_RAW:     case MNEMO_STR:
    case MNEMO_STM:     case MNEMO_CMP:
    case MNEMO_B:       case MNEMO_NOP:
    case MNEMO_LABEL:   case MNEMO_NOTE:
    case MNEMO_PASSNOTE:case MNEMO_BLK_SEP:
      {
	return {true, {}};
      }
    case MNEMO_LDM:
      {
	myAssert(asmop.ops1.name != "",ASSERT_EXIT_CODE_18);
	myAssert(asmop.ops1.name[0] == '{',ASSERT_EXIT_CODE_19);
	std::vector<std::pair<std::string, std::string*>> ve;
	auto li = get_registers_from_string(asmop.ops1.name);
	for(auto it : li)
	  {
	    ve.push_back({it, &asmop.ops1.name});
	  }
	return {false, ve};
	// LDM killed vars SHOULDN'T be renamed.
      }
    case MNEMO_BL:
      {
	return {false, {{"r0",NULL},{"r1",NULL},
			{"r2",NULL},{"r3",NULL},{"r12",NULL}}};
      }
    case MNEMO_MOV:
      {
	myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_20);
	if(asmop.dest.name == "pc")
	  return {false, {}};
	else
	  {
	    std::vector<std::pair<std::string, std::string*>> ve;
	    ve.push_back({asmop.dest.name, &asmop.dest.name});
	    if(asmop.condition_string == "")
	      return {true, ve};
	    else
	      return {false, ve};
	  }
      }
    }
    myAssert(0,ASSERT_EXIT_CODE_21);

}

std::set<std::string> Get_Killed_vars(const instr_t& asmop)
{
  switch(asmop.mnemonic)
    {
      // [dest]
    case MNEMO_ADD:     case MNEMO_SUB:
    case MNEMO_SDIV:    
    case MNEMO_MOV32:   case MNEMO_LDR:
    case MNEMO_LDR_GLB: case MNEMO_MUL:
    case MNEMO_LSL:     case MNEMO_LSR:
    case MNEMO_ASR:     case MNEMO_MLA:
    case MNEMO_MLS:
      {
	if(!is_register(asmop.dest.name))
	  {
	    printf("myAssert Failed at:");
	    instr_t asm2 = asmop;
	    asm2.print(std::cerr);
	  }
	myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_22);
	return {asmop.dest.name};
      }
    case MNEMO_RAW:     case MNEMO_STR:
    case MNEMO_STM:     case MNEMO_CMP:
    case MNEMO_B:       case MNEMO_NOP:
    case MNEMO_LABEL:   case MNEMO_NOTE:
    case MNEMO_PASSNOTE:case MNEMO_BLK_SEP:
      {
	return {};
      }
    case MNEMO_LDM:
      {
	myAssert(asmop.ops1.name != "",ASSERT_EXIT_CODE_23);
	myAssert(asmop.ops1.name[0] == '{',ASSERT_EXIT_CODE_24);
	auto li = get_registers_from_string(asmop.ops1.name);
	return std::set<std::string>(li.begin(), li.end());
      }
    case MNEMO_BL:
      {
	return {"r0","r1","r2","r3","r12"};
      }
    case MNEMO_MOV:
      {
	myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_25);
	if(asmop.dest.name == "pc")
	  return {};
	else
	  return {asmop.dest.name};
      }
    }
  myAssert(0,ASSERT_EXIT_CODE_26);
}

std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Referenced_Position(instr_t& asmop)
// Return value: [ rewritable, rewriting_position<regname, position> ]
{
  switch(asmop.mnemonic)
    {
    case MNEMO_MOV:
      {
	if(asmop.dest.name == "pc")
	  return {false, {{"r0",NULL}, {"r4",NULL}, {"r5",NULL},
			  {"r6",NULL}, {"r7",NULL}, {"r8",NULL},
			  {"r9",NULL}, {"r10",NULL}, {"r11",NULL},
			  {"lr",NULL}, {"sp",NULL}}};
	// [MOV pc, lr] is NOT REWRITABLE
      }
    case MNEMO_LDR:
    case MNEMO_ADD:     case MNEMO_SUB:
    case MNEMO_SDIV:    
    case MNEMO_MOV32:   
    case MNEMO_LDR_GLB: case MNEMO_MUL:
    case MNEMO_LSL:     case MNEMO_LSR:
    case MNEMO_ASR:     case MNEMO_MLA:
    case MNEMO_MLS:
      {
	std::vector<std::pair<std::string, std::string*>> ve;
#define TEST_OP(oper)						\
	if(oper.name != "")					\
	  {							\
	    auto li = get_registers_from_string(oper.name);	\
	    for(auto it : li)					\
	      {							\
		ve.push_back({it, &oper.name});			\
	      }							\
	  }
	TEST_OP(asmop.ops1);
	TEST_OP(asmop.ops2);
	TEST_OP(asmop.ops3);
	return {true, ve};
      }
    case MNEMO_RAW:     case MNEMO_LABEL:
    case MNEMO_NOTE:    case MNEMO_PASSNOTE:
    case MNEMO_B:       case MNEMO_NOP:
    case MNEMO_BLK_SEP:
      {
	return {false, {}};
      }
    case MNEMO_STR:     case MNEMO_STM: 
    case MNEMO_CMP:    
      {
	std::vector<std::pair<std::string, std::string*>> ve;
	TEST_OP(asmop.dest);
	TEST_OP(asmop.ops1);
	TEST_OP(asmop.ops2);
	TEST_OP(asmop.ops3);
	return {true, ve};
      }

    case MNEMO_LDM:
      {
	std::vector<std::pair<std::string, std::string*>> ve;
	TEST_OP(asmop.dest);
	return {true, ve};

      }
    case MNEMO_BL:
      {
	myAssert(argcount.count(asmop.dest.name),ASSERT_EXIT_CODE_27);
	if(argcount[asmop.dest.name] == 0)
	  return {false, {{"sp",NULL}}};
	else if(argcount[asmop.dest.name] == 1)
	  return {false, {{"r0",NULL},{"sp",NULL}}};
	else if(argcount[asmop.dest.name] == 2)
	  return {false, {{"r0",NULL},{"r1",NULL},{"sp",NULL}}};
	else if(argcount[asmop.dest.name] == 3)
	  return {false, {{"r0",NULL},{"r1",NULL},
			  {"r2",NULL},{"sp",NULL}}};
	else
	  return {false, {{"r0",NULL},{"r1",NULL},
			  {"r2",NULL},{"r3",NULL},{"sp",NULL}}};
      }
    }
  myAssert(0,ASSERT_EXIT_CODE_28);
#undef TEST_OP
}

std::set<std::string> Get_Referenced_vars(const instr_t& asmop)
{
  // {
  //   printf("Getting referenced vars for:");
  //   auto asm2 = asmop;
  //   asm2.print(std::cerr);
  // }
  switch(asmop.mnemonic)
    {
    case MNEMO_MOV:
      {
	if(asmop.dest.name == "pc")
	  return {"r0", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "lr", "sp"};
      }
    case MNEMO_LDR:
    case MNEMO_ADD:     case MNEMO_SUB:
    case MNEMO_SDIV:    
    case MNEMO_MOV32:   
    case MNEMO_LDR_GLB: case MNEMO_MUL:
    case MNEMO_LSL:     case MNEMO_LSR:
    case MNEMO_ASR:     case MNEMO_MLA:
    case MNEMO_MLS:
      {
	std::set<std::string> se;
#define TEST_OP(oper)						\
	if(oper.name != "")					\
	  {							\
	    auto li = get_registers_from_string(oper.name);	\
	    se.insert(li.begin(), li.end());			\
	  }
	TEST_OP(asmop.ops1);
	TEST_OP(asmop.ops2);
	TEST_OP(asmop.ops3);
	return se;
      }
    case MNEMO_RAW:     case MNEMO_LABEL:
    case MNEMO_NOTE:    case MNEMO_PASSNOTE:
    case MNEMO_B:       case MNEMO_NOP:
    case MNEMO_BLK_SEP:
      {
	return {};
      }
    case MNEMO_STR:     case MNEMO_STM:
    case MNEMO_CMP:    
      {
	std::set<std::string> se;
	TEST_OP(asmop.dest);
	TEST_OP(asmop.ops1);
	TEST_OP(asmop.ops2);
	TEST_OP(asmop.ops3);
	return se;
      }
    case MNEMO_LDM:
      {
	std::set<std::string> se;
	TEST_OP(asmop.dest);
	return se;

      }
    case MNEMO_BL:
      {
	if(!argcount.count(asmop.dest.name))
	  {
	    printf("Assertion Failed on name=%s\n", asmop.dest.name.c_str());
	    fflush(stdout);
	  }
	myAssert(argcount.count(asmop.dest.name),ASSERT_EXIT_CODE_29);
	if(argcount[asmop.dest.name] == 0)
	  return {"sp"};
	else if(argcount[asmop.dest.name] == 1)
	  return {"r0","sp"};
	else if(argcount[asmop.dest.name] == 2)
	  return {"r0","r1","sp"};
	else if(argcount[asmop.dest.name] == 3)
	  return {"r0","r1", "r2","sp"};
	else
	  return {"r0","r1", "r2","r3","sp"};
      }
    }
  myAssert(0,ASSERT_EXIT_CODE_30);
#undef TEST_OP
}

void asm_basic_block_t::get_register_dependency()
{
  this->UEVar.clear();
  this->VarKill.clear();
  for(auto it = this->begin;
      it != this->end;
      ++it)
    {
      auto& asmop = **it;
      auto killed = Get_Killed_vars(asmop);
      auto referenced = Get_Referenced_vars(asmop);
      for(auto it : referenced)
	{
	  if(!this->VarKill.count(it))
	    this->UEVar.insert(it);
	}
      for(auto it : killed)
	{
	  this->VarKill.insert(it);
	}
    }
}

bool asm_basic_block_t::compute_liveout()
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

void asm_basic_block_t::Finalize_LiveIn()
{
  this->liveIn = mySetUnion(this->UEVar,
			    mySetDifference(this->liveOut, this->VarKill));
}

LiveRange_t::LiveRange_t(int left, int right, std::string reg_name) :
  left(left), right(right), rewritable(true), representative_reg_name(reg_name), defined_position(NULL), 
  referenced_position({})
{
}

bool RegReplace(std::string& str, const std::string& pattern, const std::string& target)
{
  if(str[0] == '$')
    myAssert(0,ASSERT_EXIT_CODE_31);
  if(str.size() >= 2 &&
     str[0] == '_' &&
     str[1] == '_')
    return false;

  int pattern_len = pattern.size();
  bool replaced = false;
  for(int i = 0; i < str.size(); i++)
    {
      if(str.substr(i, pattern_len) == pattern &&
	 (str.begin()+i+pattern_len == str.end() ||
	  !isdigit(str[i+pattern_len])))
	{
	  str.replace(str.begin()+i, str.begin()+i+pattern_len, target);
	  replaced = true;
	}
    }
  return replaced;
}

void LiveRange_t::rewrite_reference(std::string rewrited)
{
  myAssert(this->rewritable,ASSERT_EXIT_CODE_32);
  for(auto it : this->referenced_position)
    {
      myAssert(RegReplace(*it,
			this->representative_reg_name,
			  rewrited),ASSERT_EXIT_CODE_33);
    }

}

void LiveRange_t::rewrite_definition(std::string rewrited)
{
  // printf("Rewrite Definition: [%d, %d,ASSERT_EXIT_CODE_22](%s) -> (%s)\n",
  // 	 left, right, representative_reg_name.c_str(),
  // 	 rewrited.c_str());
  // printf("Definition String is %s\n", this->defined_position->c_str());
  // fflush(stdout);

  myAssert(this->rewritable,ASSERT_EXIT_CODE_23);
  myAssert(this->defined_position != NULL,ASSERT_EXIT_CODE_24);
  myAssert(RegReplace(*this->defined_position,
		    this->representative_reg_name,
		      rewrited),ASSERT_EXIT_CODE_25);
}

LiveRangeSet_t::LiveRangeSet_t(const asm_basic_block_t& asmbb)
{
  {
    // asm_basic_block_t asmbc = asmbb;
    // asmbc.print(std::cerr);
  }
  std::map<std::string, LiveRange_t* > current_LiveRange;
  for(auto it : {"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","sp","lr"})
  {
  }
    
  for(auto it : asmbb.liveIn)
    {
      LiveRange_t tmplr(0, 0, it);

      tmplr.rewritable = false;
      // The variables from liveIn SHOULDN'T been renamed

      current_LiveRange.insert({it, new LiveRange_t(tmplr)});
      // These variables' [defined_position] is NULL
    }
     
  // Forward scan this block.
  for(auto p = asmbb.begin;
      p != asmbb.end;
      p++)
    {
      // Update current_LiveRange
      // If this instruction referenced some vars, update their LiveRange
      auto ref_var = Get_Referenced_Position(**p);
      for(auto it_var : ref_var.second)
	{
	  auto reg_name = it_var.first;
	  auto reg_position = it_var.second;
	  myAssert(current_LiveRange.count(reg_name),ASSERT_EXIT_CODE_26);
	  myAssert(current_LiveRange[reg_name]->representative_reg_name == reg_name,ASSERT_EXIT_CODE_27);
	  current_LiveRange[reg_name]->rewritable &= ref_var.first;
	  current_LiveRange[reg_name]->referenced_position.push_back(reg_position);
	  current_LiveRange[reg_name]->right = (*p)->line_number;
	}
      // If this instruction killed some vars, eliminate their LiveRanges
      auto killed_var = Get_Killed_Position(**p);
      for(auto it_var : killed_var.second)
	{
	  auto reg_name = it_var.first;
	  auto reg_position = it_var.second;
	  if(current_LiveRange.count(reg_name))
	    {
	      if(current_LiveRange[reg_name]->left != current_LiveRange[reg_name]->right)
		// Not a dead kill
		{
		  myAssert(current_LiveRange[reg_name]->representative_reg_name == reg_name,ASSERT_EXIT_CODE_28);
		  if(!this->LR_Array.count(reg_name))
		    this->LR_Array[reg_name] = {};
		  this->LR_Array[reg_name].insert(current_LiveRange[reg_name]);
		}
	      else // Dead Kill should also be taken into consideration
		{
		  myAssert(current_LiveRange[reg_name]->representative_reg_name == reg_name,ASSERT_EXIT_CODE_29);
		  if(!this->LR_Array.count(reg_name))
		    this->LR_Array[reg_name] = {};
		  this->LR_Array[reg_name].insert(current_LiveRange[reg_name]);
		}
	      // Refresh [reg_name]'s liveRange
	      LiveRange_t tmplr((*p)->line_number, (*p)->line_number, reg_name);
	      tmplr.defined_position = reg_position;
	      tmplr.rewritable &= killed_var.first;
	      current_LiveRange[reg_name] = new LiveRange_t(tmplr);
	    }
	  else
	    {
	      // Refresh [reg_name]'s liveRange
	      LiveRange_t tmplr((*p)->line_number, (*p)->line_number, reg_name);
	      tmplr.defined_position = reg_position;
	      tmplr.rewritable &= killed_var.first;
	      current_LiveRange[reg_name] = new LiveRange_t(tmplr);
	    }
	}
    }

  // Finalize LR Calculation
  for(auto it_var : asmbb.liveOut)
    {
      myAssert(current_LiveRange.count(it_var),ASSERT_EXIT_CODE_30);
      myAssert(current_LiveRange[it_var]->representative_reg_name == it_var,ASSERT_EXIT_CODE_31);
      current_LiveRange[it_var]->right = INT32_MAX;
      current_LiveRange[it_var]->rewritable = false;
    }
  for(auto it_var : current_LiveRange)
    {
      // if(it_var.second->left != it_var.second->right)
	{
	  if(!this->LR_Array.count(it_var.first))
	    this->LR_Array.insert({it_var.first, {}});
	  this->LR_Array[it_var.first].insert(it_var.second);
	}
    }
  std::vector<std::string> regs = {"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","sp","lr"};
  for(auto it : regs)
  {
    if(!this->LR_Array.count(it))
      this->LR_Array.insert({it,{}});
  }

  for(auto it1 : this->LR_Array)
    {
      for(auto it2 : it1.second)
	{
	  // printf("LiveRange for %s is [%d, %d]\n", it1.first.c_str(), it2->left, it2->right);
	}
    }
}

LiveRange_t* LiveRangeSet_t::get_Defined_LiveRange(int position, std::string regname)
// only valid for [Killed] vars
{
  LiveRange_t* lrptr = NULL;
  if(!this->LR_Array.count(regname))
    this->LR_Array.insert({regname, {}});
  for(auto it : this->LR_Array[regname])
    {
      if(it->left == position)
  	{
  	  myAssert(lrptr == NULL,ASSERT_EXIT_CODE_32);
  	  lrptr = it;
  	}
    }
  return lrptr;
}

LiveRange_t* LiveRangeSet_t::get_Reference_LiveRange(int position, std::string regname)
{
  LiveRange_t* lrptr = NULL;
  if(!this->LR_Array.count(regname))
    this->LR_Array.insert({regname, {}});
  for(auto it : this->LR_Array[regname])
    {
      if(it->left < position && position <= it->right)
  	{
  	  myAssert(lrptr == NULL,ASSERT_EXIT_CODE_33);
  	  lrptr = it;
  	}
    }
  return lrptr;
}
  
bool LiveRangeSet_t::Check_Alias_Replacable(const instr_t& asmop)
{
  myAssert(asmop.mnemonic == MNEMO_MOV,ASSERT_EXIT_CODE_34);
  if(asmop.ops1.get_static_type() == STATIC_IMM)
    return false;
  // Move IMMEDIATE is not an alias for any register.
  if(asmop.dest.name == "pc")
    return false; // MOV PC, LR
  myAssert(is_register(asmop.dest.name),ASSERT_EXIT_CODE_35);
  myAssert(is_register(asmop.ops1.name),ASSERT_EXIT_CODE_36);
  LiveRange_t* inner = this->get_Defined_LiveRange(asmop.line_number, asmop.dest.name);
  LiveRange_t* outer = this->get_Reference_LiveRange(asmop.line_number, asmop.ops1.name);
  myAssert(inner != NULL,ASSERT_EXIT_CODE_37);
  if(outer == NULL)
    {
      return false; // Double substitution.
      // printf("myAssert Failed at:");
      // instr_t asm2 = asmop;
      // asm2.print(std::cerr);
    }
  myAssert(outer != NULL,ASSERT_EXIT_CODE_34);
  if(inner->rewritable == false)
    return false;
  int left = inner->left;
  int right = inner->right;
  // If [left, right) interferes to any LiveRange for [asmop.ops1], the LiveRange should be [outer]
  myAssert(this->LR_Array.count(asmop.ops1.name),ASSERT_EXIT_CODE_35);
  for(auto it : this->LR_Array[asmop.ops1.name])
    {
      if(CHECK_CROSS(left, right-1, it->left, it->right))
	{
	  if(it != outer)
	    return false;
	}
    }
  // [inner] is an alias of [outer], and can be replaced by [outer]
  return true;
}

bool LiveRangeSet_t::Check_Available_Replacable(LiveRange_t* inner, LiveRange_t* outer)
{
  myAssert(inner != NULL,ASSERT_EXIT_CODE_36);
  if(outer == NULL)
    {
      return false; // Double substitution.
    }
  myAssert(outer != NULL,ASSERT_EXIT_CODE_37);
  if(inner->rewritable == false)
    return false;
  int left = inner->left;
  int right = inner->right;
  // If [left, right) interferes to any LiveRange for [asmop.ops1], the LiveRange should be [outer]
  myAssert(this->LR_Array.count(outer->representative_reg_name),ASSERT_EXIT_CODE_38);
  for(auto it : this->LR_Array[outer->representative_reg_name])
    {
      if(CHECK_CROSS(left, right-1, it->left, it->right))
	{
	  if(it != outer)
	    return false;
	}
    }
  // [inner] is an alias of [outer], and can be replaced by [outer]
  return true;
}

bool LiveRangeSet_t::Check_Interleave(LiveRange_t* A, LiveRange_t* B)
{
  int& l1 = A->left;
  int& r1 = A->right;
  int& l2 = B->left;
  int& r2 = B->right;
  return (CHECK_CROSS(l1, r1, l2, r2));
  // Edge is considered to avoid [MOV r0, r0, r1] as stall
}

// bool LiveRangeSet_t::Check_Interleave(LiveRange_t *lr, std::string regname)
// {
//   LiveRange_t* asmlr = lr;
//   myAssert(asmlr != NULL,ASSERT_EXIT_CODE_39);
//   myAssert(asmlr->rewritable,ASSERT_EXIT_CODE_40);
//   std::vector<std::string> st;
//   bool interleaved = false;
//   for(auto range : this->LR_Array[regname])
//     {
//       if(Check_Interleave(asmlr, range))
// 	{
// 	  interleaved = true;
// 	  break;
// 	}
//     }
//   return interleaved;
// }

std::set<std::string> LiveRangeSet_t::Get_Non_Interleaved_RegisterSet(instr_t &asmop)
{
  auto kill_set = Get_Killed_Position(asmop);
  
  if(kill_set.first == false)
    return {};
  // Cannot been replaced
  if(kill_set.second.size() != 1)
    return {};

  // Only single-rewriting can be replaced
  std::string reg_name =kill_set.second.front().first;
  LiveRange_t* asmlr = this->get_Defined_LiveRange(asmop.line_number, reg_name);
  if(asmlr == NULL) // Non-use interval
    return {};
  
  myAssert(asmlr != NULL,ASSERT_EXIT_CODE_41);

  if(!asmlr->rewritable)
    return {};
  std::set<std::string> st;
  for(auto it : this->LR_Array)
    {
      if(it.first == "lr")
	continue; // Don't use this register
      // printf("Checking %s <-> %s\n", reg_name.c_str(), it.first.c_str());
      bool not_interleaved = true;
      for(auto range : it.second)
	{
	  if(Check_Interleave(asmlr, range))
	    {
	      // printf("Checking %s <-> %s conflict at  (%s,ASSERT_EXIT_CODE_38)[%d, %d]\n",
	      // 	     reg_name.c_str(),
	      // 	     it.first.c_str(),
	      // 	     it.first.c_str(),
	      // 	     range->left,
	      // 	     range->right);
	      not_interleaved = false;
	      break;
	    }
	}
      if(not_interleaved && it.first != "r14" && it.first != "lr")
	{
	  st.insert(it.first);
	}
    }
  if(!st.empty())
    {
      // printf("All available registers for:");
      // asmop.print(std::cerr);
      // for(auto it : st)
      // 	{
      // 	  std::cerr<<it<<" ";
      // 	}
      // std::cerr<<std::endl;
      return st;
    }
  return {};
}

std::string LiveRangeSet_t::Get_Non_Interleaved_Register(instr_t &asmop)
{
  auto kill_set = Get_Killed_Position(asmop);
  
  if(kill_set.first == false)
    return "";
  // Cannot been replaced
  if(kill_set.second.size() != 1)
    return "";

  // Only single-rewriting can be replaced
  std::string reg_name =kill_set.second.front().first;
  LiveRange_t* asmlr = this->get_Defined_LiveRange(asmop.line_number, reg_name);
  if(asmlr == NULL) // Non-use interval
    return "";
  
  myAssert(asmlr != NULL,ASSERT_EXIT_CODE_39);

  if(!asmlr->rewritable)
    return "";
  std::vector<std::string> st;
  for(auto it : this->LR_Array)
    {
      if(it.first == "lr")
	continue; // Don't use this register
      // printf("Checking %s <-> %s\n", reg_name.c_str(), it.first.c_str());
      bool not_interleaved = true;
      for(auto range : it.second)
	{
	  if(Check_Interleave(asmlr, range))
	    {
	      not_interleaved = false;
	      break;
	    }
	}
      if(not_interleaved && it.first != "r14" && it.first != "lr")
	{
	  st.push_back(it.first);
	}
    }
  if(!st.empty())
    {
      // printf("All available registers for:");
      // asmop.print(std::cerr);
      // for(auto it : st)
      // 	{
      // 	  std::cerr<<it<<" ";
      // 	}
      // std::cerr<<std::endl;
      return st[rand() % st.size()];
    }
  return "";
}

void _asm_ExpressionSet_t::Try_Rename_Register(instr_t& asmop, LiveRangeSet_t& lrset)
{
  printf("Starting search at instruction:");
  asmop.print(std::cout);
  fflush(stdout);
  auto literSet = lrset.Get_Non_Interleaved_RegisterSet(asmop);
  printf("LiterSet is:");
  for(auto it : literSet)
    {
      std::cout<<it<<" ";
    }
  printf("\n");
  if(literSet.empty())
    return;
  std::string liter = "";
  int min_dist = 2147483647;

  for(auto it : literSet)
    {
      if(Get_Invalidate_Distance(it, asmop.line_number) < min_dist)
	liter = it;
    }
  myAssert(liter != "",ASSERT_EXIT_CODE_40);
  if(liter != "")
    {
      auto kill_set = Get_Killed_Position(asmop);
      myAssert(kill_set.second.size() == 1,ASSERT_EXIT_CODE_41);
      std::string reg_name =kill_set.second.front().first;
      if(liter == reg_name)
	return;
      // printf("Trying to rewrite %s -> %s at instruction:", reg_name.c_str(), liter.c_str());
      // asmop.print(std::cout);
      // fflush(stdout);
      LiveRange_t* asmlr = lrset.get_Defined_LiveRange(asmop.line_number, reg_name);
      // Make Rewrite Operation
      asmlr->rewrite_definition(liter);
      asmlr->rewrite_reference (liter);
      asmlr->representative_reg_name = liter;
      // Change LRSet
      myAssert(lrset.LR_Array.count(reg_name),ASSERT_EXIT_CODE_42);
      myAssert(lrset.LR_Array.count(liter),ASSERT_EXIT_CODE_43);
      auto p = lrset.LR_Array[reg_name].find(asmlr);
      myAssert(p != lrset.LR_Array[reg_name].end(),ASSERT_EXIT_CODE_44);
      myAssert(!lrset.LR_Array[liter].count(*p),ASSERT_EXIT_CODE_45);
      lrset.LR_Array[liter].insert(*p);
      lrset.LR_Array[reg_name].erase(p);
      // printf("Rewrite Finished.\n");
      return;
    }
  // printf("Not Found.\n");
  return;
}

void LiveRangeSet_t::Try_Rename_Register(instr_t& asmop)
{
  // printf("Starting search at instruction:");
  // asmop.print(std::cout);
  // fflush(stdout);

  auto liter = this->Get_Non_Interleaved_Register(asmop);
  if(liter != "")
    {
      auto kill_set = Get_Killed_Position(asmop);
      myAssert(kill_set.second.size() == 1,ASSERT_EXIT_CODE_46);
      std::string reg_name =kill_set.second.front().first;
      if(liter == reg_name)
	return;
      // printf("Trying to rewrite %s -> %s at instruction:", reg_name.c_str(), liter.c_str());
      // asmop.print(std::cout);
      // fflush(stdout);
      LiveRange_t* asmlr = this->get_Defined_LiveRange(asmop.line_number, reg_name);
      // Make Rewrite Operation
      asmlr->rewrite_definition(liter);
      asmlr->rewrite_reference (liter);
      asmlr->representative_reg_name = liter;
      // Change LRSet
      myAssert(this->LR_Array.count(reg_name),ASSERT_EXIT_CODE_47);
      myAssert(this->LR_Array.count(liter),ASSERT_EXIT_CODE_48);
      auto p = this->LR_Array[reg_name].find(asmlr);
      myAssert(p != this->LR_Array[reg_name].end(),ASSERT_EXIT_CODE_49);
      myAssert(!this->LR_Array[liter].count(*p),ASSERT_EXIT_CODE_50);
      this->LR_Array[liter].insert(*p);
      this->LR_Array[reg_name].erase(p);
      // printf("Rewrite Finished.\n");
      return;
    }
  // printf("Not Found.\n");
  return;
}
/////////////////////////////////////////////////////
///////// I N S T R U C T I O N//////////////////////
///////////////S C H E D U L I N G///////////////////
/////////////////////////////////////////////////////

int _check_interfere(instr_t& instr1,
		    instr_t& instr2)
// If two instruction has no interfence, return -1
// Else, return the minimum instruction count between these two instruction
// that does not introduce interfence.
{
  // printf("Check Interfere:\n");
  // instr1.print(std::cerr);
  // instr2.print(std::cerr);

  // 1. Collect banned instructions
  //   These instruction interferes to any other instructions
  //   (as execution barrier)
#define CHECK_MNEMONIC(mnemo)			\
  if(mnemo == MNEMO_B ||			\
     mnemo == MNEMO_BL ||			\
     mnemo == MNEMO_CMP ||			\
     mnemo == MNEMO_LDM ||			\
     mnemo == MNEMO_STM ||			\
     mnemo == MNEMO_LABEL ||			\
     mnemo == MNEMO_RAW)			\
    {						\
      return 0;					\
    }
  CHECK_MNEMONIC(instr1.mnemonic);
  CHECK_MNEMONIC(instr2.mnemonic);
#undef CHECK_MNEMONIC
  // 2. If an instruction is executed conditionally, it's interfere
  // to any other instruction
  if(instr1.condition_string != "" ||
     instr2.condition_string != "")
    {
      return 0;
    }
  // 3. MOV PC, LR interferes
  if((instr1.mnemonic == MNEMO_MOV && instr1.dest.name == "pc") ||
     (instr2.mnemonic == MNEMO_MOV && instr2.dest.name == "pc"))
    {
      return 0;
    }
  // 1. Collect all the register this instruction used in DEST and SRC
  std::set<std::string> instr1_write = Get_Killed_vars(instr1);
  std::set<std::string> instr1_read  = Get_Referenced_vars(instr1);
  std::set<std::string> instr2_write = Get_Killed_vars(instr2);
  std::set<std::string> instr2_read  = Get_Referenced_vars(instr2);

  // 2. Check RAW/WAR/WAW Interfence

  // WAW:
  for(auto it : instr1_write)
    {
      if(instr2_write.count(it)) // has WAW interfence
	{
	  return 0;
	}
    }
  // WAR:
  for(auto it : instr1_read)
    {
      if(instr2_write.count(it)) // has WAR interfence
	{
	  return 0;
	}
    }
  // RAW:  
  for(auto it : instr1_write)
    {
      if(instr2_read.count(it)) // has RAW interfence
	{
	  switch(instr1.mnemonic)
	    {
	    case MNEMO_RAW:     	    case MNEMO_ADD:     
	    case MNEMO_SUB:     	    case MNEMO_MOV:
	    case MNEMO_MOV32:               case MNEMO_LSL:
	    case MNEMO_LSR:                 case MNEMO_ASR:
	      return 0;
	    case MNEMO_SDIV:    
	    case MNEMO_LDR:
	      return 2;
	    case MNEMO_STR:     
	      return 1;
	    case MNEMO_STM:           case MNEMO_LDM:
	      return 2;
	    case MNEMO_LDR_GLB:
	      return 0;
	    case MNEMO_MUL:
	      return 1;
	    case MNEMO_MLA:     
	    case MNEMO_MLS:     
	      return 1;
	    case MNEMO_CMP:     
	      return 0;
	    case MNEMO_B:
	      return 3;
	    case MNEMO_BL:
	      return 3;
	    case MNEMO_NOP:     
	    case MNEMO_LABEL:   
	    case MNEMO_NOTE:    
	    case MNEMO_PASSNOTE:
	    case MNEMO_BLK_SEP:
	      return 0;
	    }
	  // // Case #1.
	  // //────────────
	  // //  LDR Rx, Y
	  // //  ADD Rd, Rx, Ra
	  // //────────────
	  // //  Requires 4 Extra cycles
	  // if(instr1.mnemonic == MNEMO_LDR)
	  //   return 4;
	  // // Case #2.
	  // //────────────
	  // //  ADD Rd, Ru, Rv
	  // //  REFERENCE Ry, Rd, Rx
	  // //────────────
	  // //  Requires 1 Extra cycles
	  // myAssert(MNEMO_REGISTERS.count(instr1.mnemonic),ASSERT_EXIT_CODE_51);
	  // if(MNEMO_REGISTERS.at(instr1.mnemonic)[0] == 1)
	  //   {
	  //     myAssert(INSTR_EXECUTION_TIME.count(instr1.mnemonic),ASSERT_EXIT_CODE_52);
	  //     return INSTR_EXECUTION_TIME.at(instr1.mnemonic)-1;
	  //   }
	  // // Otherwise, no extra cycles is used
	  // return 0;
	     
	}
    }
  
  return -1;
  // Not Interfere.
    
}

int Check_Interfere(instr_t* before_instr1,
		    instr_t* instr1,
		    instr_t* before_instr2,
		    instr_t* instr2)
{
  
  int res = _check_interfere(*instr1, *instr2);
  if(res != -1)
    return res;
  // LDR Rx, memoff
  // STR Ry, memoff
  // If memoff is equal, they interferes
  if((instr1->mnemonic == MNEMO_LDR &&
      instr2->mnemonic == MNEMO_STR) ||
     (instr1->mnemonic == MNEMO_STR &&
      instr2->mnemonic == MNEMO_LDR) ||
     (instr1->mnemonic == MNEMO_STR &&
      instr2->mnemonic == MNEMO_STR)
     )
    {
      mem_offset_t off1(instr1->ops1);
      mem_offset_t off2(instr2->ops1);
      myAssert(off1.base.name != "",ASSERT_EXIT_CODE_53);
      myAssert(off2.base.name != "",ASSERT_EXIT_CODE_54);
      if((off1.base.name == "sp" && off1.offset.get_static_type() == STATIC_IMM) &&
	 (off2.base.name == "sp" && off2.offset.get_static_type() == STATIC_IMM) &&
	 (off1.offset.imm != off2.offset.imm)
	 ) // Stack load/store are Not interfere at different address
	{
	  return -1;
	}
      // GlobalVar load/store are not interfere against stack load/store
      if((off1.base.name == "sp" && off1.offset.get_static_type() == STATIC_IMM) &&
	 (off2.base.name == "sp" && off2.offset.get_static_type() == STATIC_IMM) &&
	 (off1.offset.imm != off2.offset.imm)
	 ) // Stack load/store are Not interfere at different address
	{
	  return -1;
	}
      //  Globl vs Stack
      if((before_instr1 != NULL && before_instr1->mnemonic == MNEMO_LDR_GLB && before_instr1->dest.name == off1.base.name && off1.offset.get_static_type() == STATIC_NONE) &&
	 (off2.base.name == "sp" && off2.offset.get_static_type() == STATIC_IMM)
	 ) // Stack load/store are Not interfere at different address
	{
	  return -1;
	}
      // Stack vs Globl
      if((before_instr2 != NULL && before_instr2->mnemonic == MNEMO_LDR_GLB && before_instr2->dest.name == off2.base.name && off2.offset.get_static_type() == STATIC_NONE) &&
	 (off1.base.name == "sp" && off1.offset.get_static_type() == STATIC_IMM)
	 ) // Stack load/store are Not interfere at different address
	{
	  return -1;
	}
      // Globl vs Globl
      if((before_instr1 != NULL && before_instr1->mnemonic == MNEMO_LDR_GLB && before_instr1->dest.name == off1.base.name && off1.offset.get_static_type() == STATIC_NONE) &&
	 (before_instr2 != NULL && before_instr2->mnemonic == MNEMO_LDR_GLB && before_instr2->dest.name == off2.base.name && off2.offset.get_static_type() == STATIC_NONE) &&
	 (before_instr1->ops1.name != before_instr2->ops1.name))
	{
	  return -1;
	}
      return 0;
    }
     
  return res;
}


void Eliminate_Useless_Instructions(asm_basic_block_t* asmbb)
{
  // filter-out all comments in [asmbb]
  std::list<instr_t*>& tmplist = *new std::list<instr_t*>;
  auto p = skip_useless_blanks(asmbb->begin, asmbb->end);
  while(1)
    {
      if(p == asmbb->end)
	break;
      tmplist.push_back(*p);
      p = skip_useless_blanks(std::next(p), asmbb->end);
    }
  asmbb->begin = tmplist.begin();
  asmbb->end = tmplist.end();

}
struct _instr_sched_DAG
{
  std::map<instr_t*, std::map<instr_t*, int>> Edge;
  std::map<instr_t*, int> in_degree;
  std::map<instr_t*, int> out_degree;
  void AddEdge(instr_t* u, instr_t* v, int w)
  {
    // printf("AddEdge:\n");
    // u->print(std::cerr);
    // v->print(std::cerr);
    // printf("w = %d\n\n", w);
    // u --> v, weight w
    if(!Edge.count(u))
      Edge.insert({u,{}});
    if(!Edge.count(v))
      Edge.insert({v,{}});
    if(!in_degree.count(u))
      in_degree[u] = 0;
    if(!in_degree.count(v))
      in_degree[v] = 0;
    if(!out_degree.count(u))
      out_degree[u] = 0;
    if(!out_degree.count(v))
      out_degree[v] = 0;
    if(w == -1)
      return;
    Edge[u][v] = w;
    in_degree[v]++;
    out_degree[u]++;
  }
};

std::list<instr_t*> _instr_sched(std::list<instr_t*>::iterator begin,
				 std::list<instr_t*>::iterator end)
{
  if(std::next(begin) == end)  // 1 instruction
    {
      std::list<instr_t*> result(begin, end);
      return result;
    }

  
  // for(auto p = begin;
  //     p != end;
  //     ++p)
  //   {
  //     (*p)->print(std::cerr);
  //   }
  /*
    Ref:
    Palem, Krishna V.; Simons, Barbara B.  (1993). 
    Scheduling time-critical instructions on RISC machines. 
    ACM Transactions on Programming Languages and Systems, 15(4), 632–658.
    doi:10.1145/155183.155190
  */
  // No Deadline.
  _instr_sched_DAG graph, reversed_graph;
  int instruction_count = 0;
  // Pass #1, Construct schedule DAG
  for(auto q = begin; q != end; q++)
    {
      instruction_count++;
      for(auto p = begin; p != q; p++)
	{
	  instr_t* bp = (p == begin?NULL:*std::prev(p));
	  instr_t* bq = (q == begin?NULL:*std::prev(q));
	  int interfere = Check_Interfere(bp, *p, bq, *q);
	  // std::cerr<<("Checking interference for:\n");
	  // (*p)->print(std::cerr);
	  // (*q)->print(std::cerr);
	  // std::cerr<<"Returns:"<<interfere<<"\n\n\n";
	  graph.AddEdge(*p, *q, interfere);
	  reversed_graph.AddEdge(*q, *p, interfere);
	}
    }

  // Pass #2, Computing the longest path from an instruction to the sink
  std::map<instr_t*, int> longest_path;
  for(auto it : graph.Edge)
    {
      longest_path[it.first] = -1;
    }
  for(auto it : graph.out_degree)
    {
      if(it.second == 0)
	{
	  myAssert(longest_path.count(it.first),ASSERT_EXIT_CODE_55);
	  longest_path[it.first] = 0;
	}
    }
  
  // Pass #3, Find the topsort order
  _instr_sched_DAG clone_graph = graph;
  std::list<instr_t*> TopOrder;
  std::set<instr_t*> Zero_Inds;
  for(auto p = clone_graph.in_degree.begin();
      p != clone_graph.in_degree.end();
      ++p)
    {
      if(p->second == 0)
	Zero_Inds.insert(p->first);
    }
  while(!Zero_Inds.empty())
    {
      auto p = Zero_Inds.begin();
      auto pr = *p;
      Zero_Inds.erase(p);
      myAssert(clone_graph.in_degree.count(pr),ASSERT_EXIT_CODE_56);
      myAssert(clone_graph.in_degree[pr] == 0,ASSERT_EXIT_CODE_57);
      TopOrder.push_back(pr);
      myAssert(clone_graph.Edge.count(pr),ASSERT_EXIT_CODE_58);
      for(auto it : clone_graph.Edge[pr])
	{
	  myAssert(pr != it.first,ASSERT_EXIT_CODE_59); // No reflexive edge
	  myAssert(clone_graph.in_degree.count(it.first),ASSERT_EXIT_CODE_60);
	  myAssert(!Zero_Inds.count(it.first),ASSERT_EXIT_CODE_61);
	  myAssert(clone_graph.in_degree[it.first] > 0,ASSERT_EXIT_CODE_62);
	  clone_graph.in_degree[it.first]--;
	  if(clone_graph.in_degree[it.first] == 0)
	    Zero_Inds.insert(it.first);
	}
    }

  // Pass #4, Calculate the longest distance between point to sink
  for(auto p = TopOrder.rbegin();
      p != TopOrder.rend();
      ++p)
    {
      myAssert(reversed_graph.Edge.count(*p),ASSERT_EXIT_CODE_63);
      for(auto it : reversed_graph.Edge[*p])
	{
	  myAssert(longest_path.count(it.first),ASSERT_EXIT_CODE_64);
	  longest_path[it.first] = std::max(longest_path[it.first],
					    it.second + longest_path[*p]);
	}
    }

  // Pass #5, Validate
  // for(auto it : graph.Edge)
  //   {
  //     printf("longest_path %d for ",longest_path[it.first]);
  //     it.first->print(std::cerr);
  //   }
  
  for(auto it : graph.Edge)
    {
      myAssert(longest_path.count(it.first),ASSERT_EXIT_CODE_65);
      // 66 -> 67
      myAssert(longest_path[it.first] != -1,ASSERT_EXIT_CODE_67);
    }

  // Pass #6, Topsort and schedule
  std::list<instr_t*> result;
  struct _pq_cmp
  {
    bool operator()(const std::pair<int, instr_t*>& a, const std::pair<int, instr_t*>& b)
	      {
		return a.first < b.first;
	      };

  };
  std::priority_queue<std::pair<int, instr_t*>,
		      std::vector<std::pair<int, instr_t*> >,
		      _pq_cmp> ready;
  for(auto it : graph.in_degree)
    {
      if(it.second == 0)
	{
	  myAssert(longest_path.count(it.first),ASSERT_EXIT_CODE_67);
	  ready.push({longest_path[it.first], it.first});
	}
    }
  while(!ready.empty())
    {
      auto pr = ready.top();
      ready.pop();
      result.push_back(pr.second);
      myAssert(graph.Edge.count(pr.second),ASSERT_EXIT_CODE_68);
      for(auto it : graph.Edge[pr.second])
	{
	  myAssert(graph.in_degree.count(it.first),ASSERT_EXIT_CODE_69);
	  myAssert(graph.in_degree[it.first] > 0,ASSERT_EXIT_CODE_70);
	  graph.in_degree[it.first]--;
	  if(graph.in_degree[it.first] == 0)
	    {
	      myAssert(longest_path.count(it.first),ASSERT_EXIT_CODE_71);
	      ready.push({longest_path[it.first], it.first});
	    }
	  
	}
    }
  return result;
}


void Instruction_Scheduling(asm_basic_block_t* asmbb)
{
  if(asmbb->end == asmbb->begin)
    return;
  
  int instruction_count = 0;
  std::list<instr_t*>& result = *new std::list<instr_t*>;
  auto prev = asmbb->begin;
  for(auto p = asmbb->begin; p != asmbb->end; ++p)
    {
      if(instruction_count % 100 == 0 && instruction_count != 0)
	{
	  auto li = _instr_sched(prev, p);
	  result.insert(result.end(), li.begin(), li.end());
	  prev = p;
	}
      instruction_count++;
    }
  if(prev != asmbb->end)
    {
      auto li = _instr_sched(prev, asmbb->end);
      result.insert(result.end(), li.begin(), li.end());
    }
  asmbb->begin = result.begin();
  asmbb->end = result.end();
  
}

/////////////////////////////////////////////////////
///////// P E E P H O L E ///////////////////////////
/////////////// O P T I M I Z E /////////////////////
/////////////////////////////////////////////////////


bool peephole_opt(instr_t& instr1,
		  instr_t& instr2)
{
  // printf("Peephole_Optimizing:\n");
  // instr1.print(std::cerr);
  // instr2.print(std::cerr);
  // Optimize Redundant LOAD & STORE in two instructions
  // Case #1
  //────────────
  //   LDR X, Y
  //   STR X, Y
  //   ─────>
  //   LDR X, Y
  //────────────
  if(instr1.mnemonic == MNEMO_LDR &&
     instr2.mnemonic == MNEMO_STR)
    {
      myAssert(is_register(instr1.dest.name),ASSERT_EXIT_CODE_72);
      myAssert(is_register(instr2.dest.name),ASSERT_EXIT_CODE_73);
      if(instr1.dest == instr2.dest &&
  	 instr1.ops1 == instr2.ops1)
  	{
  	  instr2.mnemonic = MNEMO_NOP;
  	  return true;
  	}
    }

  // Case #2
  //────────────
  //   STR X, Y
  //   LDR X, Y
  //   ─────>
  //   STR X, Y
  //   NOP
  //────────────
  if(instr1.mnemonic == MNEMO_STR &&
     instr2.mnemonic == MNEMO_LDR)
    {
      if(instr1.dest == instr2.dest &&
	 instr1.ops1 == instr2.ops1)
  	{
	  instr2 = instr_t(MNEMO_NOP);
  	  return true;
  	}
    }

  // // Case #2+
  // //────────────
  // //   STR X, Y
  // //   LDR Z, Y
  // //   ─────>
  // //   STR X, Y
  // //   MOV Z, X
  // //────────────
  if(instr1.mnemonic == MNEMO_STR &&
     instr2.mnemonic == MNEMO_LDR)
    {
      if(instr1.ops1 == instr2.ops1)
  	{
  	  instr2 = instr_t(MNEMO_MOV, instr2.dest, instr1.dest);
  	  return true;
  	}
    }
  // // Case #2++
  // //────────────
  // //   LDR X, Y
  // //   LDR Z, Y
  // //   ─────>
  // //   LDR X, Y
  // //   MOV Z, X
  // //────────────
  if(instr1.mnemonic == MNEMO_LDR &&
     instr2.mnemonic == MNEMO_LDR)
    {
      if(instr1.ops1 == instr2.ops1)
  	{
  	  instr2 = instr_t(MNEMO_MOV, instr2.dest, instr1.dest);
  	  return true;
  	}
    }

  // Case #3, [MLA Rd, R1, R2, R3] <=> [Rd = (R1*R2 + R3)]
  //────────────
  //   MUL Ru, Rv, Rw
  //   ADD Ry, Ru, Rx
  //   ─────>
  //   MLA Ry, Rv, Rw, Rx
  //────────────  
  //   MUL Ru, Rv, Rw
  //   ADD Ry, Rx, Ru
  //   ─────>
  //   MLA Ry, Rv, Rw, Rx
  //────────────  
  if(instr1.mnemonic == MNEMO_MUL &&
     instr2.mnemonic == MNEMO_ADD &&
     instr2.ops2.get_static_type() != STATIC_IMM &&
     instr1.ref_count == 1)
    {
      myAssert(is_register(instr1.dest.name),ASSERT_EXIT_CODE_74);
      myAssert(is_register(instr1.ops1.name),ASSERT_EXIT_CODE_75);
      myAssert(is_register(instr1.ops2.name),ASSERT_EXIT_CODE_76);
      myAssert(is_register(instr2.dest.name),ASSERT_EXIT_CODE_77);
      myAssert(is_register(instr2.ops1.name),ASSERT_EXIT_CODE_78);
      myAssert(is_register(instr2.ops2.name),ASSERT_EXIT_CODE_79);
      if(instr1.dest == instr2.ops1)
	{
	  instr_t tmpinstr(MNEMO_MLA, instr2.dest, instr1.ops1, instr1.ops2, instr2.ops2);
	  instr1 = tmpinstr;
	  instr2 = instr_t(MNEMO_NOP);
	  return true;
	}
      if(instr1.dest == instr2.ops2)
	{
	  instr_t tmpinstr(MNEMO_MLA, instr2.dest, instr1.ops1, instr1.ops2, instr2.ops1);
	  instr1 = tmpinstr;
	  instr2 = instr_t(MNEMO_NOP);
	  return true;
	}
    }
     
  
  // Case #4, [MLS Rd, R1, R2, R3] <=> [Rd = (R3 - R1*R2)]
  //────────────
  //   MUL Ru, Rv, Rw
  //   SUB Ry, Rx, Ru
  //   ─────>
  //   MLS Ry, Rv, Rw, Rx
  //────────────  
 if(instr1.mnemonic == MNEMO_MUL &&
    instr2.mnemonic == MNEMO_SUB &&
    instr2.ops2.get_static_type() != STATIC_IMM &&
    instr1.ref_count == 1)
    {
      myAssert(is_register(instr1.dest.name),ASSERT_EXIT_CODE_80);
      myAssert(is_register(instr1.ops1.name),ASSERT_EXIT_CODE_81);
      myAssert(is_register(instr1.ops2.name),ASSERT_EXIT_CODE_82);
      myAssert(is_register(instr2.dest.name),ASSERT_EXIT_CODE_83);
      myAssert(is_register(instr2.ops1.name),ASSERT_EXIT_CODE_84);
      myAssert(is_register(instr2.ops2.name),ASSERT_EXIT_CODE_85);
      if(instr1.dest == instr2.ops2)
	{
	  instr_t tmpinstr(MNEMO_MLS, instr2.dest, instr1.ops1, instr1.ops2, instr2.ops1);
	  instr1 = tmpinstr;
	  instr2 = instr_t(MNEMO_NOP);
	  return true;
	}
    }

 // Case #5. Weaken strength for Ariths
 //────────────
 //   ADD Ry, Rs, #0
 //   SUB Ry, Rs, #0
 //   LSL Ry, Rs, #0
 //   LSR Ry, Rs, #0
 //   ASR Ry, Rs, #0
 //   ─────>
 //   MOV Ry, Rs
 //────────────  
 if((instr1.mnemonic == MNEMO_ADD ||
     instr1.mnemonic == MNEMO_SUB ||
     instr1.mnemonic == MNEMO_LSL ||
     instr1.mnemonic == MNEMO_LSR ||
     instr1.mnemonic == MNEMO_ASR)
    && instr1.ops2.get_static_type() == STATIC_IMM
    && instr1.ops2.imm == 0)
    {
      myAssert(is_register(instr1.dest.name),ASSERT_EXIT_CODE_86);
      myAssert(is_register(instr1.ops1.name),ASSERT_EXIT_CODE_87);
      instr1 = instr_t(MNEMO_MOV, instr1.dest.name, instr1.ops1.name);
      return true;
    }

 // Case #6. LSL/LSR/ASR Suffix for Ariths
 //────────────
 //   LSL Ry, Rs, #k
 //   [ADD/SUB/SDIV/MUL] Rz, Rt, Ry
 //   ─────>
 //   LSL Ry, Rs, #k
 //   [...] Rz, [Rb, Rs, LSL#k]
 //────────────
 if((instr1.mnemonic == MNEMO_LSL ||
     instr1.mnemonic == MNEMO_LSR ||
     instr1.mnemonic == MNEMO_ASR) &&
    (instr2.mnemonic == MNEMO_ADD ||
     instr2.mnemonic == MNEMO_SUB ||
     instr2.mnemonic == MNEMO_SDIV ||
     instr2.mnemonic == MNEMO_MUL) &&
    instr1.dest != instr1.ops1 &&
    instr1.ops2.get_static_type() == STATIC_IMM &&
    instr2.ops2.name == instr1.dest.name)
   {
     // printf("Hit at:\n");
     // instr1.print(std::cerr);
     // instr2.print(std::cerr);
     std::map<MNEMONIC_TYPE, std::string> mp =
       {
	{MNEMO_LSL, "LSL"},
	{MNEMO_LSR, "LSR"},
	{MNEMO_ASR, "ASR"},	
       };
     myAssert(mp.count(instr1.mnemonic),ASSERT_EXIT_CODE_88);
     instr2.ops2.name = instr1.ops1.name + ", " + mp[instr1.mnemonic] + " #"+std::to_string(instr1.ops2.imm);
     return true;
   }
 //────────────
 //   LSL Ry, Rs, #k
 //   [LDR/STR] Rz, [Rx, Ry]
 //   ─────>
 //   LSL Ry, Rs, #k
 //   [LDR/STR] Rz, [Rx, Rs, LSL #k]
 //────────────
 if((instr1.mnemonic == MNEMO_LSL ||
     instr1.mnemonic == MNEMO_LSR ||
     instr1.mnemonic == MNEMO_ASR) &&
    (instr2.mnemonic == MNEMO_LDR ||
     instr2.mnemonic == MNEMO_STR) &&
    instr1.dest != instr1.ops1 &&
    instr1.ops2.get_static_type() == STATIC_IMM)
   {
     myAssert(is_memory_access(instr2.ops1),ASSERT_EXIT_CODE_89);
     mem_offset_t offs(instr2.ops1);
     if(offs.offset.name == instr1.dest.name)
       {       
	 // printf("Hit at:\n");
	 // instr1.print(std::cerr);
	 // instr2.print(std::cerr);
	 std::map<MNEMONIC_TYPE, std::string> mp =
	   {
	    {MNEMO_LSL, "LSL"},
	    {MNEMO_LSR, "LSR"},
	    {MNEMO_ASR, "ASR"},	
	   };
	 myAssert(mp.count(instr1.mnemonic),ASSERT_EXIT_CODE_90);
	 offs.offset.name = instr1.ops1.name + ", " + mp[instr1.mnemonic] + " #"+std::to_string(instr1.ops2.imm);
	 instr2.ops1 = offs.get_literal();
	 return true;
       }
   }

 //────────────
 //   LSL Ry, Rs, #k
 //   [LDR/STR] Rz, [Ry, Rx]
 //   ─────>
 //   LSL Ry, Rs, #k
 //   [LDR/STR] Rz, [Rx, Rs, LSL #k]
 //────────────

  if((instr1.mnemonic == MNEMO_LSL ||
     instr1.mnemonic == MNEMO_LSR ||
     instr1.mnemonic == MNEMO_ASR) &&
    (instr2.mnemonic == MNEMO_LDR ||
     instr2.mnemonic == MNEMO_STR) &&
    instr1.dest != instr1.ops1 &&
    instr1.ops2.get_static_type() == STATIC_IMM)
   {
     myAssert(is_memory_access(instr2.ops1),ASSERT_EXIT_CODE_91);
     mem_offset_t offs(instr2.ops1);
     if(is_register(offs.base.name) &&
	is_register(offs.offset.name) &&
	offs.base.name == instr1.dest.name)
       {       
	 // printf("Hit at:\n");
	 // instr1.print(std::cerr);
	 // instr2.print(std::cerr);

	 std::map<MNEMONIC_TYPE, std::string> mp =
	   {
	    {MNEMO_LSL, "LSL"},
	    {MNEMO_LSR, "LSR"},
	    {MNEMO_ASR, "ASR"},	
	   };
	 myAssert(mp.count(instr1.mnemonic),ASSERT_EXIT_CODE_92);
	 offs.base.name = instr1.ops1.name + ", " + mp[instr1.mnemonic] + " #"+std::to_string(instr1.ops2.imm);
	 std::swap(offs.base.name, offs.offset.name);
	 instr2.ops1 = offs.get_literal();
	 return true;
       }
   }
 //────────────
 //   LSL Ry, Rs, #k1
 //   LSL Rz, Ry, #k2
 //   ─────>
 //   LSL Ry, Rs, #k
 //   LSL Rz, Rs, #k1+k2
 //────────────
  // if(instr1.mnemonic == MNEMO_LSL &&
  //    instr2.mnemonic == MNEMO_LSL &&
  //    instr1.ops2.get_static_type() == STATIC_IMM &&
  //    instr2.ops2.get_static_type() == STATIC_IMM &&
  //    instr1.dest != instr1.ops1 &&
  //    instr2.ops1 == instr1.dest
  //    )
  //  {
  //    instr2.ops1 = instr1.ops1;
  //    instr2.ops2.imm = instr1.ops2.imm + instr2.ops2.imm;
  //    return true;
  //  }

 // Try to swap [ldr/str]'s arg order
 // if(instr1.mnemonic == MNEMO_LDR ||
 //    instr1.mnemonic == MNEMO_STR)
 //   {
 //     myAssert(is_memory_access(instr1.ops1),ASSERT_EXIT_CODE_93);
 //     mem_offset_t offs(instr1.ops1);
 //     if(is_register(offs.base.name) &&
 // 	is_register(offs.offset.name))
 //       {
 // 	 std::swap(offs.base.name,
 // 		   offs.offset.name);
 // 	 instr1.ops1.name = offs.get_literal();
 //       }
 //   }
 return false;
}

//////////////////////////////////////////////
//// Duplicate Code Elimination //////////////
//////////////////////////////////////////////
bool Reducable(std::list<instr_t*>::iterator p,
	       std::list<instr_t*>::iterator q)
{
  if((*p)->mnemonic != (*q)->mnemonic ||
     (*p)->dest     != (*q)->dest     ||
     (*p)->ops1     != (*q)->ops1     ||
     (*p)->ops2     != (*q)->ops2     ||
     (*p)->ops3     != (*q)->ops3     ||
     (*p)->dest     == (*p)->ops1     ||
     (*p)->dest     == (*p)->ops2     ||
     (*p)->dest     == (*p)->ops3     ||
     (*p)->mnemonic == MNEMO_STR      ||
     (*p)->condition_string != (*q)->condition_string)
    return false;
  for(auto pp = p; pp != q; pp++)
    {
      switch((*pp)->mnemonic)
	{
	case MNEMO_ADD:   case MNEMO_SUB:
	case MNEMO_SDIV:  case MNEMO_MOV:
	case MNEMO_MOV32: case MNEMO_MUL:
	case MNEMO_LSL:   case MNEMO_LSR:
	case MNEMO_ASR:   case MNEMO_MLA:
	case MNEMO_MLS:   case MNEMO_NOP:
	case MNEMO_NOTE:  case MNEMO_PASSNOTE:
	case MNEMO_STR:
	  {
	    break;
	  }
	default:
	  {
	    return false;
	  }
	}
    }
  // Check if there are any modification for (*p)->dest
  for(auto pp = std::next(p); pp != q; pp++)
    {
      if(MNEMO_REGISTERS.at((*pp)->mnemonic)[0] == 1 &&
	 (*pp)->dest == (*p)->dest)
	{
	  return false;
	}
      if(MNEMO_REGISTERS.at((*pp)->mnemonic)[0] == 1 &&
	 (*pp)->dest == (*p)->ops1)
	{
	  return false;
	}
      if(MNEMO_REGISTERS.at((*pp)->mnemonic)[0] == 1 &&
	 (*pp)->dest == (*p)->ops2)
	{
	  return false;
	}
      if(MNEMO_REGISTERS.at((*pp)->mnemonic)[0] == 1 &&
	 (*pp)->dest == (*p)->ops3)
	{
	  return false;
	}
    }
  return true;
}


void ASM_Optimize(std::list<instr_t*>::iterator& begin,
		  std::list<instr_t*>::iterator end,
		  std::list<instr_t*>& InstrList,
		  bool is_fake)
{
  // printf("Optimizing: ");
  // for(auto p = begin;
  //     p != end;
  //     ++p)
  //   {
  //     (*p)->print(std::cerr);
  //   }

  // Pass 1. Peephole optimize
  {
    // peephole-2
    auto p = begin;
    while(p != end)
      {
  	while(p != end &&
  	      (
  	       (*p)->mnemonic == MNEMO_RAW ||
  	       (*p)->mnemonic == MNEMO_LABEL ||
  	       (*p)->mnemonic == MNEMO_NOP ||
  	       (*p)->mnemonic == MNEMO_NOTE ||
  	       (*p)->mnemonic == MNEMO_PASSNOTE
  	       ))
  	  p++;
  	if(p == end)
  	  break;
  	auto q = std::next(p);
  	while(q != end &&
  	      (
  	       (*q)->mnemonic == MNEMO_RAW ||
  	       (*q)->mnemonic == MNEMO_NOP ||
  	       (*q)->mnemonic == MNEMO_NOTE ||
  	       (*q)->mnemonic == MNEMO_PASSNOTE
  	       ))
  	  q++;
  	if(q == end)
  	  {
  	    p++;
  	    continue;
  	  }
  	if(peephole_opt(**p, **q))
  	  {}
  	else
  	  {p++;}
      }
    
  }
  { // peephole-n
    auto p = begin;
    while(p != end)
      {
  	p = skip_blanks(p, end);
  	if(p == end)
  	  break;
  	auto q = skip_blanks(std::next(p), end);
  	if(q == end)
  	  break;
  	int cnt = 0;
	instr_t* bp = (p == begin?NULL:*std::prev(p));
	instr_t* bq = (q == begin?NULL:*std::prev(q));
  	while(Check_Interfere(bp, *p,bq, *q) == -1)
  	  {
	    // printf("TOTAL RANGE:\n");
	    // (*p)->print(std::cerr);
	    // (*q)->print(std::cerr);
  	    for(auto r = skip_blanks(std::next(p), end);
  		r != q; r = skip_blanks(std::next(r), end))
  	      {
		// printf("CHECKING SUBRANGE:\n");
		// (*r)->print(std::cerr);
		// (*q)->print(std::cerr);
		// printf("UNDER:\n");
		// (*p)->print(std::cerr);
		// (*q)->print(std::cerr);
		instr_t* br = (r == begin?NULL:*std::prev(r));
		instr_t* bq = (q == begin?NULL:*std::prev(q));
  		if(Check_Interfere(br,*r,bq,*q) != -1)
		  {
		    goto _cont_opt;
		  }
  	      }
  	    peephole_opt(**p, **q);
  	    q = skip_blanks(std::next(q), end);
  	    if(q == end)
  	      break;
  	    cnt++;
  	    if(cnt >= 30)
  	      break;
  	  }
  	if(q != end)
  	  {
  	    for(auto r = skip_blanks(std::next(p), end);
  		r != q; r = skip_blanks(std::next(r), end))
  	      {
		// printf("CHECKING SUBRANGE:\n");
		// (*r)->print(std::cerr);
		// (*q)->print(std::cerr);
		// printf("UNDER:\n");
		// (*p)->print(std::cerr);
		// (*q)->print(std::cerr);
		instr_t* br = (r == begin?NULL:*std::prev(r));
		instr_t* bq = (q == begin?NULL:*std::prev(q));
  		if(Check_Interfere(br,*r,bq,*q) != -1)
		  {
		    goto _cont_opt;
		  }
  	      }
	    
  	    peephole_opt(**p, **q);
  	  }
      _cont_opt:
  	p = std::next(p);
      }
  }

  // Pass 2. Reduce duplicate instructions
  {
    auto p = begin;
    while(p != end)
      {
  	auto q = std::next(p);
  	for(int cnt = 0; cnt < 30; cnt++)
  	  {
  	    if(q == end) break;
  	    if(Reducable(p, q))
  	      {
  		*(*q) = instr_t(MNEMO_NOP);
  	      }
  	    q++;
  	  }
  	p++;
      }
  }

  // Pass 3. Eliminate duplicate jumps
  {
    auto p = begin;
    while(p != end)
      {
  	if((*p)->mnemonic == MNEMO_B &&
  	   (*p)->condition_string == "")
  	  {
  	    for(auto q = skip_blanks(std::next(p), end);
  		q != end && (*q)->mnemonic == MNEMO_LABEL;
  		q = skip_blanks(std::next(q), end))
  	      {
  		if((*p)->dest == (*q)->dest)
  		  {
  		    *(*p) = instr_t(MNEMO_NOP);
  		    break;
  		  }
  	      }
  	    p++;
  	  }
  	else
  	  {
  	    p++;
  	  }
      }
  }

  // Pass 4. Release double jump
  {
    std::map<std::string, std::list<instr_t*>::iterator> labels;
    // Pass 4.1, collect all labels
    for(auto p = begin;
  	p != end;
  	++p)
      {
  	if((*p)->mnemonic == MNEMO_LABEL)
  	  {
  	    myAssert((*p)->dest.name != "",ASSERT_EXIT_CODE_94);
  	    myAssert(!labels.count((*p)->dest.name),ASSERT_EXIT_CODE_95);
  	    labels.insert({(*p)->dest.name, p});
  	  }
      }
    // Pass 4.2, scan all jumps, check if can be rewrited
    for(auto p = begin;
  	p != end;
  	++p)
      {
  	if((*p)->mnemonic == MNEMO_B)
  	  {
  	    myAssert((*p)->dest.name != "",ASSERT_EXIT_CODE_96);
  	    myAssert(labels.count((*p)->dest.name),ASSERT_EXIT_CODE_97);
  	    auto q = labels.at((*p)->dest.name);
  	    myAssert((*q)->mnemonic == MNEMO_LABEL,ASSERT_EXIT_CODE_98);
  	    while((*q)->mnemonic == MNEMO_LABEL && q != end)
  	      q++;
  	    if(q == end)
  	      continue;
  	    if((*q)->mnemonic == MNEMO_B &&
  	       (*q)->condition_string == "")
  	      {
  		(*p)->dest.name = (*q)->dest.name;
  	      }
  	  }
      }
    
  }
  std::list<asm_basic_block_t*> blks;
  // Pass 4. Block Seperation
  {
    // Collect all labels as jump targets
    std::set<std::string> target_labels;

    // Pass 4.1, Scan all [B/MOV pc, lr] to find separate points
    for(auto p = begin;
	p != end;
	++p)
      {
	instr_t& asmop = **p;
	if(asmop.mnemonic == MNEMO_B ||
	   (asmop.mnemonic == MNEMO_MOV &&
	    asmop.dest.name == "pc" &&
	    asmop.ops1.name == "lr"))
	  // Is a Branch Operation
	  {
	    if(asmop.mnemonic == MNEMO_B)
	      { // Jump with a label
		myAssert(asmop.dest.name != "",ASSERT_EXIT_CODE_99);
		target_labels.insert(asmop.dest.name);
	      }
	    // Collect target location
	    ++p;
	    p = InstrList.insert(p, new instr_t(MNEMO_BLK_SEP));
	  }
      }

    // Pass 4.2, Scan all [Labels] as jump target
    for(auto p = begin;
	p != end;
	++p)
      {
	instr_t& asmop = **p;
	if(asmop.mnemonic == MNEMO_LABEL)
	  {
	    myAssert(asmop.dest.name != "",ASSERT_EXIT_CODE_100);
	    if(target_labels.count(asmop.dest.name))
	      {
		p = InstrList.insert(p, new instr_t(MNEMO_BLK_SEP));
		p++; // Skip this [LABEL]
	      }
	  }
      }

    // Pass 4.3, Collect all blocks
    auto block_begin = begin;
    for(auto p = begin;
	p != end;
	++p)
      {
	if((*p)->mnemonic == MNEMO_BLK_SEP)
	  {
	    auto block_end = p;
	    asm_basic_block_t* bp = new asm_basic_block_t(block_begin, block_end);
	    blks.push_back(bp);
	    block_begin = p;
	    std::advance(block_begin,1);
	  }
      }

    // Pass 4.4, Eliminate empty blocks
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
    // Pass 4.5, Add blocks' JUMP constraints
    for(auto p = blks.begin();
    	p != blks.end();
    	++p)
      {
    	auto end_asmopp = std::prev((*p)->end);
    	auto& end_asmop = **end_asmopp;

	if(end_asmop.mnemonic == MNEMO_B && end_asmop.condition_string != "")
	  // Conditional Branches
    	  {
    	    myAssert(std::next(p) != blks.end(), ASSERT_EXIT_CODE_73);
    	    asm_basic_block_t::AddEdge(*p, *(std::next(p)));
    	    auto jmp_target = end_asmop.dest.name;
	    myAssert(jmp_target != "",ASSERT_EXIT_CODE_102);
	    asm_basic_block_t* target = NULL;
#define FIND_TARGET_LABEL(jmp_target, target)			\
	    {							\
	      for(auto q = blks.begin();			\
		  q != blks.end();				\
		  ++q)						\
		{						\
		  auto qasmopp = *(*q)->begin;			\
		  auto& qasmop = *qasmopp;			\
		  if(qasmop.mnemonic == MNEMO_LABEL &&		\
		     qasmop.dest.name == jmp_target)		\
		    {						\
		      target = *q;				\
		    }						\
		}						\
	    }
	    
	    FIND_TARGET_LABEL(jmp_target, target);
	    myAssert(target != NULL, ASSERT_EXIT_CODE_75);
	    asm_basic_block_t::AddEdge(*p, target);
	  }
	else if((end_asmop.mnemonic == MNEMO_B && end_asmop.condition_string == "") ||
	   (end_asmop.mnemonic == MNEMO_MOV &&
	    end_asmop.dest.name == "pc" &&
	    end_asmop.ops1.name == "lr"))
	  {
	    if(end_asmop.mnemonic == MNEMO_B)
	      {
		auto jmp_target = end_asmop.dest.name;
		asm_basic_block_t* target = NULL;
		FIND_TARGET_LABEL(jmp_target, target);
		myAssert(target != NULL, ASSERT_EXIT_CODE_76);
		asm_basic_block_t::AddEdge(*p, target);		
	      }
	    else
	      {
		// For RETURNs, do nothing
	      }
	  }
	else // Ordinary Instructions
	  {
	    myAssert(std::next(p) != blks.end(), ASSERT_EXIT_CODE_77);
    	    asm_basic_block_t::AddEdge(*p, *(std::next(p)));	    
	  }
      }

    // Pass 4.6, Eliminate blocks that is not referred (not include first block)

    // 4.6.1 Remove from [blks]
    // Warning: The first block won't be un-referred
    std::set<asm_basic_block_t*> erased;
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
    // 4.6.2 Remove from each basicblock's edge list
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
      }
    
  }

  // Pass 5. Live Variable Analyze
  {
    // Pass 5.1 Find [UEVar] and [VarKill] for each block
    for(auto p = blks.begin();
	p != blks.end();
	++p)
      {
	 (*p)->get_register_dependency();
	 (*p)->liveOut.clear();
      }
    
    // Pass 5.2 Find [LiveOut] for each block
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


  if(print_ASMFlow && !is_fake)
    {
      for(auto p = blks.begin();
	  p != blks.end();
	  ++p)
	{
	  (*p)->print(std::cerr);
	  std::cerr<<std::endl<<std::endl;
	}
    }

  // Pass 5. Optimize BB by DataFlow Analysis
  {
    // Pass 5.1, detach each block against [irs], it's no needed anymore
    for(auto it : blks)
      {
	std::list<instr_t*>* tmpList = new std::list<instr_t*>(it->begin, it->end);
	tmpList->push_back((instr_t*)new instr_t(MNEMO_BLK_SEP));
	it->begin = tmpList->begin();
	it->end = tmpList->end();
      }

    // Pass 5.2, Allocate a start block at the beginning of all blocks,
    // and a end block at the end of all blocks
    std::list<instr_t*>* tmpList = new std::list<instr_t*>();
    asm_basic_block_t* blk_entry = new asm_basic_block_t(tmpList->begin(), tmpList->begin());
    asm_basic_block_t* blk_exit = new asm_basic_block_t(tmpList->begin(), tmpList->begin());
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
    // Pass 5.3, Optimize
    ASM_Flow_Optimize(blks);
  }
  if(!is_fake)
    {
      // Pass 7, Instruction Scheduling
      // Pass 7.1, Eliminate useless instructions
      for(auto it : blks)
	{
	  Eliminate_Useless_Instructions(it);
	}
    
      // Pass 7.2, Numbering each instruction
      int linum = 1;
      for(auto it : blks)
	{
	  for(auto q = it->begin;
	      q != it->end;
	      q++)
	    {
	      (*q)->line_number = linum++;
	    }
	}


      // Pass 7.3, Instruction Scheduling
      for(auto it : blks)
	{
	  // std::cerr<<("Before Schedule:\n");
	  // it->print(std::cerr);
	  Instruction_Scheduling(it);
	  // std::cerr<<("After Schedule:\n");
	  // it->print(std::cerr);
	}

    }
  
  // Pass 6. Rewrite ASM Code
  {
    std::list<instr_t*> tmpList;

      for(auto p = blks.begin();
    	  p != blks.end();
    	  ++p)
    	{
    	  for(auto q = (*p)->begin;
    	      q != (*p)->end;
    	      ++q)
    	    {
    	      tmpList.push_back(*q);
    	    }
    	}

      InstrList.erase(std::next(begin), end);
      InstrList.insert(InstrList.end(),
		       tmpList.begin(),
		       tmpList.end());
      begin = InstrList.erase(begin);
  }
}
