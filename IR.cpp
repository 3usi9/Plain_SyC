#include "IR.h"
#include "AST.h" // __print_Indentation
#include "common.h"
#include<assert.h>
#include<cstring>
#include<sstream>
#include<iomanip>


void IRList_t::push_back(IR_t* ir)
{
  this->body.push_back(ir);
  
}

//////////////////////////////////////////
/////////I R   D A T A////////////////////
//////////////////////////////////////////

_irdata_global_area::_irdata_global_area(int cnt)
{
  this->data.push_back({OPER_SPACE, cnt});
}

int& _irdata_global_area::operator[](int idx)
{
  int sum = 0;
  for(auto it = data.begin();
      it != data.end();
      ++it)
    {
      if(it->first == OPER_WORD)
	{
	  sum++;
	  continue;
	}
      else if(sum + it->second > idx)
	{
	  myAssert(it->first == OPER_SPACE, ASSERT_EXIT_CODE_1);
	  myAssert(idx >= sum, ASSERT_EXIT_CODE_2);
	  int left_count = idx - sum;
	  if(left_count == 0)
	    {
	      it->second--;
	      it = data.insert(it, {OPER_WORD,-1});
	      return it->second;
	    }
	  else
	    {
	      it->second -= 1+left_count;
	      it = data.insert(it, { {OPER_SPACE, left_count}, {OPER_WORD, -1} } );
	      return (++it)->second;
	    }
	}
      else
	{
	  sum += it->second;
	}
    }
  myAssert(0, ASSERT_EXIT_CODE_3);
}

IRData_t::IRData_t(int length,
		   const _irdata_global_area& data) : data(data)
{
  myAssert(length % 4 == 0, ASSERT_EXIT_CODE_4);
  this->length = length;
}


IRData_t::IRData_t(int length) : data(length/4)
{
  myAssert(length % 4 == 0, ASSERT_EXIT_CODE_5);
  this->length = length;
}

//////////////////////////////////////////
/////////I R   L A B E L//////////////////
//////////////////////////////////////////

IRLabel_t::IRLabel_t(LABEL_TYPE type) :
  type(type), label_name("")
{
}

IRLabel_t::IRLabel_t(LABEL_TYPE type,
		     std::string label_name) :
  type(type), label_name(label_name)
{
}
//////////////////////////////////////////
/////////I R   O P E R ///////////////////
//////////////////////////////////////////
operand_t::operand_t() :
  type(OPERAND_NOT_DEFINED), name(""), imm(0), rewrited_name("")
{
}

operand_t::operand_t(const std::string& name) :
  type(OPERAND_VAR), name(name), imm(0), rewrited_name("")
{
}

operand_t::operand_t(int imm) :
  type(OPERAND_IMM), name(""), imm(imm), rewrited_name("")
{
}

bool operand_t::operator==(const operand_t& y) const
{
  return ((!(*this<y)) && !(y<*this));
}

bool operand_t::operator!=(const operand_t &y) const
{
  return !((*this) == y);
}

bool operand_t::operator<(const operand_t& y) const
{
  if(this->type != y.type)
    {
      return this->type < y.type;
    }
  // type is equal
  myAssert(this->type == y.type,ASSERT_EXIT_CODE_115);
  if(this->type == OPERAND_NOT_DEFINED)
    {
      return false;
    }
  if(this->type == OPERAND_IMM)
    {
      return this->imm < y.imm;
    }
  if(this->type == OPERAND_VAR)
    {
      return this->name < y.name;
    }
  myAssert(0,ASSERT_EXIT_CODE_116);
}

