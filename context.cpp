#include "context.h"
#include "common.h"
#include "exceptions.h"
#include<cstdio>
#include<typeinfo>
#include<assert.h>
#include<iostream>
#include<string>
context_t global_ctx;
int unique_id = 1;

arg_t::arg_t(argtype_t __argtype) : __argtype(__argtype)
{
  myAssert(__argtype == ARG_SINGLE, ASSERT_EXIT_CODE_16);
}

arg_t::arg_t(argtype_t __argtype, std::vector<int>& dims) :
  __argtype(__argtype), dimlist(dims)
{
  myAssert(__argtype == ARG_ARRAY, ASSERT_EXIT_CODE_17);
}

bool arg_t::is_array()
{
  return this->__argtype == ARG_ARRAY;
}
// sym_t::sym_t(const std::string& name,
// 	     bool arrayQ,
// 	     const std::vector<int>& dimlist) :
//   name(name), arrayQ(arrayQ), dimlist(dimlist)
// {
// }

// sym_t::sym_t(const std::string& name,
// 	     bool arrayQ) :
//   name(name), arrayQ(arrayQ)
// {
// }

sym_t::sym_t(const std::string& name,
	     _symtype_t type) : name(name)
{

}

sym_t::sym_t(const std::string& name,
	     _symtype_t type,
	     const std::vector<int>& typelist) : name(name)
{
  myAssert(this->is_array(), ASSERT_EXIT_CODE_18);
  this->dimlist = typelist;
}

sym_t::sym_t(const std::string& name,
	     _symtype_t type,
	     const std::vector<arg_t*>& typelist) : name(name)
{
  myAssert(this->is_func(), ASSERT_EXIT_CODE_19);
  this->arglist = typelist;
}


bool sym_t::is_array()
{
  myAssert(this->name.size() >= 2, ASSERT_EXIT_CODE_20); // at least two elements
  if(this->name[0] == '~')
    return true;
  if(this->name[1] == '&')
    return true;
  else
    return false;
  myAssert(0, ASSERT_EXIT_CODE_21);
  // should not reach here
}

bool sym_t::is_global()
{
  myAssert(this->name.size() >= 2, ASSERT_EXIT_CODE_22); // at least two elements
  if(this->name[0] == '$')
    {
      return true;
    }
  else
    {
      myAssert(this->name[0] == '%', ASSERT_EXIT_CODE_23);
      return false;
    }
}

