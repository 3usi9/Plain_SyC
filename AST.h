#ifndef _AST_H
#define _AST_H
#include<vector>
#include<typeinfo>
#include<iostream>
#include "debug.h"
#include "common.h"
#include "IR.h"
#include "context.h"
/************************************/
// Level 0, Basic Type
extern context_t global_ctx;
typedef int btype_t;

struct cond_result_t
// denote the jump condition to target
{
  OPERATION_TYPE then_op;  // target is [then], when to jump
  OPERATION_TYPE else_op;  // target is [else], when to jump
};

/************************************/
// Level 1, ASTNode
class ASTNode_t
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  // Not implemented yet.
  
public:
  ASTNode_t();
  virtual ~ASTNode_t() = 0;
  // destructor, [pure]

  virtual
  void printNode(int  levelspec = 0,
		 bool isEnd = false,
		 std::ostream& out = std::cerr) = 0;
  // print current node's Type and semantic value, [pure]

  static void __print_Indentation(int   levelspec,
				  bool  isEnd = false,
				  std::ostream& out = std::cerr);

  static void __print_message(const char* msg = "", std::ostream& out = std::cerr, const std::string note = "");


  static void printToken(token_t token,
			 int levelspec = 0,
			 bool isEnd    = false,
			 std::ostream& out = std::cerr,
			 bool RequireIdent = false);
  // print Token info

};

/************************************/
// Level 2, Inherited from ASTNode_t
class expr_t : public ASTNode_t
/* ASTNode_t -> { expr_t } 
 *           -> { _cond_expr_t,
 *                _binary_expr_t,
 *                _unary_expr_t,
 *                _func_call_t,
 *                _number_t,
 *                arr_initvallist_t,
 *                arr_dimlist_t,
 *                func_real_arglist_t,
 *                func_formal_arglist_t,
 *                func_formal_arg_t,
 *                stmt_t,
 *                ident_t
 *              }
 *               */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  
  
  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);

  virtual
  std::pair<cond_result_t, IRList_t*> eval_cond(context_t& ctx);
  // Return the jump OpCode to fill the target address
  // Can be optimized

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  CONST_STATUS_T const_valid;

  // virtual
  // bool is_constexpr();
  // // Check if a expression is const
  // Currently, we don't need semantic analysis part
  
public: // Syntax Analysis
  bool const_required;

public: // Public constructor
  expr_t();
public:
  

  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class expr_t : public ASTNode_t

/************************************/
// Level 3, Inherited from ASTNode_t -> { expr_t }