// TypeRefs:
// GloblVar
// GloblArr
// LocalVar
// LocalArr
// ArgumentArr
// Imm
// Not-Defined
// Others
STATIC_TYPE operand_t::get_static_type() const
{
  if(this->type == OPERAND_IMM)
    return STATIC_IMM;
  if(this->type == OPERAND_NOT_DEFINED)
    return STATIC_NONE;
  myAssert(this->type == OPERAND_VAR,ASSERT_EXIT_CODE_117);
  if(this->name.size() < 2)
    {
      return STATIC_OTHERS;
    }
  if(this->name[0] == '~')
    {
      return STATIC_REFER_ARR;
    }
  if(this->name[0] == '$')
    {
      if(this->name[1] == '&')
	return STATIC_GLOBL_ARR;
      return STATIC_GLOBL_VAR;
    }
  if(this->name[0] == '%')
    {
      if(this->name[1] == '&')
	return STATIC_LOCAL_ARR;
      return STATIC_LOCAL_VAR;
    }
  return STATIC_OTHERS;
}

// TypeRefs:
// GloblVar  [OK]
// GloblArr  [OK]
// LocalStackVar
// LocalArr  [OK]  
// RefArr    [OK]
// Immediate [OK]

RUNTIME_TYPE operand_t::get_runtime_type() const
{
  if(this->type == OPERAND_IMM)
    return RUNTIME_IMM;
  if(this->type == OPERAND_NOT_DEFINED)
    return RUNTIME_NONE;
  myAssert(this->type == OPERAND_VAR,ASSERT_EXIT_CODE_118);
  if(this->name.size() < 2)
    {
      return RUNTIME_OTHERS;
    }

  if(this->name[0] == '$') // Global
    {
      if(this->name[1] == '&')
	return RUNTIME_GLOBL_ARR;
      return RUNTIME_GLOBL_VAR;
    }
  if(this->name[0] == '~') // Argument
    return RUNTIME_REF_ARR;
  if(this->name[0] == '@')
    return RUNTIME_ARG_VAR;
  myAssert(this->name[0] == '%', ASSERT_EXIT_CODE_6);
  if(this->name[1] == '&')
    {
      myAssert(this->rewrited_name[0] == '^', ASSERT_EXIT_CODE_7);
      return RUNTIME_LOCAL_ARR;
    }
  myAssert(this->rewrited_name != "", ASSERT_EXIT_CODE_8);
  if(this->rewrited_name[0] == '#')
    return RUNTIME_LOCAL_REG_VAR;
  myAssert(this->rewrited_name[0] == '^', ASSERT_EXIT_CODE_9);
  return RUNTIME_LOCAL_STACK_VAR;
}

void operand_t::print(std::ostream &out, std::string pred)
{
  if(this->type == OPERAND_IMM)
    {
      out<<COLOR_GREEN<<pred <<this->imm;
    }
  else if(this->type == OPERAND_VAR)
    {
      out<<COLOR_YELLOW<<pred<<this->name;
      if(this->rewrited_name != "")
	{
	  out<<COLOR_RED "|" COLOR_CYAN <<this->rewrited_name;
	}
    }
  else
    {
      myAssert(0, ASSERT_EXIT_CODE_10); // should not reach here
    }
}

IROper_t::IROper_t(OPERATION_TYPE oper) :
  oper(oper), dest(operand_t()), ops1(operand_t()),
  ops2(operand_t()), ops3(operand_t()), timestamp(0), line_number(0),
  valid(1), ref_count(0), out_current_loop(0)
{
}

IROper_t::IROper_t(OPERATION_TYPE oper,
		   const operand_t& dest) :
  oper(oper), dest(dest), ops1(operand_t()),
  ops2(operand_t()), ops3(operand_t()), timestamp(0), line_number(0),
  valid(1), ref_count(0), out_current_loop(0)
{
}

IROper_t::IROper_t(OPERATION_TYPE oper,
		   const operand_t& dest,
		   const operand_t& ops1) :
  oper(oper), dest(dest), ops1(ops1),
  ops2(operand_t()), ops3(operand_t()), timestamp(0), line_number(0),
  valid(1), ref_count(0), out_current_loop(0)
{
}

IROper_t::IROper_t(OPERATION_TYPE oper,
		   const operand_t& dest,
		   const operand_t& ops1,
		   const operand_t& ops2) :
  oper(oper), dest(dest), ops1(ops1),
  ops2(ops2), ops3(operand_t()), timestamp(0), line_number(0),
  valid(1), ref_count(0), out_current_loop(0)
{
}

