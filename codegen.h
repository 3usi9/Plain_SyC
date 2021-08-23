#ifndef _CODEGEN_H
#define _CODEGEN_H
#include "IR.h"
#include<sstream>
#include<iostream>
#include<map>
#include<set>
#include<bitset>

/////////////////////////////////////////////
////U T I L I T I E S////////////////////////
/////////////////////////////////////////////
bool is_special_register(std::string name);
bool is_register(std::string name);

int var_to_int(const std::string& var, int exit_code = -1);
enum MNEMONIC_TYPE
  {
    MNEMO_RAW,     // Raw Assembly
    MNEMO_ADD,     // Addition
    MNEMO_SUB,     // Subtraction
    MNEMO_SDIV,    // Signed Divide
    MNEMO_MOV,     // Register Move
    MNEMO_MOV32,   // 32-bits Immediate Move
    MNEMO_LDR,     // Load  MemRef
    MNEMO_STR,     // Store MemRef
    MNEMO_LDM,     // Load Multiple
    MNEMO_STM,     // Store Multiple
    MNEMO_LDR_GLB, // Load Global Reference
    MNEMO_MUL,     // Multiple
    MNEMO_LSL,     // Logical Shift Left
    MNEMO_LSR,     // Logical Shift Right
    MNEMO_ASR,     // Arith   Shift Right

    MNEMO_MLA,     // MLA
    MNEMO_MLS,     // MLS

    MNEMO_CMP,     // Compare

    MNEMO_B,       // Branch
    MNEMO_BL,      // Branch and Link

    MNEMO_NOP,     // No Operation
    MNEMO_LABEL,   // Labels
    MNEMO_NOTE,    // Comments
    MNEMO_PASSNOTE,// Specified Notes
    MNEMO_BLK_SEP, // Block Separator
  };

const static std::map<MNEMONIC_TYPE, std::vector<int> > MNEMO_REGISTERS =
  // vector: [ dest, ops1, ops2, ops3 ]
  // 0: REFERENCE or NOT-USED
  // 1: DEFINE
  { 
    { MNEMO_RAW,     {0, 0, 0, 0} },
    { MNEMO_ADD,     {1, 0, 0, 0} },
    { MNEMO_SUB,     {1, 0, 0, 0} },
    { MNEMO_SDIV,    {1, 0, 0, 0} },
    { MNEMO_MOV,     {1, 0, 0, 0} },
    { MNEMO_MOV32,   {1, 0, 0, 0} },
    { MNEMO_LDR,     {1, 0, 0, 0} },
    { MNEMO_STR,     {0, 0, 0, 0} },
    { MNEMO_LDR_GLB, {1, 0, 0, 0} },
    { MNEMO_MUL,     {1, 0, 0, 0} },
    { MNEMO_LSL,     {1, 0, 0, 0} },
    { MNEMO_LSR,     {1, 0, 0, 0} },
    { MNEMO_ASR,     {1, 0, 0, 0} },
    { MNEMO_MLA,     {1, 0, 0, 0} },
    { MNEMO_MLS,     {1, 0, 0, 0} },
    { MNEMO_CMP,     {0, 0, 0, 0} },
    { MNEMO_B,       {0, 0, 0, 0} },
    { MNEMO_BL,      {0, 0, 0, 0} },
    { MNEMO_NOP,     {0, 0, 0, 0} },
    { MNEMO_LABEL,   {0, 0, 0, 0} },
    { MNEMO_NOTE,    {0, 0, 0, 0} },
    { MNEMO_PASSNOTE,{0, 0, 0, 0} },
    { MNEMO_BLK_SEP, {0, 0, 0, 0} },
  };


struct _asm_rhs_t
{
public:
  MNEMONIC_TYPE mnemo;
  operand_t ops1, ops2, ops3;
  bool operator==(const _asm_rhs_t& y) const;
  bool operator!=(const _asm_rhs_t& y) const;
  bool operator<(const _asm_rhs_t& y) const;
  _asm_rhs_t(MNEMONIC_TYPE mnemo,
	     operand_t ops1,
	     operand_t ops2,
	     operand_t ops3);
};