class ident_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { ident_t }
 *           -> { arr_ident_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::string& name;
  ident_t(std::string& name);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class ident_t : public expr_t


class _cond_expr_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { _cond_expr_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _cond_expr_t : public expr_t

class _binary_expr_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { _binary_expr_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);

  virtual
  std::pair<cond_result_t, IRList_t*> eval_cond(context_t& ctx);
  // Return the jump OpCode to fill the target address
  // Can be optimized
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t &ops1, &ops2;
  token_t oper;
  _binary_expr_t(expr_t& ops1,
		 token_t oper,
		 expr_t& ops2);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _binary_expr_t : public expr_t

class _unary_expr_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { _unary_expr_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  token_t oper;
  expr_t &opd;
  _unary_expr_t(token_t oper,
		expr_t &opd);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _unary_expr_t : public expr_t


class func_real_arglist_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { func_real_arglist_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::vector<expr_t*> body;
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class func_real_arglist_t : public expr_t

class _func_call_t : public expr_t
/* ASTNode_t -> expr_t
  *           -> { _func_call_t }
  */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  ident_t& name;
  func_real_arglist_t& real_args;
  _func_call_t(ident_t& name,
	       func_real_arglist_t& real_args);
public:

  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _func_call_t : public expr_t

class _number_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { _number_t }
 * Literal Numbers
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Debug Notes
  RADIX_T radix;
public: // Syntax Analysis
  btype_t semantic_value;
  // this class is simple, do not use constructor
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);

  _number_t();
  _number_t(btype_t value);
}; // class _number_t : public expr_t

class arr_initvallist_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { arr_initvallist_t }
 */
{
public: // Generating IR
  int lower_level; // used to eval braces
  // represent how many braces inside this list
  
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  std::vector<expr_t*> body;
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class arr_initvallist_t : public expr_t

class arr_dimlist_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { arr_dimlist_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::vector<expr_t*> body;
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class arr_dimlist_t : public expr_t


class func_formal_arg_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { func_formal_arg_t }
 */
{

public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);

public: // Semantic Analysis
  virtual
  void check_semantic();
  

public: // Syntax Analysis
  token_t  arg_type;
  ident_t& arg_name;
  
  func_formal_arg_t(ident_t& name);
  /* for { FuncFormalArgSingle } */
  

public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class func_formal_arg_t : public expr_t

class func_formal_arglist_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { func_formal_arglist_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::vector<func_formal_arg_t*> body;
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class func_formal_arglist_t : public expr_t

class stmt_t : public expr_t
/* ASTNode_t -> expr_t
 *           -> { stmt_t }
 *           -> { _assign_stmt_t,
 *                _eval_stmt_t,
 *                _block_stmt_t,
 *                _if_noelse_stmt_t,
 *                _if_withelse_stmt_t,
 *                _while_stmt_t,
 *                _break_stmt_t,
 *                _continue_stmt_t,
 *                _return_novalue_stmt_t,
 *                _return_withvalue_stmt_t,
 *                _empty_stmt_t,
 *                block_t,
 *                decl_t
 *               }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class stmt_t : public expr_t


/************************************/
// Level 4, Inherited from ASTNode_t -> expr_t -> ident_t

class arr_ident_t : public ident_t
/* ASTNode_t -> expr_t -> ident_t
 *                     -> { arr_ident_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  virtual
  int eval_compile(context_t& ctx, int level);

  virtual
  std::pair<operand_t, IRList_t*> eval_runtime(context_t& ctx);

  // IRList_t* elem_assign_runtime(const operand_t& oper);
  // treat this ident as LVal (but not declaration).
  // Action:  this[dims] <- oper
  // mainly calculate the address of this[dims]
  // [deprecated];
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  ident_t& name;
  arr_dimlist_t& dimlist;
  arr_ident_t(ident_t& name,
  	      arr_dimlist_t& dimlist);
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);

}; // class arr_ident_t : public ident_t

/************************************/
// Level 4, Inherited from ASTNode_t -> expr_t -> stmt_t

class block_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { block_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  IR_t* make_IR(bool create_scope, context_t& ctx);
  
  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::vector<stmt_t*> body;
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class block_t : public stmt_t


class _assign_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _assign_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  ident_t& lhs;
  expr_t&  rhs;
  _assign_stmt_t(ident_t& lhs,
		 expr_t&  rhs);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _assign_stmt_t : public stmt_t

class _eval_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _eval_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t& expr;
  _eval_stmt_t(expr_t& expr);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _eval_stmt_t : public stmt_t

class _block_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _block_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax
  block_t& block_body;
  _block_stmt_t(block_t& block_body);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _block_stmt_t : public stmt_t

class _if_noelse_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _if_noelse_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t& cond_expr;
  stmt_t& then_stmt;
  _if_noelse_stmt_t(expr_t& cond_expr,
		    stmt_t& then_stmt);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _if_noelse_stmt_t : public stmt_t

class _if_withelse_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _if_withelse_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t& cond_expr;
  stmt_t& then_stmt;
  stmt_t& else_stmt;
  _if_withelse_stmt_t(expr_t& cond_expr,
		      stmt_t& then_stmt,
		      stmt_t& else_stmt);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _if_withelse_stmt_t : public stmt_t

class _while_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _while_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t& cond_expr;
  stmt_t& body_stmt;
  _while_stmt_t(expr_t& cond_expr,
		stmt_t& body_stmt);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _while_stmt_t : public stmt_t

class _break_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _break_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // No Syntax Members needed
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _break_stmt_t : public stmt_t

class _continue_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _continue_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // No Syntax Members needed
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _continue_stmt_t : public stmt_t

class _return_novalue_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _return_novalue_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  // nothing here...
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _return_novalue_stmt_t : public stmt_t

class _return_withvalue_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _return_withvalue_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  expr_t& retn_value;
  _return_withvalue_stmt_t(expr_t& retn_value);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _return_withvalue_stmt_t : public stmt_t

class _empty_stmt_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { _empty_stmt_t }
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _empty_stmt_t : public stmt_t


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////// func_def_t class ////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/************************************/
// Level 2, Directly inherited from ASTNode

class func_def_t : public ASTNode_t
// ASTNode_t -> { func_def_t } -> {}
/* for { FuncDef } */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  token_t  retn_type;
  ident_t& name;
  func_formal_arglist_t& formal_args;
  block_t& body;
  func_def_t(token_t retn_type,
	     ident_t& name,
	     func_formal_arglist_t& formal_args,
	     block_t& body);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class func_def_t : public ASTNode_t



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////// var_def_t class /////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/************************************/
// Level 2, Directly inherited from ASTNode


class var_def_t : public ASTNode_t
/* ASTNode_t -> { var_def_t } 
 *           -> { _var_def_one_noinit_t,
 *                _var_def_one_withinit_t,
 *                _var_def_array_noinit_t,
 *                _var_def_array_withinit_t
 *              }
 */
/* 
 * for { VarDef,
 *       VarDefOne,
 *       VarDefArray,
 *       ConstVarDef,
 *       ConstVarDefOne,
 *       ConstVarDefArray
 *     }
 */
/**** Pure Virtual Class ****/
/* Single variable declaration */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class var_def_t : public ASTNode_t

/************************************/
// Level 2, Inherited from ASTNode_t -> { var_def_t }

class _var_def_one_noinit_t : public var_def_t
/* ASTNode_t ->  var_def_t 
 *               -> { _var_def_one_noinit_t } 
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  ident_t& name;
  _var_def_one_noinit_t(ident_t& name
			);
public:

  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _var_def_one_noinit_t : public var_def_t

class _var_def_one_withinit_t : public var_def_t
/* ASTNode_t -> var_def_t 
 *              -> { _var_def_one_withinit_t } 
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  ident_t& name;
  expr_t&  value;
  bool is_const;
  // propagated from above
  _var_def_one_withinit_t(ident_t& name,
			  expr_t&  value);

public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _var_def_one_withinit_t : public var_def_t

class _var_def_array_noinit_t : public var_def_t
/* ASTNode_t -> var_def_t 
 *              -> { _var_def_array_noinit_t } 
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  arr_ident_t& name;
  _var_def_array_noinit_t(arr_ident_t& name);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _var_def_array_noinit_t : public var_def_t

class _var_def_array_withinit_t : public var_def_t
/* ASTNode_t -> var_def_t 
 *              -> { _var_def_array_withinit_t } 
 */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  

  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public:
  arr_ident_t& name;
  arr_initvallist_t& value;
  bool is_const;
  // propagated from above
  _var_def_array_withinit_t(arr_ident_t& name,
			    arr_initvallist_t& value);
public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);


  
}; // class _var_def_array_withinit_t : public var_def_t



// decl_t
class decl_t : public stmt_t
/* ASTNode_t -> expr_t -> stmt_t -> { decl_t }
 */
/* for { Decl, 
 *       VarDecl, 
 *       VarDefClosure, 
 *       ConstVarDecl,
 *       ConstVarDefClosure
 */
/* Declaration Lists */
 {
 public: // Generating IR
   virtual IR_t* make_IR(context_t& ctx);
   void _before_make_IR();
   
   

   
 public: // Semantic Analysis
   virtual
   void check_semantic();
   
 public: // Syntax Analysis
   token_t type;
   std::vector<var_def_t*> body;
   bool is_const;
 public:
   
   virtual
   void printNode(int  levelspec = 0,
		  bool  isEnd     = false,
		  std::ostream& out = std::cerr);
   
}; // class decl_t : public stmt_t


/************************************/
// Level 2, Directly inherited from ASTNode

class root_t : public ASTNode_t
/* ASTNode_t -> { root_t } -> {} */
/* for { CompUnit } */
{
public: // Generating IR
  virtual IR_t* make_IR(context_t& ctx);
  void _before_make_IR();
  
  
  

  
public: // Semantic Analysis
  virtual
  void check_semantic();
  
public: // Syntax Analysis
  std::vector<ASTNode_t*> body;

public:
  
  virtual
  void printNode(int  levelspec = 0,
		 bool  isEnd     = false,
		 std::ostream& out = std::cerr);

  // print current node's Syntax Tree, [pure]
}; // class root_t : public ASTNode_t


#endif