bool sym_t::is_arg()
{
  myAssert(this->name.size() >= 2, ASSERT_EXIT_CODE_24); // at least two elements
  if(this->name[0] == '@')
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool sym_t::is_func()
{
  myAssert(this->name.size() >= 2, ASSERT_EXIT_CODE_25);
  if(this->name[0] == '!')
    {
      return true;
    }
  else
    {
      return false;
    }
}

constsym_t::constsym_t(int value) :
  value({value}), dimlist({}), arrayQ(false)
{
}

constsym_t::constsym_t(const std::vector<int>& dimlist,
		       const std::vector<int>& vallist) :
  dimlist(dimlist), value(vallist), arrayQ(true)
{
}

void constsym_t::print() const
{
  if(this->arrayQ)
    {
      for(int i = 0; i < this->value.size(); i++)
	{
	  std::cerr<<this->value[i]<<" ";
	}
    }
}
void sym_t::print(std::ostream& out)
{
  out << this->name << ":";
}

void context_t::var_table_push(std::string sym_name,
			       const sym_t& sym_info)
{
  // std::cerr<<COLOR_YELLOW "Push into SymbolTable : "
  // 	   <<COLOR_CYAN << sym_name << COLOR_RED << " -> "
  // 	   <<COLOR_GREEN << sym_info.name <<  COLOR_YELLOW " at level " COLOR_GREEN
  // 	   <<this->symbol_table.size()<<std::endl;
  
  myAssert(this->symbol_table.size() >= 1, ASSERT_EXIT_CODE_26);
  
  if(  (this->symbol_table.back()).count(sym_name) )
    {
      printf("Symbol of %s\n", sym_name.c_str());
      COMPILE_ERROR("Symbol Redefinition");
    }
  (this->symbol_table.back()).insert(std::make_pair(sym_name,sym_info));
  return;
}

void context_t::var_table_push_globl(std::string sym_name,
				     const sym_t& sym_info)
{
  // std::cerr<<COLOR_YELLOW "Push into SymbolTable's Global Scope : "
  // 	   <<COLOR_CYAN << sym_name << COLOR_RED << " -> "
  // 	   <<COLOR_GREEN << sym_info.name<<std::endl;
  
  myAssert(this->symbol_table.size() >= 1, ASSERT_EXIT_CODE_27);
  
  if(  (this->symbol_table.front()).count(sym_name) )
    {
      COMPILE_ERROR("Symbol Redefinition");
    }
  (this->symbol_table.front()).insert(std::make_pair(sym_name,sym_info));
  return;
}

sym_t& context_t::var_table_find(std::string sym_name)
{
  // printf("Finding: %s\n", sym_name.c_str());
  for(int i = this->symbol_table.size() - 1;
      i >= 0;
      i--)
    {
      if(symbol_table[i].count(sym_name))
  	{
  	  return symbol_table[i].at(sym_name);
  	}
    }
  // err
  throw ex_symbol_not_found;
}

sym_t& context_t::var_table_find_globl(std::string sym_name)
{
  myAssert(this->symbol_table.size() >= 1, ASSERT_EXIT_CODE_28);
  if(symbol_table[0].count(sym_name))
    {
      return symbol_table[0].at(sym_name);
    }
  
  // err
  throw ex_symbol_not_found;
}

void context_t::var_table_rewrite(const std::string& sym_name,
				  const std::string& new_label)
{
  auto& p = this->var_table_find(sym_name);
  myAssert(!p.is_global(), ASSERT_EXIT_CODE_29);
  myAssert(!p.is_array(), ASSERT_EXIT_CODE_30);
  // std::cerr<<COLOR_YELLOW "Label Name Rewrite ["
  // 	   <<COLOR_CYAN << sym_name << COLOR_YELLOW<<"]:"
  // 	   << COLOR_CYAN << p.name << COLOR_RED <<" -> "
  // 	   << COLOR_CYAN << new_label
  // 	   << COLOR_YELLOW " at level " << COLOR_GREEN
  // 	   <<this->const_symbol_table.size()<<std::endl;
  p.name = new_label;
  
}



void context_t::const_table_push(std::string sym_name,
			       const constsym_t& sym_info)
{
  // std::cerr<<COLOR_YELLOW "Push into ConstTable  : "
  // 	   <<COLOR_CYAN << sym_name << COLOR_YELLOW " at level " << COLOR_GREEN
  // 	   <<this->const_symbol_table.size()<<std::endl;
  // sym_info.print();
  myAssert(this->const_symbol_table.size() >= 1, ASSERT_EXIT_CODE_31);
  
  if(  (this->const_symbol_table.back()).count(sym_name) )
    {
      COMPILE_ERROR("Const Symbol Redefinition");
    }
  (this->const_symbol_table.back()).insert(std::make_pair(sym_name,sym_info));
  return;
}

constsym_t& context_t::const_table_find(std::string sym_name)
{
  for(int i = this->const_symbol_table.size() - 1;
      i >= 0;
      i--)
    {
      if(const_symbol_table[i].count(sym_name))
  	{
  	  return const_symbol_table[i].at(sym_name);
  	}
    }
  // err
  // printf("Const Table Finding: %s", sym_name.c_str());
  // myAssert(0, ASSERT_EXIT_CODE_32);
  throw ex_symbol_not_found;
}


void context_t::const_assign_push(std::string sym_name,
				  const constsym_t& sym_info)
{
  // std::cerr<<COLOR_YELLOW "Push into Const Assignment  : "
  // 	   <<COLOR_CYAN << sym_name
  // 	   << COLOR_RED " = " << COLOR_GREEN << sym_info.value[0]
  // 	   << COLOR_YELLOW " at level " << COLOR_GREEN
  // 	   <<this->const_assign_table.size()<<std::endl;
  // sym_info.print();
  myAssert(this->const_assign_table.size() >= 1, ASSERT_EXIT_CODE_33);
  
  if(  (this->const_assign_table.back()).count(sym_name) )
    {
      myAssert(0, ASSERT_EXIT_CODE_34);
      // this condition shall not happen
    }
  (this->const_assign_table.back()).insert(std::make_pair(sym_name,sym_info));
  return;
}

constsym_t& context_t::const_assign_find(std::string sym_name)
{
  for(int i = this->const_assign_table.size() - 1;
      i >= 0;
      i--)
    {
      if(const_assign_table[i].count(sym_name))
  	{
  	  return const_assign_table[i].at(sym_name);
  	}
    }
  // err
  throw ex_symbol_not_found;
}

int context_t::get_unique_id()
{
  this->var_table_push(LOCAL_VAR(std::to_string(unique_id)), sym_t(LOCAL_VAR(std::to_string(unique_id)), SYMTYPE_SING));
  // Push %unique_id -> %unique_id
  // For expression evaluation code reuse
  
  return unique_id++;
  // [id] is shared among all [context_t] objects
}

context_t::context_t()
{
  symbol_table = {{}};
  const_symbol_table = {{}};
  const_assign_table = {{}};
}

bool context_t::is_global_scope()
{
  myAssert(symbol_table.size() == const_symbol_table.size(), ASSERT_EXIT_CODE_35);
  return symbol_table.size() == 1;
}

void context_t::scope_push()
{
  // printf("%p: pushing level %d\n",this, this->symbol_table.size());
  this->symbol_table.push_back({});
  this->const_symbol_table.push_back({});
  this->const_assign_table.push_back({});
}

void context_t::scope_pop()
{
  myAssert(symbol_table.size() > 1 &&
	 symbol_table.size() == const_symbol_table.size() &&
	   symbol_table.size() == const_assign_table.size(), ASSERT_EXIT_CODE_36);
  this->symbol_table.pop_back();
  this->const_symbol_table.pop_back();
  this->const_assign_table.pop_back();
  // printf("%p: popping level %d\n",this, this->symbol_table.size());
}
///////////////////////////////////////
////L O O P////////////////////////////
///////////////////////////////////////
bool context_t::in_loop()
{
  return !loop_label.empty();
}