class instr_t
{
public:
  MNEMONIC_TYPE mnemonic;
  std::string condition_string;
  int absolute_offset;

  operand_t dest, ops1, ops2, ops3;
  instr_t(MNEMONIC_TYPE mnemonic);
  instr_t(MNEMONIC_TYPE mnemonic,
	   const operand_t& dest);

  instr_t(MNEMONIC_TYPE mnemonic,
	   const operand_t& dest,
	   const operand_t& ops1);

  // triple
  instr_t(MNEMONIC_TYPE mnemonic,
	   const operand_t& dest,
	   const operand_t& ops1,
	   const operand_t& ops2);

  // quad
  instr_t(MNEMONIC_TYPE mnemonic,
	   const operand_t& dest,
	   const operand_t& ops1,
	   const operand_t& ops2,
	   const operand_t& ops3);

  void print(std::ostream& out);
public: // optimize
  int ref_count;
  int line_number;
  _asm_rhs_t get_rhs();
  
};

const static std::map<OPERATION_TYPE, std::vector<int> > OPERATION_REGISTERS =
  // vector: [ dest, ops1, ops2, ops3 ]
  // 0: REFERENCE or NOT-USED
  // 1: DEFINE
  { 
    { OPER_STACK_ALLOC, {1, 0, 0, 0} },      /* [STACK ALLOC]
					      * DEST: [NAME:VAR]
					      * OPS1: [SIZE:IMM]
					      * <PSEUDO INSTRUCTION>
					      */
    {OPER_LOAD, {1, 0, 0, 0} },              /* [LOAD]
					      * DEST: [NAME:VAR]
					      * OPS1: [BASE:VAR]
					      * OPS2: [VALUE:IMM]
					      * LOAD  OPS1(OPS2) -> DEST
					      */
    
    {OPER_STORE, {0, 0, 0, 0} },             /* [STORE]
					      * DEST: [NAME:VAR]
					      * OPS1: [OFFSET:IMM]
					      * OPS2: [VALUE:IMM / VAR(reg)]
					      * STORE DEST(OPS1) <- OPS2
					      */
    {OPER_MOVE, {1, 0, 0, 0} },              /* [MOVE]
					      * DEST: [NAME:VAR]
					      * OPS1: [NAME:VAR / IMM]
					      * MOVE, [DEST] CAN BE ASSIGNED ONLY ONCE
					      */
    {OPER_FUNC, {0, 0, 0, 0} },              /* [FUNC]
					      * DEST: [FUNCTION_NAME:VAR]
					      * OPS1: [FUNCTION_ARGS:IMM]
					      */
    {OPER_ENDF, {0, 0, 0, 0} },              /* [ENDF]
					      * DEST: [FUNCTION_NAME:VAR]
					      */

    {OPER_RETN, {0, 0, 0, 0} },              /* [RETN] Return with value
					      * DEST: [RETVAL: IMM/VAR]
					      */
    {OPER_RET, {0, 0, 0, 0} },               /* [RET] Return without value
					      */
    {OPER_JMP, {0, 0, 0, 0} },               /* [JMP]
					      * DEST: [TARGET:VAR]
					      */

    {OPER_CMP, {0, 0, 0, 0} },                // [CMP] 
			                      // DEST: [CMP1]
			                      // OPS1: [CMP2]
    
    {OPER_JEQ, {0, 0, 0, 0} },                // [Jcc] 
    {OPER_JNE, {0, 0, 0, 0} },                // DEST: [TARGET:VAR]
    {OPER_JGT, {0, 0, 0, 0} },
    {OPER_JLT, {0, 0, 0, 0} },
    {OPER_JGE, {0, 0, 0, 0} },
    {OPER_JLE, {0, 0, 0, 0} },

    {OPER_NOP, {0, 0, 0, 0} },               /* [NOP] */

    {OPER_LABEL, {0, 0, 0, 0} },             /* [LABEL] 
					      * DEST: [TAGNAME:VAR]
					      * <PSEUDO INSTRUCTION>
					      */

    {OPER_PHI_MOV, {1, 0, 0, 0} },           /* [PHI_MOV] 
					      * DEST: [GLOBL:VAR]
					      * OPS1: [LOCAL:VAR]
					      * <PSEUDO INSTRUCTION>
					      * MOVE, MIGHT BE MULTIPLE ASSIGNMENT TO [DEST]
					      * (other instruction's target is also [DEST])
					      */
    // {OPER_PHI_EXIT, {0, 0, 0, 0} },           /* [PHI_EXIT] */


    {OPER_MOVEQ,  {1, 0, 0, 0} },            // [MOVcc] 
    {OPER_MOVNE,  {1, 0, 0, 0} },            // DEST: [TARGET:VAR]
    {OPER_MOVGT,  {1, 0, 0, 0} },            // OPS1: [NAME:VAR/IMM]
    {OPER_MOVLT,  {1, 0, 0, 0} },            // OPS2: [SUCCESS:IMM]
    {OPER_MOVGE,  {1, 0, 0, 0} },            // OPS3: [FAIL:IMM]
    {OPER_MOVLE,  {1, 0, 0, 0} },

    {OPER_ADD,    {1, 0, 0, 0} },            // [Arith]
    {OPER_ADDLT,    {1, 0, 0, 0} },          
    {OPER_SUB,    {1, 0, 0, 0} },            // DEST: [RESULT:VAR]
    {OPER_MUL,    {1, 0, 0, 0} },            // OPS1: [OPERAND1:VAR]
    {OPER_DIV,    {1, 0, 0, 0} },            
    {OPER_SHL,    {1, 0, 0, 0} },
    {OPER_SHR,    {1, 0, 0, 0} },
    {OPER_SHRGE,  {1, 0, 0, 0} },
    {OPER_SHRLT,  {1, 0, 0, 0} },
    {OPER_SAR,    {1, 0, 0, 0} },
    {OPER_SARGE,    {1, 0, 0, 0} },
    {OPER_SARLT,    {1, 0, 0, 0} },
    {OPER_ADD_OFFSET,    {1, 0, 0, 0} },         
    {OPER_SET_ARG, {0, 0, 0, 0} },           // [SET Argument for Function]
                                             // DEST: [Argument Order Number:IMM]
                                             // OPS1: [Argument:VAR/IMM]

    {OPER_CALL,  {1, 0, 0, 0} },             // [CALL]
                                             // DEST: [Retn Value Store/VAR]
                                             // OPS1: [Calling Target/SYMBOL]
    {OPER_CALL_NRET,  {0, 0, 0, 0} },        // [CALL without Returning]
    {OPER_LOAD_ARG, {1, 0, 0, 0} },          // [LOAD Argument for Function]
                                             // DEST: [Load Target:VAR]
                                             // OPS1: [Argument Order Number:IMM]

    
    {OPER_SPACE, {0, 0, 0, 0} },             // [SPACE]
                                             // DEST: [Reserved Count]
                                             // PSEUDO INSTRUCTION
    {OPER_WORD, {0, 0, 0, 0} },              // [WORD]
                                             // DEST: [Value]
                                             // PSEUDO INSTRUCTION
    {OPER_BLK_SEP, {0, 0, 0, 0} },
  };