IROper_t::IROper_t(OPERATION_TYPE oper,
		   const operand_t& dest,
		   const operand_t& ops1,
		   const operand_t& ops2,
		   const operand_t& ops3) :
  oper(oper), dest(dest), ops1(ops1),
  ops2(ops2), ops3(ops3), timestamp(0), line_number(0),
  valid(1), ref_count(0), out_current_loop(0)
{
}

bool IROper_t::operator<(const IROper_t &y) const
{
  if(this->oper != y.oper)
    return this->oper < y.oper;
  myAssert(this->oper == y.oper,ASSERT_EXIT_CODE_119);

  if(this->ops1 != y.ops1)
    return this->ops1 < y.ops1;
  myAssert(this->ops1 == y.ops1,ASSERT_EXIT_CODE_120);

  if(this->ops2 != y.ops2)
    return this->ops2 < y.ops2;
  myAssert(this->ops2 == y.ops2,ASSERT_EXIT_CODE_121);

  if(this->ops3 != y.ops3)
    return this->ops3 < y.ops3;
  myAssert(this->ops3 == y.ops3,ASSERT_EXIT_CODE_122);

  if(this->line_number != y.line_number)
    return this->line_number < y.line_number;
  myAssert(this->line_number == y.line_number,ASSERT_EXIT_CODE_123);
  if(this->dest != y.dest)
    return this->dest < y.dest;
  myAssert(this->dest == y.dest,ASSERT_EXIT_CODE_124);
  return false;
}
bool IROper_t::operator==(const IROper_t &y) const
{
    return ((!(*this<y)) && !(y<*this));
}

bool IROper_t::operator!=(const IROper_t &y) const
{
  return !((*this) == y);
}


bool _rhs_t::operator<(const _rhs_t &y) const
{
  if(this->oper != y.oper)
    return this->oper < y.oper;
  myAssert(this->oper == y.oper,ASSERT_EXIT_CODE_125);

  if(this->ops1 != y.ops1)
    return this->ops1 < y.ops1;
  myAssert(this->ops1 == y.ops1,ASSERT_EXIT_CODE_126);

  if(this->ops2 != y.ops2)
    return this->ops2 < y.ops2;
  myAssert(this->ops2 == y.ops2,ASSERT_EXIT_CODE_127);

  if(this->ops3 != y.ops3)
    return this->ops3 < y.ops3;
  myAssert(this->ops3 == y.ops3,ASSERT_EXIT_CODE_128);
  return false;
}
bool _rhs_t::operator==(const _rhs_t &y) const
{
    return ((!(*this<y)) && !(y<*this));
}

bool _rhs_t::operator!=(const _rhs_t &y) const
{
  return !((*this) == y);
}

_rhs_t::_rhs_t(OPERATION_TYPE oper,
	       operand_t ops1,
	       operand_t ops2,
	       operand_t ops3) :
  oper(oper), ops1(ops1), ops2(ops2), ops3(ops3)
{
}

_rhs_t IROper_t::get_rhs()
{
  return _rhs_t(oper, ops1, ops2, ops3);
}

/////////////////////////////////////////
///////IR N O T E/////////////////////////
/////////////////////////////////////////
IRNote_t::IRNote_t(const std::string& s) 
{
  note = s;
}

