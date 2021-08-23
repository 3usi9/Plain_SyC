#include "AST.h"
#include "common.h"
#include <assert.h>


////////////////////////
// func_def_t
func_def_t::func_def_t(token_t retn_type,
		       ident_t& name,
		       func_formal_arglist_t& formal_args,
		       block_t& body) :
  retn_type(retn_type),
  name(name),
  formal_args(formal_args),
  body(body)
{
}

////////////////////////
// ident_t

ident_t::ident_t(std::string& name) :
  name(name)
{
}


////////////////////////
// func_formal_arg_t

func_formal_arg_t::func_formal_arg_t(ident_t& name) :
  arg_name(name)
{
}

////////////////////////
// _var_def_one_noinit_t

_var_def_one_noinit_t::_var_def_one_noinit_t(ident_t& name) :
  name(name)
{
}

arr_ident_t::arr_ident_t(ident_t& name,
			 arr_dimlist_t& dimlist) :
  name(name), dimlist(dimlist), ident_t(name)
{
}

////////////////////////
// _eval_stmt_t
_eval_stmt_t::_eval_stmt_t(expr_t& expr) :
  expr(expr)
{
}
//////////////////////
// _var_def_one_withinit_t
_var_def_one_withinit_t::_var_def_one_withinit_t(ident_t& name,
						 expr_t&  value) :
  name(name), value(value)
{
}

//////////////////////
// _binary_expr_t
_binary_expr_t::_binary_expr_t(expr_t& ops1,
			       token_t oper,
			       expr_t& ops2) :
  ops1(ops1), oper(oper), ops2(ops2)
{
}

//////////////////////
// _unary_expr_t
_unary_expr_t::_unary_expr_t(token_t oper,
			     expr_t &opd) :
  oper(oper), opd(opd)
{
}

//////////////////////
// _func_call_t
_func_call_t::_func_call_t(ident_t& name,
			   func_real_arglist_t& real_args) :
  name(name), real_args(real_args)
{
}

//////////////////////
// expr_t
expr_t::expr_t() :
  const_valid(CONST_UNKNOWN),
  const_required(false)
{
}


//////////////////////
// _var_def_array_noinit_t
_var_def_array_noinit_t::_var_def_array_noinit_t(arr_ident_t& name) :
  name(name)
{
}

//////////////////////
// _var_def_array_withinit_t
_var_def_array_withinit_t::_var_def_array_withinit_t(arr_ident_t& name,
						     arr_initvallist_t& value) :
  name(name), value(value)
{
}

//////////////////////
// _assign_stmt_t
_assign_stmt_t::_assign_stmt_t(ident_t& lhs,
			       expr_t&  rhs) :
  lhs(lhs), rhs(rhs)
{
}


//////////////////////
// _if_noelse_stmt
_if_noelse_stmt_t::_if_noelse_stmt_t(expr_t& cond_expr,
				     stmt_t& then_stmt) :
  cond_expr(cond_expr), then_stmt(then_stmt)
{
}

//////////////////////
// _if_withelse_stmt
_if_withelse_stmt_t::_if_withelse_stmt_t(expr_t& cond_expr,
					 stmt_t& then_stmt,
					 stmt_t& else_stmt) :
  cond_expr(cond_expr), then_stmt(then_stmt), else_stmt(else_stmt)
{
}

//////////////////////
// _block_stmt_t
_block_stmt_t::_block_stmt_t(block_t& block_body) :
  block_body(block_body)
{
}

//////////////////////
// _while_stmt_t
_while_stmt_t::_while_stmt_t(expr_t& cond_expr,
			     stmt_t& body_stmt) :
  cond_expr(cond_expr), body_stmt(body_stmt)
{
}

//////////////////////
// _return_withvalue_stmt_t
_return_withvalue_stmt_t::_return_withvalue_stmt_t(expr_t& retn_value) :
  retn_value(retn_value)
{
}

/////////////////////
// _number_t
_number_t::_number_t(){}
_number_t::_number_t(btype_t val) : semantic_value(val), radix(DEC)
{}