static std::set<OPERATION_TYPE> UNCONDITION_BRANCH_OPERATIONS =
  {
    OPER_RET,
    OPER_RETN,
    
    OPER_JMP,
  };
static std::set<OPERATION_TYPE> CONDITION_BRANCH_OPERATIONS =
  {
    OPER_JEQ,
    OPER_JNE,
    OPER_JGT,
    OPER_JLT,
    OPER_JGE,
    OPER_JLE,
  };

class basic_block_t
{
private:
  int __has_ts;
public:
  int id;
  std::list<IR_t*>::iterator begin;
  std::list<IR_t*>::iterator end;
  std::vector<basic_block_t*> out_edges;
  std::vector<basic_block_t*> in_edges;
  
  // std::set<std::string> require_variables;
  // std::set<std::string> provide_variables;
  // std::set<std::string> liveIn;
  std::set<std::string> UEVar;
  std::set<std::string> VarKill;

  std::set<std::string> liveOut;
  bool compute_liveout();
  // Live Variable DataFlow Analyze,
  // Reference: [Engineering a Compiler, K. Cooper, P.447]

  std::set<std::string> RegUEVar;
  std::set<std::string> RegVarKill;

  std::set<std::string> RegliveOut;
  std::set<std::string> RegliveIn;
  bool compute_Regliveout();
  void Finalize_RegliveIn();

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
  std::map<basic_block_t*, std::set<std::string>> Earliest;
  std::map<basic_block_t*, std::set<std::string>> Later;
  std::set<std::string> LaterIn;
  void Finalize_Earliest(const std::set<std::string> all_exprs);
  bool Compute_LaterIn(const std::set<std::string> all_exprs);
  void Finalize_Later(const std::set<std::string> all_exprs);
  // Insert and Delete
  std::map<basic_block_t*, std::set<std::string>> Insert;
  std::set<std::string> Delete;
  void Finalize_Insert(const std::set<std::string> all_exprs);
  void Finalize_Delete(const std::set<std::string> all_exprs);
  void print_Insert(std::ostream& out);
  void print_Delete(std::ostream& out);
  //////////////////////////////////////////
  std::set<std::string> local_eliminated_variables;
  std::map< std::string, std::list<IR_t*>::iterator > last_reference;
  // Record the variables that can be eliminated in current block,
  // we can eliminate them, release registers for other operations