IRNote_t::IRNote_t() {}
/////////////////////////////////////////
///////P R I N T/////////////////////////
/////////////////////////////////////////
void IR_t::print(std::ostream& out, int levelspec)
{
  myAssert(0, ASSERT_EXIT_CODE_11);
}
void IROper_t::__print_oper_type(std::ostream& out)
{
  out << COLOR_CYAN;
#define CASE(x) \
  case (x): \
    out << LIT2STR(x);			\
    break;
  switch(this->oper)
    {
      CASE(OPER_STACK_ALLOC);
      CASE(OPER_STORE);
      CASE(OPER_LOAD);
      CASE(OPER_MOVE);
      CASE(OPER_FUNC);
      CASE(OPER_ENDF);
      CASE(OPER_RETN);
      CASE(OPER_RET);
      CASE(OPER_JMP);
      CASE(OPER_CMP);
      CASE(OPER_JEQ);
      CASE(OPER_JNE);
      CASE(OPER_JGT);
      CASE(OPER_JLT);
      CASE(OPER_JGE);
      CASE(OPER_JLE);
      CASE(OPER_NOP);
      CASE(OPER_LABEL);
      CASE(OPER_PHI_MOV);
      // CASE(OPER_PHI_EXIT);
      CASE(OPER_MOVEQ);
      CASE(OPER_MOVNE);
      CASE(OPER_MOVGT);
      CASE(OPER_MOVLT);
      CASE(OPER_MOVGE);
      CASE(OPER_MOVLE);
      CASE(OPER_ADD);
      CASE(OPER_ADDLT);
      CASE(OPER_ADD_OFFSET);
      CASE(OPER_SUB);
      CASE(OPER_MUL);
      CASE(OPER_DIV);
      CASE(OPER_SHL);
      CASE(OPER_SHR);
      CASE(OPER_SHRGE);
      CASE(OPER_SHRLT);
      CASE(OPER_SAR);
      CASE(OPER_SARGE);
      CASE(OPER_SARLT);
      CASE(OPER_SET_ARG);
      CASE(OPER_CALL);
      CASE(OPER_CALL_NRET);
      CASE(OPER_LOAD_ARG);
      
      CASE(OPER_SPACE);
      CASE(OPER_WORD);
      CASE(OPER_BLK_SEP);
      
    default:
      perror(COLOR_RED "Unknown Operator.");
      myAssert(0, ASSERT_EXIT_CODE_12);
    }
  
}
void IROper_t::print(std::ostream& out, int levelspec)
{
  ASTNode_t::__print_Indentation(levelspec, false, out);
  std::stringstream ss;
  if(this->timestamp != 0)
    {
      ss.setf(std::ios::left);
      ss << " " COLOR_PURPLE << std::setw(3) << this->timestamp;
    }
  else if(this->line_number != 0)
    {
      ss.setf(std::ios::left);
      ss << " " COLOR_GREEN << std::setw(3) << this->line_number;
    }
  ss << COLOR_YELLOW "IROper " << COLOR_CYAN;
  this->__print_oper_type(ss);
#define F(x, back)				  \
  if(this->x.type != OPERAND_NOT_DEFINED) \
    { \
      ss << back;	 \
      this->x.print(ss); \
    } \
  else \
    { \
      goto _IROper_print_out; \
    }
  if(this->oper == OPER_LABEL)
    // special processing for LABELs
    {
      ss << " ";
      this->dest.print(ss, COLOR_PURPLE);
      goto _IROper_print_out;
    }
  else
    {
      F(dest,  " ");
    }
  F(ops1, COLOR_RED " <- ");
  F(ops2, COLOR_RED ", ");
  F(ops3, COLOR_RED ", ");
#undef F  
 _IROper_print_out:    
  ASTNode_t::__print_message(ss.str().c_str(), out);
}

void IRData_t::print(std::ostream& out, int levelspec)
{
  ASTNode_t::__print_Indentation(levelspec, false, out);
  ASTNode_t::__print_message("IRData",out);
  for(auto it : this->data.data)
    {
      ASTNode_t::__print_Indentation(levelspec, false, out);
      IROper_t op(it.first, (it.first == OPER_SPACE)?(it.second*4):(it.second));
      op.print(out, -1);
    }
  // if(this->length == 4) // integer
  //   {
  //     ASTNode_t::__print_Indentation(levelspec, false, out);
  //     char buf[10];
  //     sprintf(buf,"%08X", this->data[0]);

  //     out << COLOR_CYAN " " << std::hex << "0x"<<buf<<" " COLOR_PURPLE "= "
  // 	  << COLOR_GREEN    << std::dec << this->data[0];
  //   }
  // else
  //   {
  //     for(int i = 0; i < this->length; i++)
  // 	{
  // 	  if(i % 8 == 0)
  // 	    {
  // 	      if(i)
  // 		out << std::endl;
  // 	      ASTNode_t::__print_Indentation(levelspec, false, out);
  // 	      char buf[25];
  // 	      sprintf(buf, "0x%04X", i);
  // 	      out << COLOR_YELLOW << buf << ": " << COLOR_CYAN << " ";
  // 	    }
  // 	  char buf[10];
  // 	  sprintf(buf,"%02X",(uint32_t) (((uint8_t*) (this->data))[i]));
  // 	  out <<buf
  // 	      << " ";
  // 	}
  //   }
  // out<< std::dec << std::endl;

}

