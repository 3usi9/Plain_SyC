#ifndef _IR_H
#define _IR_H
#include<list>
#include<string>
#include<vector>
#include<set>
#include<stdint.h>
#include<assert.h>
#include<map>

/////////////////////////

///////////////////////
enum ADDR_MODE_T
  {
    
  };

enum OPERAND_TYPE
  {
    OPERAND_NOT_DEFINED = 0,
    OPERAND_IMM = 1,
    OPERAND_VAR = 2
  };

enum STATIC_TYPE // IR-level type specification
  {
    // Vars
    STATIC_GLOBL_VAR,
    STATIC_LOCAL_VAR,
    // Arrs
    STATIC_GLOBL_ARR,
    STATIC_LOCAL_ARR,
    STATIC_REFER_ARR,
    // Immediate
    STATIC_IMM,
    // Not-Defined
    STATIC_NONE,
    // Others
    STATIC_OTHERS,
  };
enum RUNTIME_TYPE // CodeGen-level type specification
  {
    // Vars
    RUNTIME_GLOBL_VAR,
    RUNTIME_LOCAL_REG_VAR,
    RUNTIME_LOCAL_STACK_VAR,
    RUNTIME_ARG_VAR,
    // Arrs
    RUNTIME_GLOBL_ARR,
    RUNTIME_LOCAL_ARR,
    RUNTIME_REF_ARR,
    // Immediate
    RUNTIME_IMM,
    // Others
    RUNTIME_NONE,
    RUNTIME_OTHERS
  };
class operand_t
{
public: // Syntax Analyze Stage
  OPERAND_TYPE type;
  std::string name; // for OPERAND_VAR
  int imm;        // for OPERAND_IMM
  
public: // Operators for comparison
  bool operator==(const operand_t& y) const;
  bool operator!=(const operand_t& y) const;
  bool operator<(const operand_t& y) const;
  STATIC_TYPE get_static_type() const;
  
public: // CodeGen Stage, for register allocation
  std::string rewrited_name;
  operand_t();
  operand_t(const std::string& name);
  operand_t(int imm);
  void print(std::ostream& out, std::string pred = "");

  RUNTIME_TYPE get_runtime_type() const;

};
class IR_t
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs


  std::string note;
  
public: // Optimize
  virtual
  std::set<std::string> get_phi_vars();

  virtual
  int set_linenum(int linenum);

  virtual
  void rewrite_varname(const std::map<std::string, std::string>& rules);
  // Rewrite [prev_name] -> [new_name] under all childs of the tree
  // If for a oper, [dest] is EQUAL to [prev_name]
  // Delete this node.

  virtual
  void refresh_validation();
  // Delete those instruction marked as [invalid]
};

class IRList_t : public IR_t
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs

public:
  // IRs in AST node
  // might be flatten to IR sequence
  std::list<IR_t*> body;
  void push_back(IR_t* ir);

public: // Optimize
  std::set<std::string> phi_vars;
  virtual
  std::set<std::string> get_phi_vars();
  // PHI Vars for this block.
  virtual
  int set_linenum(int linenum);

  virtual
  void rewrite_varname(const std::map<std::string, std::string>& rules);
  // Rewrite [prev_name] -> [new_name] under all childs of the tree
  // If for a oper, [dest] is EQUAL to [prev_name]
  // Delete this node.

  virtual
  void refresh_validation();
  // Delete those instruction marked as [invalid]
  
};