  bool has_timestamp();
  // to find if this block is alread timestamped
  int assign_timestamp(int cur_time);
  // Assign timestamp for this block
  
  basic_block_t(std::list<IR_t*>::iterator begin,
		std::list<IR_t*>::iterator end);
  void print(std::ostream& out);
  static void AddEdge(basic_block_t* from,
		      basic_block_t* to);

  void get_register_dependency();
  void get_postalloc_reg_dependency();

};


class asm_context_t
{
public:
  void make_ASM(std::list<IR_t*>::iterator begin,
		std::list<IR_t*>::iterator end,
		std::list<instr_t*>& InstrList,
		bool is_fake = false);



  void make_func_ASM(std::list<IR_t*>::iterator begin,
		     std::list<IR_t*>::iterator end,
		     std::list<instr_t*>& InstrList,
		     bool is_fake);


  void oper_output(IROper_t& oper, std::list<instr_t*>& InstrList);
  void label_output(IRLabel_t& lab, std::list<instr_t*>& InstrList);
  void data_output(IRData_t& dat, std::list<instr_t*>& InstrList);
  void note_output(IRNote_t& note, std::list<instr_t*>& InstrList);
  void _pretty_print(std::list<instr_t*>& InstrList);

};


class register_ctx_t
// Register context
{
  
public:

  const int max_regs = 11;
  std::map<std::string, int> loadarg_target;
  // Optimize

  std::map<std::string, int> latest;
  
  std::map<std::string, basic_block_t*> var_to_blk;
  
  std::multimap<int, std::pair<std::string,
			       std::pair<std::list<IR_t*>::iterator,
					 basic_block_t*>
			       > > time_to_var;
  std::map<basic_block_t*, std::set<std::string> > post_liveOut;

  std::set<std::string> has_function_call;
  bool r14_used;
  // If there is a function call in this variable's LiveRange,
  // we shall not use r0-r3(volatile)

  std::map<std::string, int> max_regcnt;
  // If there is a instruction like
  // [SET_ARG 2 <- u]
  // [SET_ARG 1 <- v] # The last reference of [v]
  // [SET_ARG 0 <- w]
  // [CALL    f     ]
  // Assign [u -> r2] will been crashed by [ 2 <- u ]
  // But Assign [u -> r1], or [u -> r0] is permitted.

  // Active Registers
  std::map<int, std::string> active_reg_to_name;
  std::map<std::string, int> active_name_to_reg;
  // Active Memories
  std::map<int, std::string> active_mem_to_name;
  std::map<std::string, int> active_name_to_mem;
  std::set<int> mem_pool;
  // Available Registers
  std::vector<bool> __availablility;
  
  // Touched Registers, for #4-#10, we need to guard these variables
  std::vector<bool> __touched;
  std::map<int, int> guard_map;
  
  // Spilled Element count
  // used for stack allocation
 /*
  *               ┌────────────┐
         Argument:│ @5         │
                  │ @4         │
                  ├────────────┤
       LR Protect:│ #LR Reg    │
                  ├────────────┤
    Non-vol guard:│ #7         │
                  │ #4         │
                  ├────────────┤
        Variables:│ ...        │
                  │ ^1         │
                  │ ^0         │
                  ├────────────┤
     Function Arg:│ [Reserved] │
                  │ [Reserved] │
           SP ->  │ [Reserved] │
                  └────────────┘
  */  

  int stack_lr_size;
  // LR Register

  int stack_guard_size;
  // protect non-volatile registers

  int stack_var_size;
  // Local Variables & Arrays

  int stack_arg_size;
  // SET_ARG & func call

  bool r11_guarded;
  // r11 may not be guarded.
  
  // Allocation Table
  // '#k' denotes k-th register
  // '^k' denotes k-th stack-offset
  // for arrays, record their [stack location]'s location
  std::map<std::string, std::string> var_alloc_table;

  // record array's [name] -> [stack location]
  // std::map<std::string, std::string> local_array_mem_table;
  // At block boundary, check if there is any variable
  // that can be dropped.
  // Input: pointer to [NEW] block
  void expire_end_of_blk(basic_block_t* blkp);
  
  void expire_old_intervals(int cur_time, basic_block_t* blkp);

  // No-modify
  bool test_availability(int reg_idx);
  // test [reg_idx]'s availability
  
  int find_free_reg(int start, std::string var_name);
  // find an available register from [start] (inclusive)
  // [var_name] is for LIKELY finding
  
  // Modify
  void occupy_reg(int reg_idx, std::string var_name);
  // mark reg_idx as occupied
  
  void occupy_mem(std::string var_name);
  // directly spill [var_name]
  
  // Spill
  std::string find_spill_var(int start);
  // Find a var to spill, from [start] (inclusive)
  
  void spill_var(std::string name);
  // spill a variable in register to memory

  register_ctx_t();

};