void IRLabel_t::print(std::ostream& out, int levelspec)
{
  ASTNode_t::__print_Indentation(levelspec, false, out);
  std::stringstream ss;
  ss << COLOR_YELLOW "IRLabel " << COLOR_CYAN;
  this->__print_label_type(ss);
  ss <<" "<<COLOR_GREEN << this->label_name;
  ASTNode_t::__print_message(ss.str().c_str(), out);
}

void IRList_t::print(std::ostream& out, int levelspec)
{
  if(this->body.size() == 0) return;
  ASTNode_t::__print_Indentation(levelspec, false, out);
  ASTNode_t::__print_message("IRList", out, this->note);
  {
    std::stringstream ss;
    ss << "PHI Vars = ";
    if(this->phi_vars.empty())
      ss << COLOR_YELLOW "NONE" COLOR_GREEN;
    else
      {
	for(auto it : this->phi_vars)
	  {
	    ss<<it<<" ";
	  }
      }
    ASTNode_t::__print_Indentation(levelspec, false, out);
    ASTNode_t::__print_message(ss.str().c_str(), out);
  }
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      if((*p) != NULL)
	(*p)->print(out, levelspec+1);
    }
}

void IRLabel_t::__print_label_type(std::ostream& out)
{
#define CASE(x) \
  case (x): \
    out << LIT2STR(x);				\
    break;
  switch(this->type)
    {
      CASE(DATA_AREA_BEGIN);
      CASE(DATA_AREA_END);
    default:
      perror(COLOR_RED "Unknown Token.");
      myAssert(0, ASSERT_EXIT_CODE_13);
    }
}

void IRNote_t::print(std::ostream& out, int levelspec)
{
  ASTNode_t::__print_Indentation(levelspec, false, out);
  std::stringstream ss;
  ss << COLOR_GREEN "IRNote" << COLOR_CYAN;
  ASTNode_t::__print_message(ss.str().c_str(), out, this->note);
}

void IRSep_t::print(std::ostream &out, int levelspec)
{
  ASTNode_t::__print_Indentation(levelspec, false, out);
  std::stringstream ss;
  ss << COLOR_GREEN "-------IRSeparator-------" << COLOR_CYAN;
  ASTNode_t::__print_message(ss.str().c_str(), out, this->note);

}
//////////////////////////////
//////C O L L E C T///////////
////////F U N C T I O N///////
//////////////////////////////

void IR_t::collect(std::list<IR_t*>& li)
{
  myAssert(0, ASSERT_EXIT_CODE_14);
}

void IRList_t::collect(std::list<IR_t*>& li)
{
  for(auto& it : this->body)
    {
      if(it != NULL)
	it->collect(li);
    }
}

void IROper_t::collect(std::list<IR_t*>& li)
{
  if(this->oper == OPER_PHI_MOV)
    {
      myAssert( this->dest.type == OPERAND_VAR, ASSERT_EXIT_CODE_15);
      std::string name = this->dest.name;
    }
  
  li.push_back(this);
}

void IRData_t::collect(std::list<IR_t*>& li)
{
  // li.push_back(this);
  for(auto it : this->data.data)
    {
      IROper_t& op = *(new IROper_t(it.first, (it.first == OPER_SPACE)?(it.second*4):(it.second)));
      li.push_back(&op);
    }
}

void IRLabel_t::collect(std::list<IR_t*>& li)
{
  li.push_back(this);
}

void IRNote_t::collect(std::list<IR_t*>& li)
{
  // li.push_back(this);
  return;
}

