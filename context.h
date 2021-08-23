#ifndef _CONTEXT_H
#define _CONTEXT_H
#include<list>
#include<map>
#include<string>
#include<vector>
#include<stack>
#include "exceptions.h"
#include "IR.h"
enum argtype_t
  {
    ARG_SINGLE,
    ARG_ARRAY
  };
  
class arg_t
// single argument
{
public:
  argtype_t __argtype;
  bool is_array();
  std::vector<int> dimlist;
  // for array, record its dimlist
  arg_t(argtype_t __argtype);
  arg_t(argtype_t __argtype, std::vector<int>& dims);
};

enum _symtype_t
// single, array, or function
  {
    SYMTYPE_SING = 0,
    SYMTYPE_ARRAY = 1,
    SYMTYPE_FUNC = 2
  };

class sym_t
{
public:
  std::string name;
  // label name, processed
  // for global variables, name = $[&] + [varname]
  // for local variables(anonymous), name = %[&] + [sequence]
  // for function variables, name = ! + [funcname]
  
  std::vector<int> dimlist;
  // array

  std::vector<arg_t*> arglist;
  // function formal argument list

  // bool arrayQ;
  // // Bad design
  
public:
  // sym_t(const std::string& name,
  // 	bool arrayQ,
  // 	const std::vector<int>& dimlist);
  // sym_t(const std::string& name,
  // 	bool arrayQ);

  sym_t(const std::string& name,
	_symtype_t type);

  sym_t(const std::string& name,
	_symtype_t type,
	const std::vector<int>& typelist);

  sym_t(const std::string& name,
	_symtype_t type,
	const std::vector<arg_t*>& typelist);
  

  void print(std::ostream& out);
  bool is_array();
  bool is_global();
  bool is_arg();
  bool is_func();
};

class constsym_t
{
public:
  std::vector<int> dimlist;
  std::vector<int> value;
  bool arrayQ;

  void print() const;
public:
  constsym_t(int value); // Not-array
  constsym_t(const std::vector<int>& dimlist,
	     const std::vector<int>& vallist); // Is-array
	     
};

class context_t
/* Accessing Context */
{
public: // we may need to directly use these two variables
  typedef std::vector< std::map<std::string, sym_t> > symbol_table_t;
  typedef std::vector< std::map<std::string, constsym_t> > const_symbol_table_t;
  symbol_table_t symbol_table;
  const_symbol_table_t const_symbol_table;

  const_symbol_table_t const_assign_table;
  // for SSA forms, if a variable is assigned to a IMMEDIATE,
  // it will be a constant too
public:
  context_t();

  void var_table_push(  std::string sym_name,
			const sym_t& sym_info);
  
  void var_table_push_globl(std::string sym_name,
			    const sym_t& sym_info);

// sym-name is not-literaled
  void const_table_push(std::string sym_name,
			const constsym_t& sym_info);

  void const_assign_push(std::string sym_name,
			 const constsym_t& sym_info);
  
  sym_t& var_table_find(std::string sym_name);
  sym_t& var_table_find_globl(std::string sym_name);
  constsym_t& const_table_find(std::string sym_name);
  constsym_t& const_assign_find(std::string sym_name);

  void var_table_rewrite(const std::string& sym_name,
			 const std::string& new_label);
  void scope_push();
  void scope_pop();

  bool is_global_scope();
  int get_unique_id();

public: // loop processing
  std::stack<int> loop_label;
  std::stack<std::vector<std::string> > loop_vars;
  bool in_loop();
  void loop_label_pop();
  void loop_scope_push();
  void loop_scope_pop();

  std::stack<std::vector<symbol_table_t> > loop_conti_symbol_snapshot;
  std::stack<std::map<std::pair<int, std::string>, std::string> > loop_conti_rewrite;
  std::stack<std::vector<symbol_table_t> > loop_break_symbol_snapshot;
  std::stack<std::map<std::pair<int, std::string>, std::string> > loop_break_rewrite;

  std::stack<std::map< std::pair<int, std::string>, std::string> > rewritten; // track all symbols that needs to be rewritten
  // std::stack<std::map< std::string, std::pair<int, std::string>> > phi_vars; // track all phi_vars and their attached target
  std::stack<IROper_t*> phi_target; // track current phi target

};


#endif