enum OPERATION_TYPE
  {
    OPER_STACK_ALLOC,  /* [STACK ALLOC]
			* DEST: [NAME:VAR]
			* OPS1: [SIZE:IMM]
			* <PSEUDO INSTRUCTION>
			*/
    OPER_LOAD,        /* [LOAD]
			* DEST: [NAME:VAR]
			* OPS1: [BASE:VAR]
			* OPS2: [VALUE:IMM]
			* LOAD  OPS1(OPS2) -> DEST
			*/
    
    OPER_STORE,        /* [STORE]
			* DEST: [NAME:VAR]
			* OPS1: [OFFSET:IMM]
			* OPS2: [VALUE:IMM / VAR(reg)]
			* STORE DEST(OPS1) <- OPS2
			*/
    OPER_MOVE,         /* [MOVE]
			* DEST: [NAME:VAR]
			* OPS1: [NAME:VAR / IMM]
			* MOVE, [DEST] CAN BE ASSIGNED ONLY ONCE
			*/
    OPER_FUNC,         /* [FUNC]
			* DEST: [FUNCTION_NAME:VAR]
			* OPS1: [FUNCTION_ARGS:IMM]
			*/
    OPER_ENDF,         /* [ENDF]
			* DEST: [FUNCTION_NAME:VAR]
			*/

    OPER_RETN,         /* [RETN] Return with value
			* DEST: [RETVAL: IMM/VAR]
			*/
    OPER_RET,          /* [RET] Return without value
			*/
    OPER_JMP,          /* [JMP]
			* DEST: [TARGET:VAR]
			*/

    OPER_CMP,           // [CMP] 
			// DEST: [CMP1]
			// OPS1: [CMP2]
    
    OPER_JEQ,          // [Jcc] 
    OPER_JNE,          // DEST: [TARGET:VAR]
    OPER_JGT,
    OPER_JLT,
    OPER_JGE,
    OPER_JLE,

    OPER_NOP,          /* [NOP] */

    OPER_LABEL,        /* [LABEL] 
			* DEST: [TAGNAME:VAR]
			* <PSEUDO INSTRUCTION>
			*/

    OPER_PHI_MOV,      /* [PHI_MOV] 
			* DEST: [GLOBL:VAR]
			* OPS1: [LOCAL:VAR]
			* <PSEUDO INSTRUCTION>
			* MOVE, MIGHT BE MULTIPLE ASSIGNMENT TO [DEST]
			* (other instruction's target is also [DEST])
			*/
    OPER_MOVEQ,         // [MOVcc] 
    OPER_MOVNE,         // DEST: [TARGET:VAR]
    OPER_MOVGT,         // OPS1: [SUCCESS:IMM]
    OPER_MOVLT,         // OPS2: [FAIL:IMM]
    OPER_MOVGE,
    OPER_MOVLE,

    OPER_ADD,           // [Arith]
    OPER_ADDLT,           // [Arith]
    OPER_SUB,           // DEST: [RESULT:VAR]
    OPER_MUL,           // OPS1: [OPERAND1:VAR]
    OPER_DIV,        // OPS2: [OPERAND2:VAR/IMM]

    /* OPER_MOD, */
    // Raspberry-Pi Does NOT support MOD    
    OPER_SHL,
    OPER_SHR,
    OPER_SHRGE,
    OPER_SHRLT,
    OPER_SAR,
    OPER_SARGE,
    OPER_SARLT,
    OPER_ADD_OFFSET,    // Add Memory Offset
    OPER_SET_ARG,       // [SET Argument for Function]
                        // DEST: [Argument Order Number:IMM]
                        // OPS1: [Argument:VAR/IMM]

    /* OPER_SET_ARG_PTR,   // [SET Reference Argument for Function] */
    /*                     // DEST: [Argument Order Number:IMM] */
    /*                     // OPS1: [Argument:VAR/IMM, Represent an address] */
    /*                     // [ If OPS1 is Reg, Load Reg's Memory Location ] */
    

    OPER_CALL,          // [CALL]
                        // DEST: [Retn Value Store/VAR]
                        // OPS1: [Calling Target/SYMBOL]
    OPER_CALL_NRET,     // [CALL without Returning]

    OPER_LOAD_ARG,      // [LOAD Argument for Function]
                        // DEST: [Load Target:VAR]
                        // OPS1: [Argument Order Number:IMM]
    
    OPER_SPACE,         // [SPACE]
                        // DEST: [Reserved Count]
                        // PSEUDO INSTRUCTION
    OPER_WORD,          // [WORD]
                        // DEST: [Value]
                        // PSEUDO INSTRUCTION
    OPER_BLK_SEP,       // [Block Separator]
                        // PSEUDO INSTRUCTION
    
  };