//////////////////////////////////////////////////////////
////////////A S S E M B L Y///////////////////////////////
////////////////////////G E N E R A T I N G///////////////
//////////////////////////////////////////////////////////

class AsmGenerator
{
public:

  static std::string Assure_Reg(const operand_t& source_oper,
				register_ctx_t& regctx,
				std::list<instr_t*>& InstrList,
				bool& r12_occupied);
  // Assured [source_oper] is saved in a register
  // if [source_oper] is already a reg, return itself.
  // else, return a volatile register containing its value
  
  
  static void Load_Reg(std::string target_reg,
		       const operand_t& source_oper,
		       register_ctx_t& regctx,
		       std::list<instr_t*>& InstrList,
		       std::string condition_string = "");
  // Load [source_oper] (might be IMM or REG) to [target_reg] (REG)
  // [Target reg] must be a register type
  // [source_oper] might be
  // Renamed:
  //   | 1. Register     :    Load Register  (unconditional)
  //   | 2. Mem Reference:    Load local (spilled) Var's value
  // Not-Renamed:
  //   | 3. Immediate    :    Load Immediate (consider 16-bits/32-bits)
  //   | 4. Globl Array  :    Array's literal name
  //   | 5. Globl Var    :    Load Var's value
  //   | 6. Function Arg :    Load argument's real value 


  
  static void Store_Stack(const operand_t& source_oper,
			  int offset,
			  register_ctx_t& regctx,
			  std::list<instr_t*>& InstrList,
			  std::string condition_string = "");
  // Store [source_oper] to stack offset [SP+#offset]
  // Might Destroy [r11] as Address Register
  //  - If [source_oper] is Register, it can be assured that
  //    [source_oper] won't be destroyed for [r0 - r12]
  //  - If [source_oper] is a memory reference,
  //    [r12] WILL be destroyed, [r11] MIGHT be destroyed
  //  - If [source_oper] is Immediate
  //    [r12] WILL be destroyed, [r11] MIGHT be destroyed
  