class _rhs_t
{
public:
  OPERATION_TYPE oper;
  operand_t ops1, ops2, ops3;
  bool operator==(const _rhs_t& y) const;
  bool operator!=(const _rhs_t& y) const;
  bool operator<(const _rhs_t& y) const;
  _rhs_t(OPERATION_TYPE oper,
	 operand_t ops1,
	 operand_t ops2,
	 operand_t ops3);
};


class IROper_t : public IR_t
{

public: // generate IR
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs

  // operation IRs
  OPERATION_TYPE oper;
  operand_t dest, ops1, ops2, ops3;
  
  IROper_t* phi_target;
  // refer to the Elimination Point of this PHI Move
  // buggy, due to deep copy not covered this variable

public: // Operators
  
  // implicit instr
  IROper_t(OPERATION_TYPE oper);

  // unary  
  IROper_t(OPERATION_TYPE oper,
	   const operand_t& dest);

  // binary
  IROper_t(OPERATION_TYPE oper,
	   const operand_t& dest,
	   const operand_t& ops1);

  // triple
  IROper_t(OPERATION_TYPE oper,
	   const operand_t& dest,
	   const operand_t& ops1,
	   const operand_t& ops2);

  // quad
  IROper_t(OPERATION_TYPE oper,
	   const operand_t& dest,
	   const operand_t& ops1,
	   const operand_t& ops2,
	   const operand_t& ops3);
  
public: // Operators for comparison
  bool operator==(const IROper_t& y) const;
  bool operator!=(const IROper_t& y) const;
  bool operator<(const IROper_t& y) const;
  _rhs_t get_rhs();
  void __print_oper_type(std::ostream& out);
  
public: // generate Code
  int timestamp;
  
public: // Optimize
  int line_number;
  int valid;
  
  int ref_count;
  // Optimize MUL to MLA or MLS
  int out_current_loop;
  // Optimize Invariant Code Motion (IRLine Stage) and Phi-Elimination (Flow-stage)
  
  virtual
  std::set<std::string> get_phi_vars();
  virtual
  int set_linenum(int linenum);

  virtual
  void rewrite_varname(const std::map<std::string, std::string>& rules);
  // Rewrite [prev_name] -> [new_name] under all childs of the tree
  // If for a oper, [dest] is EQUAL to [prev_name]
  // Delete this node.

};

class _irdata_global_area
{
public:
  std::list< std::pair<OPERATION_TYPE, int> > data;
  int& operator[](int idx);
  _irdata_global_area(int cnt);
  
};

class IRData_t : public IR_t
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs

public:
  int length; // unit: bytes
  // uint32_t* data;
  _irdata_global_area data;
  IRData_t(int length,
	   const _irdata_global_area& data);
  IRData_t(int length); // initialize to 0
};




enum LABEL_TYPE
  {
    DATA_AREA_BEGIN,    // data-area
    DATA_AREA_END       // data-area
  };
class IRLabel_t : public IR_t
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs

  void __print_label_type(std::ostream& out);

public:
  IRLabel_t(LABEL_TYPE type);
  IRLabel_t(LABEL_TYPE type, std::string label_name);
  
  // Label IRs
  LABEL_TYPE type;
  std::string label_name;
  // Decayed, not using [ident_t], but use [std::string]
  // AST-defined types should not be used in IR generating
  
public: // Optimize
};

class IRNote_t : public IR_t
// comment node
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

  virtual
  void collect(std::list<IR_t*>& li);
  // collect all operation IRs

  IRNote_t(const std::string& s);
  IRNote_t();

public: // Optimize
};

class IRSep_t : public IR_t
// Basic Block's Separator, used in CodeGen Phase
{
public:
  virtual
  void print(std::ostream& out, int levelspec);

public: // Optimize
};

#endif