  // [source_oper] might be
  // Renamed:
  //   | 1. Register     :    Store Register into stack
  //   | 2. Mem Reference:    Memory <- Memory, too bad
  // Not-Renamed:
  //   | 3. Immediate    :    Store Immediate
  //   | 4. Globl Array  :    Array's literal name (address)
  //   | 5. Globl Var    :    Var's value
  //   | 6. Function Arg :    Store argument's real value
  
  static void Store_Globl_Var(const operand_t& source_oper,
			      std::string var_name,
			      register_ctx_t& regctx,
			      std::list<instr_t*>& InstrList,
			      std::string condition_string = "");
  // Store [source_oper] into a global variable (not array)
  // [source_oper] might be
  // Renamed:
  //   | 1. Register     :    Store Register into stack
  //   | 2. Mem Reference:    Memory <- Memory, too bad
  // Not-Renamed:
  //   | 3. Immediate    :    Store Immediate
  //   | 4. Globl Array  :    Array's literal name (address)
  //   | 5. Globl Var    :    Var's value

  static void Store_Register_Offset(const operand_t& source_oper,
				    std::string offset,
				    int imm_offset,
				    register_ctx_t& regctx,
				    std::list<instr_t*>& InstrList);
  // Store [source_oper] into an address pointed by [offset]
  // [source_oper] might be
  // Renamed:
  //   | 1. Register     :    Store Register into stack
  //   | 2. Mem Reference:    Memory <- Memory, too bad
  // Not-Renamed:
  //   | 3. Immediate    :    Store Immediate
  //   | 4. Globl Array  :    Array's literal name (address)
  //   | 5. Globl Var    :    Var's value

  static void Store_Register_RegOffset(const operand_t& source_oper,
				       std::string offset,
				       std::string reg_offset,
				       register_ctx_t& regctx,
				       std::list<instr_t*>& InstrList);
  
  static void Load_Memory_Offset(const operand_t& target_oper,
				 std::string offset,
				 int imm_offset,
				 register_ctx_t& regctx,
				 std::list<instr_t*>& InstrList);


  static bool check_fit_12(int value);
  // fit 12-bits immediate
  
  static bool check_fit_5(int value);
  // fit 5-bits immediate


  static bool check_fit_mem_offset(int value);
  // fit 12-bits memory offset


private:
  static void Load_Reg_Immediate(std::string target_reg,
				 int value,
				 std::list<instr_t*>& InstrList,
				 std::string condition_string = "");
  // Load a Immediate into register
  
  static void _store_register_to_stack(std::string reg_name,
				       int offset,
				       register_ctx_t& regctx,
				       std::list<instr_t*>& InstrList,
				       std::string condition_string);
  static bool _check_rotate_12(int value);
  // Check if [value] can be represented as [ROTATE4 + IMM8]

  static unsigned int _rotatel(const unsigned int value, int shift);
  // Rotate left
  static std::string rename_operand(operand_t op);
    
};
std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Killed_Position(IROper_t& irop);
std::set<std::string> Get_Killed_vars(IROper_t& irop);
std::pair<bool, std::vector<std::pair<std::string, std::string*>> > Get_Referenced_Position(IROper_t& irop);
std::set<std::string> Get_Referenced_vars(IROper_t& irop);
#endif
