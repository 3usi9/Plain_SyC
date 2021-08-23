#include "AST.h"
#include<stdint.h>
#include<sstream>
#include "context.h"
#include "tokens.h"
#include "IR.h"
#include "exceptions.h"
extern context_t global_ctx;
int expr_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_37);
  // should not reach here
}

std::pair<operand_t, IRList_t*> expr_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_38);
  // should not reach here
}

int _cond_expr_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_39);
}

std::pair<operand_t, IRList_t*> _cond_expr_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_40);
}

IRList_t* Multiply(const operand_t& opd, const operand_t& ops1, const operand_t& ops2, context_t& ctx)
{
  IRList_t& IRList = *(new IRList_t);
// #define MUL2SHL(OPD,OPS,IMMOper)					\
// 	if(IMMOper.type == OPERAND_IMM &&				\
// 	   IS_POWER_OF_2(IMMOper.imm))					\
// 	  {								\
// 	    if(0  <= get_low(IMMOper.imm) &&				\
// 	       32 >  get_low(IMMOper.imm)				\
// 	       )							\
// 	      {								\
// 		IRList.push_back(new IROper_t(OPER_SHL,			\
// 					      OPD,			\
// 					      OPS,			\
// 					      get_low(IMMOper.imm)	\
// 					      )				\
// 				 );					\
// 	      }								\
// 	    else							\
// 	      {								\
// 		IRList.push_back(new IROper_t(OPER_MOVE,		\
// 					      OPD,			\
// 					      0				\
// 					      )				\
// 				 );					\
// 	      }								\
// 	    return &IRList;							\
// 	  }

// 	MUL2SHL(opd, ops1, ops2);
// 	MUL2SHL(opd, ops2, ops1);
// #undef MUL2SHL	
	IRList.push_back(new IROper_t(OPER_MUL, opd, ops1, ops2));
	return &IRList;
}
IRList_t* Divide(operand_t& opd, const operand_t& ops1, const operand_t& ops2, context_t& ctx)
{
  IRList_t& IRList = *(new IRList_t);
  if(ops1.get_static_type() == STATIC_IMM &&
     ops2.get_static_type() == STATIC_IMM)
    {
      IRList.push_back(new IROper_t(OPER_MOVE, opd, ops1.imm/ops2.imm));
      return &IRList;
    }
  // if(ops1.get_static_type() != STATIC_IMM &&
  //    ops2.get_static_type() == STATIC_IMM &&
  //    IS_POWER_OF_2(ops2.imm))					
  //   {								
  //     if(0  <= get_low(ops2.imm) &&				
  // 	 32 >  get_low(ops2.imm)				
  // 	 )							
  // 	{
  // 	  int imm = ops2.imm;
  // 	  assert(ops1.get_static_type() != STATIC_IMM);
  // 	  std::string div_label = std::to_string(ctx.get_unique_id());

  // 	  IRList.push_back(new IROper_t(OPER_CMP, ops1, 0));
  // 	  // If [ops1 >= 0], result is [ops1 >> get_low(ops2)]
  // 	  // If [ops1 < 0], result is  [(ops1 + (1<<ops2 - 1)) >> ops2]
  // 	  int id = (ctx.get_unique_id());
  // 	  operand_t tmp(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
  // 	  operand_t tmp2(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
  // 	  operand_t tmp3(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
  // 	  std::string label = "NEG_DIVIDE_" + std::to_string(id);
  // 	  std::string label_exit = "DIVIDE_EXIT" + std::to_string(id);
  // 	  IRList.push_back(new IROper_t(OPER_JLT, label));

  // 	  IRList.push_back(new IROper_t(OPER_SAR,			
  // 					tmp2,			
  // 					ops1,			
  // 					get_low(imm)
  // 					));
  // 	  IRList.push_back(new IROper_t(OPER_PHI_MOV,			
  // 					opd,
  // 					tmp2
  // 					));
	  
  // 	  IRList.push_back(new IROper_t(OPER_JMP, label_exit));
  // 	  IRList.push_back(new IROper_t(OPER_LABEL,
  // 					label));
  // 	  IRList.push_back(new IROper_t(OPER_ADD,			
  // 					tmp,
  // 					ops1,			
  // 					(imm-1)
  // 					));

  // 	  IRList.push_back(new IROper_t(OPER_SAR,			
  // 					tmp3,			
  // 					tmp,
  // 					get_low(imm)
  // 					));
  // 	  IRList.push_back(new IROper_t(OPER_PHI_MOV,			
  // 					opd,
  // 					tmp3
  // 					));

  // 	  IRList.push_back(new IROper_t(OPER_LABEL,
  // 					label_exit));
  // 	  return &IRList;
		
  // 	}

  //   }
  IRList.push_back(new IROper_t(OPER_DIV, opd, ops1, ops2));
  return &IRList;
}

int _binary_expr_t::eval_compile(context_t& ctx, int level)
{
  if(level > EVAL_COMPILE_MAX_LEVEL)
    throw ex_exceed_level_limit;
  // std::cerr <<COLOR_RED "Trying to eval compile for ";
  // this->printNode(-1, 0, std::cerr);
  // std::cerr << "At level "<<level<<std::endl;
  switch(this->oper)
    {
      // Arith
    case PLUS:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) + this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case MINUS:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) - this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));      
    case MUL:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) * this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));      
    case DIV:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) / this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));      
    case MOD:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) % this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));

      // Logic
    case LAND:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) && this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case LOR:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) || this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));

      // Relation
    case EQ:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) == this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case NE:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) != this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case LT:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) < this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case GT:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) > this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case LE:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) <= this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    case GE:
      return this->ops1.eval_compile(ctx, (level==-1)?(level):(level+1)) >= this->ops2.eval_compile(ctx, (level==-1)?(level):(level+1));
    default:
      {
	std::cerr<<COLOR_YELLOW "Symbol ";
	ASTNode_t::printToken(oper);
	std::cerr<<" is not a binary expression operator.";
      }
    }
  myAssert(0, ASSERT_EXIT_CODE_41);
}

std::pair<operand_t, IRList_t*> _binary_expr_t::eval_runtime(context_t& ctx)
{

  IRList_t& IRList = *(new IRList_t);
  {

    std::stringstream ss;
    this->printNode(-1, 0, ss);
    IRList.note = "_binary_expr_t::eval_runtime at "  COLOR_GREEN + ss.str() + COLOR_GREEN;    
  }

  auto ops1_result = this->ops1.eval_runtime(ctx);
  auto ops2_result = this->ops2.eval_runtime(ctx);
  operand_t opd(LOCAL_VAR(std::to_string(ctx.get_unique_id())));  
  if(this->oper != LAND && this->oper != LOR)
    {
	{
	  try
	    {
	      std::stringstream ss;
	      this->printNode(-1,0,ss);
	      return std::make_pair(operand_t(this->eval_compile(ctx, 0)),
				    (IRList_t*) (new IRNote_t("Compile-time eval succeed for " + ss.str())));
	    }
	  catch (...)
	    {}
	}
  
      IRList.push_back(ops1_result.second);
      IRList.push_back(ops2_result.second);
    }
  
  switch(this->oper)
    {
#define CASE1(OPCASE, OPERATION, NEG_OPERATION, REVERSE_OPERATION, REVERSE_NEG_OPERATION) \
      case OPCASE:							\
	{								\
	  if(ops1_result.first.type == OPERAND_IMM)			\
	    {								\
	      IRList.push_back(new IROper_t(OPER_CMP,ops2_result.first, ops1_result.first)); \
	      IRList.push_back(new IROper_t(REVERSE_OPERATION, opd, 1)); \
	      IRList.push_back(new IROper_t(REVERSE_NEG_OPERATION, opd, 0)); \
	      break;							\
	    }								\
	  else								\
	    {								\
	      IRList.push_back(new IROper_t(OPER_CMP,ops1_result.first, ops2_result.first)); \
	      IRList.push_back(new IROper_t(OPERATION, opd, 1));	\
	      IRList.push_back(new IROper_t(NEG_OPERATION, opd, 0));	\
	      break;							\
	    }								\
	}
      
      CASE1(EQ, OPER_MOVEQ, OPER_MOVNE, OPER_MOVEQ, OPER_MOVNE);
      CASE1(NE, OPER_MOVNE, OPER_MOVEQ, OPER_MOVNE, OPER_MOVEQ);
      CASE1(LT, OPER_MOVLT, OPER_MOVGE, OPER_MOVGT, OPER_MOVLE);
      CASE1(GT, OPER_MOVGT, OPER_MOVLE, OPER_MOVLT, OPER_MOVGE);
      CASE1(LE, OPER_MOVLE, OPER_MOVGT, OPER_MOVGE, OPER_MOVLT);
      CASE1(GE, OPER_MOVGE, OPER_MOVLT, OPER_MOVLE, OPER_MOVGT);
#undef CASE1

      
      // Other operations are [PLUS, MINUS, MUL, DIV, MOD], [LAND, LOR]
      // Arithmetic: [PLUS, MINUS, MUL, DIV, MOD]
#define CASE2(OPCASE, OPERATION) \
    case OPCASE:\
      {\
	IRList.push_back(new IROper_t(OPERATION, opd, ops1_result.first, ops2_result.first));\
	break;\
      }
      CASE2(PLUS,  OPER_ADD);
      CASE2(MINUS, OPER_SUB);
      // CASE2(DIV,   OPER_DIV);
      // CASE2(MOD,   OPER_MOD);
#undef CASE2
    case DIV:
      {
	IRList.push_back(Divide(opd, ops1_result.first, ops2_result.first, ctx));
	break;
	    
      }
    case MOD:
      // Mod is Not Implemented in CPU
      {
	if(ops2_result.first.get_static_type() == STATIC_IMM)
	  {
	    operand_t divide(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
	    IRList.push_back(Divide(divide, ops1_result.first, ops2_result.first, ctx));
	    operand_t multip(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
	    IRList.push_back(Multiply(multip, divide, ops2_result.first, ctx));
	    // Eliminate previous opd
	    opd = operand_t(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
	    IRList.push_back(new IROper_t(OPER_SUB, opd, ops1_result.first, multip));
	  }
	else
	  {
	    IRList.push_back(new IROper_t(OPER_SET_ARG, 1, ops2_result.first));
	    IRList.push_back(new IROper_t(OPER_SET_ARG, 0, ops1_result.first));
	    IRList.push_back(new IROper_t(OPER_CALL, opd, std::string("__aeabi_idivmod")));
	  }
	break;
      }

    case MUL:
      {
	IRList.push_back(Multiply(opd, ops1_result.first, ops2_result.first, ctx));
	break;
      }


      // Logical: [LAND, LOR]
      // Shortcut property: if LHS fails, it should directly
      //                    branch to end position, and RHS
      //                    shouldn't be evaluated
    case LAND:
	/*               ┌─────────────────────┐
                         │ OPS1 BODY           │
                         ├─────────────────────┤
                         │ PHI_MOV [DEST], 0   │
                         │ FAIL JMP [END]      │
                         ├─────────────────────┤
                         │ OPS2 BODY           │
                         ├─────────────────────┤
                         │ PHI_MOV [DEST], res │
                         ├─────────────────────┤
          END_[label]:   │                     │
                         └─────────────────────┘
	*/
      
	{
	  std::string end_label = "END_" + std::to_string(ctx.get_unique_id());
	  IRList_t* end = new IRList_t;
	  end->push_back(new IROper_t(OPER_LABEL,end_label));
	  auto ops1 = this->ops1.eval_cond(ctx);
	  IRList.push_back(ops1.second);
	  
	  auto* ptr = new IROper_t(OPER_PHI_MOV, opd, 0);
	  ptr->phi_target = (IROper_t*) end->body.front();
	  IRList.push_back(ptr);
	  IRList.push_back(new IROper_t(ops1.first.else_op, end_label));
	  // ops1

	  auto ops2 = this->ops2.eval_runtime(ctx);
	  IRList.push_back(ops2.second);
	  auto* ptr2 = new IROper_t(OPER_PHI_MOV, opd, ops2.first);
	  ptr2->phi_target = (IROper_t*) end->body.front();
	  IRList.push_back(ptr2);
	  IRList.push_back(end);
	  break;
	}

    case LOR:
	/*               ┌─────────────────────┐
                         │ OPS1 BODY           │
                         ├─────────────────────┤
                         │ PHI_MOV [DEST], 1   │
                         │ SUCC JMP [END]      │
                         ├─────────────────────┤
                         │ OPS2 BODY           │
                         ├─────────────────────┤
                         │ PHI_MOV [DEST], res │
                         ├─────────────────────┤
          END_[label]:   │                     │
                         └─────────────────────┘
	*/
      {
	std::string end_label = "END_" + std::to_string(ctx.get_unique_id());
	IRList_t* end = new IRList_t;
	end->push_back(new IROper_t(OPER_LABEL, end_label));
	auto ops1 = this->ops1.eval_cond(ctx);
	IRList.push_back(ops1.second);
	auto* ptr = new IROper_t(OPER_PHI_MOV, opd, 1);
	ptr->phi_target = (IROper_t*) end->body.front();
	IRList.push_back(ptr);
	IRList.push_back(new IROper_t(ops1.first.then_op, end_label));

	auto ops2 =  this->ops2.eval_runtime(ctx);
	IRList.push_back(ops2.second);
	auto* ptr2 = new IROper_t(OPER_PHI_MOV, opd, ops2.first);
	ptr2->phi_target = (IROper_t*) end->body.front();
	IRList.push_back(ptr2);
	IRList.push_back(end);
	break;
      }
    default:
      {
	COMPILE_ERROR("Unknown Operator");
	break;
      }
    }      
  return std::make_pair(opd, &IRList);
}

int _unary_expr_t::eval_compile(context_t& ctx, int level)
{
  switch(this->oper)
    {
    case PLUS:
      return this->opd.eval_compile(ctx, level);
      break;
    case MINUS:
      return -this->opd.eval_compile(ctx, level);
      break;
    case LNEG:
      return !this->opd.eval_compile(ctx, level);
      break;
    default:
      myAssert(0, ASSERT_EXIT_CODE_42); // Unknown Operator
    }

}

std::pair<operand_t, IRList_t*> _unary_expr_t::eval_runtime(context_t& ctx)
{
    {
      try
	{
	  return {operand_t(this->eval_compile(ctx, 0)), NULL};
	}
      catch(...)
	{}
    }
  
  IRList_t& IRList = *(new IRList_t);

  switch(this->oper)
    {
    case PLUS:
      {
	return this->opd.eval_runtime(ctx);
	break;
      }
    case MINUS:
      {
	auto eval_result = this->opd.eval_runtime(ctx);
	IRList.push_back(eval_result.second);
	operand_t retn_val = (LOCAL_VAR(std::to_string(ctx.get_unique_id())));
	IRList.push_back(new IROper_t(OPER_SUB, retn_val, 0, eval_result.first));
	return {retn_val, &IRList};
	break;
      }
    case LNEG:
      {
	auto eval_result = this->opd.eval_runtime(ctx);
	IRList.push_back(eval_result.second);
	operand_t retn_val = (LOCAL_VAR(std::to_string(ctx.get_unique_id())));
	IRList.push_back(new IROper_t(OPER_CMP, eval_result.first, 0));
	IRList.push_back(new IROper_t(OPER_MOVEQ, retn_val, 1));
	IRList.push_back(new IROper_t(OPER_MOVNE, retn_val, 0));
	return {retn_val, &IRList};
	break;
      }
    default:
      myAssert(0, ASSERT_EXIT_CODE_43); // Unknown Operator
      
    }
}

int _func_call_t::eval_compile(context_t& ctx, int level)
{
  throw *new std::exception;
}

std::pair<operand_t, IRList_t*> _func_call_t::eval_runtime(context_t& ctx)
{
  std::vector<operand_t> args;
  IRList_t& IRList = *(new IRList_t);
  {
    std::stringstream ss;
    ss << "Function call "<<COLOR_PURPLE<<"[";
    this->printNode(-1,0,ss);
    ss<<COLOR_PURPLE<<"]";
    IRList.note = ss.str();
  }
  // find function body in var table
  sym_t& fsym = ctx.var_table_find_globl(this->name.name);
  if(!fsym.is_func())
    {
      COMPILE_ERROR((this->name.name + " is not a function").c_str());
    }

  if(fsym.arglist.size() != this->real_args.body.size())
    {
      std::string s = this->name.name + " expect " + std::to_string(fsym.arglist.size())
	+" args, but got "+std::to_string(this->real_args.body.size())+" args";
      COMPILE_ERROR(s.c_str());
    }
  
  operand_t retn_val = (LOCAL_VAR(std::to_string(ctx.get_unique_id())));
  IRList.push_back(new IRNote_t("Argument Preparation Stage"));
  for(int i = 0; i < this->real_args.body.size(); i++)
    // prepare arguments
    {
      try
	{
	  // printf("Eval Runtime at :");
	  // this->real_args.body[i]->printNode(-1, 0, std::cerr);
	  // printf("\n");
	  auto pr = this->real_args.body[i]->eval_runtime(ctx);
	  IRList.push_back(new IRNote_t("Argument " COLOR_CYAN +
					std::to_string(i)+COLOR_GREEN
					" is an expression"));

	  // Check argument type correspondence
	  if(fsym.arglist[i]->is_array())
	    {
	      COMPILE_ERROR("Argument type does not agree");
	    }
	  
	  args.push_back(pr.first);
	  IRList.push_back(pr.second);

	}
      catch(std::exception &ex)
	// Check if we're passing an array to [ex]
	{
	  if(typeid(ex) == typeid(ex_array_as_single_t))
	    // Passing an array as identifier
	    {
	  
	      if(typeid(*this->real_args.body[i]) != typeid(ident_t))
		// check for atomic expression
		{
		  COMPILE_ERROR(ex.what());
		}

	      // check for var table
	      sym_t& sym = ctx.var_table_find(((ident_t*) this->real_args.body[i])->name);
	      if(!sym.is_array())
		{
		  COMPILE_ERROR(ex.what());
		}

	      // Check argument type correspondence
	      if(!fsym.arglist[i]->is_array())
		{
		  COMPILE_ERROR("Argument type does not agree");
		}
	      // Check array dimension
	      myAssert(fsym.arglist[i]->dimlist.size() >= 1, ASSERT_EXIT_CODE_44);
	      if(fsym.arglist[i]->dimlist.size() !=
		 sym.dimlist.size())
		{
		  COMPILE_ERROR("Array dimension size does not agree");
		}
	  
	      for(int j = 1;
		  j < fsym.arglist[i]->dimlist.size();
		  j++)
		{
		  if(sym.dimlist[j] != fsym.arglist[i]->dimlist[j])
		    {
		      COMPILE_ERROR("Array size count does not agree");
		    }
		}
	      IRList.push_back(new IRNote_t("Argument " COLOR_CYAN+
					    std::to_string(i)+ COLOR_GREEN
					    " is an array, "
					    "we shall push its address"));
	      args.push_back(operand_t(sym.name));
	    }
	  else if(typeid(ex) == typeid(ex_arr_dim_mismatch))
	    // Passing an array but dimension mismatch
	    {
	      if(typeid(*this->real_args.body[i]) != typeid(arr_ident_t))
		// check for atomic expression
		{
		  COMPILE_ERROR("Argument type does not agree");
		}
	      
	      // check for var table
	      sym_t& sym = ctx.var_table_find(((ident_t*) this->real_args.body[i])->name);
	      if(!sym.is_array())
		{
		  COMPILE_ERROR(ex.what());
		}
	      
	      // Check argument type correspondence
	      if(!fsym.arglist[i]->is_array())
		{
		  COMPILE_ERROR("Argument type does not agree");
		}

	      auto& caller_dimlist = ((arr_ident_t*)this->real_args.body[i])->dimlist;
	      int skip_dimensions = caller_dimlist.body.size();
	      
	      // Check array dimension
	      myAssert(fsym.arglist[i]->dimlist.size() >= 1, ASSERT_EXIT_CODE_45);
	      if(fsym.arglist[i]->dimlist.size() !=
		 sym.dimlist.size() - skip_dimensions
		 )
		{
		  COMPILE_ERROR("Array dimension size does not agree");
		}

	      // Equality: fsym.arglist[i]->dimlist.size() + skip_dimensions ==== sym.dimlist.size()
	      // Check and fix dimension
	      {
		for(int j = 1;
		    j < fsym.arglist[i]->dimlist.size();
		    j++)
		  {
		    if(sym.dimlist[j + skip_dimensions] != fsym.arglist[i]->dimlist[j])
		      {
			COMPILE_ERROR("Array size count does not agree");
		      }
		  }
		IRList.push_back(new IRNote_t("Argument " COLOR_CYAN+
					      std::to_string(i)+ COLOR_GREEN
					      " is a part of array, "
					      "we shall push its address"));


		// find the base size
		int position = sym.dimlist.size() - 1;
		int len_base = 4;
		while(position >= skip_dimensions)
		  {
		    len_base *= sym.dimlist[position];
		    position--;
		  }


		// try to evaluate the offset directly
		  {
		    try
		      {
			int idx = 0;
			int len = len_base;
			for(int i = skip_dimensions - 1;
			    i >= 0;
			    i--)
			  {
			    idx += caller_dimlist.body[i]->eval_compile(ctx, 0) * len;
			    len *= sym.dimlist[i];
			  }
			// If succeed
			{
			  std::stringstream ss;
			  ss<<COLOR_GREEN "Compile-time eval succeed for ";
			  ((arr_ident_t*)this->real_args.body[i])->printNode(-1, 0, ss);
			  ss<<COLOR_GREEN " at " << COLOR_GREEN
			    << ((arr_ident_t*)this->real_args.body[i])->name.name
			    <<COLOR_WHITE"+";
			  ss<<COLOR_GREEN<<idx;
			  IRList.push_back(new IRNote_t(ss.str()));
			  operand_t opd = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
			  IRList.push_back(new IROper_t(OPER_ADD_OFFSET, opd,  sym.name, idx));
			  args.push_back(opd);
			  continue;
			}
		      }
		    catch(...)
		      {

		      }
		  }
		// Compile-time evaluation failed.
		operand_t idx(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
		operand_t len(LOCAL_VAR(std::to_string(ctx.get_unique_id())));

		{ // Note
		  std::stringstream ss;
		  ss<<COLOR_GREEN "Compile-time eval " COLOR_RED "FAILED" COLOR_GREEN " for ";
		  this->printNode(-1, 0, ss);
		  IRList.push_back(new IRNote_t(ss.str()));
		}

		{ // Calculate INDEX and LENGTH
		  int dims = caller_dimlist.body.size() - 1;
		  auto& ve = caller_dimlist.body;
		  IRList_t& cur_IRList = *(new IRList_t);

		  cur_IRList.push_back(new IRNote_t("Current Position is " COLOR_WHITE + std::to_string(dims)));
		  auto pr = ve[dims]->eval_runtime(ctx);

		  { // Notes
		    std::stringstream ss;
		    ss<<COLOR_GREEN "INDEX ";
		    idx.print(ss);
		    ss<<COLOR_GREEN " = LAST ";
		    pr.first.print(ss);
		    ss<<COLOR_GREEN " * 4";
		    cur_IRList.push_back(new IRNote_t(ss.str()));
		  }

		  cur_IRList.push_back(pr.second);
		  cur_IRList.push_back(new IROper_t(OPER_MUL, idx, pr.first, len_base));
		  // INDEX := LAST * LEN_BASE;
		  // for 32-bit integer

		  cur_IRList.push_back(new IRNote_t("Set initial value for LENGTH"));
		  cur_IRList.push_back(new IROper_t(OPER_MOVE, len, len_base));


		  {
		    std::stringstream ss;
		    ss <<COLOR_GREEN "Current INDEX"
		       <<COLOR_GREEN " is ";
		    idx.print(ss);
		    cur_IRList.push_back(new IRNote_t(ss.str()));
		    ss.str("");
		    ss <<COLOR_GREEN "Current LENGTH";
		    ss <<COLOR_GREEN " is ";
		    len.print(ss);
		    cur_IRList.push_back(new IRNote_t(ss.str()));

		  }
    
		  cur_IRList.push_back(new IRNote_t("Propagate Started"));
		  IRList.push_back(&cur_IRList);
		  // Noted that we're in SSA form
		  while(--dims >= 0)
		    // INDEX := INDEX + LENGTH * CURRENT_INDEX
		    {
		      IRList_t& cur_IRList = *(new IRList_t);
		      cur_IRList.push_back(new IRNote_t("Current Dimension is " COLOR_WHITE + std::to_string(dims)));
		      pr = ve[dims]->eval_runtime(ctx);
		      cur_IRList.push_back(pr.second);
		      operand_t mul_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
		      operand_t idx_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));

		      cur_IRList.push_back(new IROper_t(OPER_MUL, mul_result, len, pr.first));
		      cur_IRList.push_back(new IROper_t(OPER_ADD, idx_result, idx, mul_result));
		      idx = idx_result;
		      cur_IRList.push_back(new IRNote_t("Propagate LENGTH := LENGTH * CUR_LENGTH"));
		      // LENGTH := LENGTH * CUR_LENGTH
		      operand_t len_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
		      cur_IRList.push_back(new IROper_t(OPER_MUL, len_result, len, sym.dimlist[dims]));
		      len = len_result;
		      {
			std::stringstream ss;
			ss <<COLOR_GREEN "Current INDEX"
			   <<COLOR_GREEN " is ";
			idx.print(ss);
			cur_IRList.push_back(new IRNote_t(ss.str()));
			ss.str("");
			ss <<COLOR_GREEN "Current LENGTH";
			ss <<COLOR_GREEN " is ";
			len.print(ss);
			cur_IRList.push_back(new IRNote_t(ss.str()));

		      }

		      IRList.push_back(&cur_IRList);
		    }

		  {
		    std::stringstream ss;
		    ss <<COLOR_GREEN "Final offset for ";
		    this->real_args.body[i]->printNode(-1, 0, ss);
		    ss <<COLOR_GREEN " is ";
		    idx.print(ss);
		    IRList.push_back(new IRNote_t(ss.str()));
		  }

		  operand_t opd = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
		  IRList.push_back(new IROper_t(OPER_ADD_OFFSET, opd,  sym.name, idx));
		  args.push_back(opd);
		  continue;
		}
		
		
	      }
	      
	    }
	  else
	    {
	      COMPILE_ERROR("Unhandled Exception.");
	    }
	}
    }
  IRList.push_back(new IRNote_t("Argument Push Stage"));
  for(int i = args.size() - 1; i >= 0; i--)
    {
      IRList.push_back(new IROper_t(OPER_SET_ARG, i, args[i]));
    }
  IRList.push_back(new IROper_t(OPER_CALL, retn_val, this->name.name));
  return std::make_pair(retn_val, &IRList);
}

int _number_t::eval_compile(context_t& ctx, int level)
{
  return this->semantic_value;
}

std::pair<operand_t, IRList_t*> _number_t::eval_runtime(context_t& ctx)
{
  return std::make_pair(operand_t(this->semantic_value),(IRList_t*) NULL);
}

int arr_initvallist_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_46);
}

std::pair<operand_t, IRList_t*> arr_initvallist_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_47);
}

int arr_dimlist_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_48);
}

std::pair<operand_t, IRList_t*> arr_dimlist_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_49);
}

int func_real_arglist_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_50);
}

std::pair<operand_t, IRList_t*> func_real_arglist_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_51);
}

int func_formal_arglist_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_52);
}

std::pair<operand_t, IRList_t*> func_formal_arglist_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_53);
}

int func_formal_arg_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_54);
}

std::pair<operand_t, IRList_t*> func_formal_arg_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_55);
}

int stmt_t::eval_compile(context_t& ctx, int level)
{
  myAssert(0, ASSERT_EXIT_CODE_56);
}

std::pair<operand_t, IRList_t*> stmt_t::eval_runtime(context_t& ctx)
{
  myAssert(0, ASSERT_EXIT_CODE_57);
}

int ident_t::eval_compile(context_t& ctx, int level)
// static evaluation, only const is permitted
{
  // std::cerr <<COLOR_RED "Trying to eval compile ident for ";
  // this->printNode(-1, 0, std::cerr);
  // std::cerr <<std::endl;

  auto& p = ctx.var_table_find(this->name);
  constsym_t* csym;
  if(!p.is_global() && !p.is_array()) // it's a local variable, and not an array
    {
      csym = &ctx.const_assign_find(p.name);
      // std::cerr <<COLOR_RED "local ident at ";
      // this->printNode(-1, 0, std::cerr);
      // std::cerr <<" -> " << p.name << std::endl;
    }
  else
    {
      csym = &ctx.const_table_find(this->name);

      // std::cerr <<COLOR_RED "global or array ident at "<< p.name << std::endl;
    }
  if(csym->arrayQ)
    {
      printf("Array name: %s", this->name.c_str());
      COMPILE_ERROR(COLOR_YELLOW "Array is used as variable, but we don't support addressing");
    }
  else
    {
      return csym->value[0];
    }
}

std::pair<operand_t, IRList_t*> ident_t::eval_runtime(context_t& ctx)
{
  // printf("Eval runtime ident_t for:");
  // this->printNode(-1, 0, std::cerr);
  // printf("\n");
  sym_t& sym = ctx.var_table_find(this->name);
  if(sym.is_array())
    {
      throw ex_array_as_single;
    }
  else
    {
      try
	{
	  std::string msg = "ident " COLOR_PURPLE "[" COLOR_CYAN + this->name + 
	    COLOR_PURPLE "]" COLOR_GREEN " has const assignment "
	    COLOR_PURPLE "[" COLOR_CYAN + sym.name + COLOR_RED" -> " COLOR_CYAN
	    + std::to_string(this->eval_compile(ctx, 0)) + COLOR_PURPLE"]";
	  return std::make_pair(this->eval_compile(ctx, 0),
	  			(IRList_t*) new IRNote_t(msg));
	  
	  // return std::make_pair(operand_t(ctx.const_assign_find(sym.name).value[0]),
	  // 			(IRList_t*) new IRNote_t(msg));
	  // Immediate
	}
      catch(...)
	{
	  return std::make_pair(operand_t(sym.name), (IRList_t*) NULL);
	}
    }
  
}

int arr_ident_t::eval_compile(context_t& ctx, int level)
{
  throw std::exception();
  // Can't evaluate as constant
}

std::pair<operand_t, IRList_t*> arr_ident_t::eval_runtime(context_t& ctx)
{
  sym_t& arr = ctx.var_table_find(this->name.name);
  if(!arr.is_array())
    {
      std::string s = "Symbol " + this->name.name + " is not an array.";
      COMPILE_ERROR(s.c_str());
    }
  // ensured that [arr] is an array

  if(this->dimlist.body.size() != arr.dimlist.size())
    {
      throw ex_arr_dim_mismatch;
    }
  // ensured the dimension is matched


  operand_t opd(LOCAL_VAR(std::to_string(ctx.get_unique_id())));  
  IRList_t& IRList = *(new IRList_t);

  {
    // try to evaluate the offset directly
      {
	try
	  {
	    int idx = 0;
	    int len = 4;
	    for(int i = this->dimlist.body.size() - 1;
		i >= 0;
		i--)
	      {
		idx += this->dimlist.body[i]->eval_compile(ctx, 0) * len;
		len *= arr.dimlist[i];
	      }
	    // If succeed
	    {
	      std::stringstream ss;
	      ss<<COLOR_GREEN "Compile-time eval succeed for ";
	      this->printNode(-1, 0, ss);
	      ss<<COLOR_GREEN " at " << COLOR_GREEN << this->name.name <<COLOR_WHITE"+";
	      ss<<COLOR_GREEN<<idx;
	      IRList.push_back(new IRNote_t(ss.str()));
	      IRList.push_back(new IROper_t(OPER_LOAD, opd, arr.name, idx));
	      return std::make_pair(opd, &IRList);
	    }
	  }
	catch(...)
	  {

	  }
      }
  }
  // Compile-time evaluation failed.
  // we shall set idx & len as operands, let the program to calculate at runtime.

  { // Note
    std::stringstream ss;
    ss<<COLOR_GREEN "Compile-time eval " COLOR_RED "FAILED" COLOR_GREEN " for ";
    this->printNode(-1, 0, ss);
    IRList.push_back(new IRNote_t(ss.str()));
  }

  // Calculate INDEX and LENGTH
  int dims = this->dimlist.body.size() - 1;
  auto& ve = this->dimlist.body;
  IRList_t& cur_IRList = *(new IRList_t);

  cur_IRList.push_back(new IRNote_t("Current Position is " COLOR_WHITE + std::to_string(dims)));
  auto pr = ve[dims]->eval_runtime(ctx);
  operand_t idx(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
  operand_t len(LOCAL_VAR(std::to_string(ctx.get_unique_id())));

  { // Notes
    std::stringstream ss;
    ss<<COLOR_GREEN "INDEX ";
    idx.print(ss);
    ss<<COLOR_GREEN " = LAST ";
    pr.first.print(ss);
    ss<<COLOR_GREEN " * 4";
    cur_IRList.push_back(new IRNote_t(ss.str()));
  }

  cur_IRList.push_back(pr.second);
  cur_IRList.push_back(new IROper_t(OPER_SHL, idx, pr.first, 2));
  // INDEX := LAST * 4;
  // for 32-bit integer

  cur_IRList.push_back(new IRNote_t("Set initial value for LENGTH"));
  cur_IRList.push_back(new IROper_t(OPER_MOVE, len, 4*arr.dimlist[dims]));


  {
    std::stringstream ss;
    ss <<COLOR_GREEN "Current INDEX"
       <<COLOR_GREEN " is ";
    idx.print(ss);
    cur_IRList.push_back(new IRNote_t(ss.str()));
    ss.str("");
    ss <<COLOR_GREEN "Current LENGTH";
    ss <<COLOR_GREEN " is ";
    len.print(ss);
    cur_IRList.push_back(new IRNote_t(ss.str()));

  }
    
  cur_IRList.push_back(new IRNote_t("Propagate Started"));
  IRList.push_back(&cur_IRList);
  // Noted that we're in SSA form
  while(--dims >= 0)
    // INDEX := INDEX + LENGTH * CURRENT_INDEX
    {
      IRList_t& cur_IRList = *(new IRList_t);
      cur_IRList.push_back(new IRNote_t("Current Dimension is " COLOR_WHITE + std::to_string(dims)));
      pr = ve[dims]->eval_runtime(ctx);
      cur_IRList.push_back(pr.second);
      
      operand_t mul_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
      operand_t idx_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));

      cur_IRList.push_back(new IROper_t(OPER_MUL, mul_result, len, pr.first));
      cur_IRList.push_back(new IROper_t(OPER_ADD, idx_result, idx, mul_result));
      idx = idx_result;
      cur_IRList.push_back(new IRNote_t("Propagate LENGTH := LENGTH * CUR_LENGTH"));
      // LENGTH := LENGTH * CUR_LENGTH
      operand_t len_result(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
      cur_IRList.push_back(new IROper_t(OPER_MUL, len_result, len, arr.dimlist[dims]));
      len = len_result;
      {
	std::stringstream ss;
	ss <<COLOR_GREEN "Current INDEX"
	   <<COLOR_GREEN " is ";
	idx.print(ss);
	cur_IRList.push_back(new IRNote_t(ss.str()));
	ss.str("");
	ss <<COLOR_GREEN "Current LENGTH";
	ss <<COLOR_GREEN " is ";
	len.print(ss);
	cur_IRList.push_back(new IRNote_t(ss.str()));

      }

      IRList.push_back(&cur_IRList);
    }

  {
    std::stringstream ss;
    ss <<COLOR_GREEN "Final offset for ";
    this->printNode(-1, 0, ss);
    ss <<COLOR_GREEN " is ";
    idx.print(ss);
    IRList.push_back(new IRNote_t(ss.str()));
  }

  // LOAD Data
  IRList.push_back(new IROper_t(OPER_LOAD, opd, arr.name, idx));
  return std::make_pair(opd, &IRList);
}

std::pair<cond_result_t,IRList_t*> expr_t::eval_cond(context_t& ctx)
// unary condition
// compare with [0]
{
  IRList_t& IRList = *(new IRList_t);
  cond_result_t result;
  // First, try to eval_compile
  try
    {
      if(this->eval_compile(ctx, 0)) // Cam be evaluated at compile time
	{
	  // Always true
	  IRList.push_back(new IRNote_t("Compile-time eval Succeed at TRUE branch"));	  
	  return std::make_pair(cond_result_t
				{   OPER_JMP, 
				    OPER_NOP
				    },&IRList);
	  
	}
      else
	{ // Always False
	  IRList.push_back(new IRNote_t("Compile-time eval Succeed at FALSE branch"));
	  return std::make_pair(cond_result_t
				{   OPER_NOP,
				    OPER_JMP
				    },&IRList);
	  
	}
    }
  catch(...)
    {}
  auto body_ret = this->eval_runtime(ctx);
  IRList.push_back(body_ret.second);
  IRList.push_back(new IROper_t(OPER_CMP, body_ret.first, 0));
  result.then_op = OPER_JNE;
  result.else_op = OPER_JEQ;
  return {result, &IRList};
}

std::pair<cond_result_t,IRList_t*> _binary_expr_t::eval_cond(context_t& ctx)
// binary condition
{
  IRList_t& IRList = *(new IRList_t);
  {
    std::stringstream ss;
    this->printNode(-1,0,ss);
    IRList.note = "_binary_expr_t :: eval_cond at" + ss.str() + COLOR_GREEN;
  }
  try
    {
      if(this->eval_compile(ctx, 0)) // can be evaluated at compile time
	{ // Always True, [then] directly jump, [else] won't reach
	  IRList.push_back(new IRNote_t("Compile-time eval Succeed at TRUE branch"));	  
	  return std::make_pair(cond_result_t
	    {   OPER_JMP, 
		OPER_NOP
		},&IRList);
	}
      else
	{ // Always False, [then] won't reach, [else] directly jump
	  IRList.push_back(new IRNote_t("Compile-time eval Succeed at FALSE branch"));
	  return std::make_pair(cond_result_t
	    {   OPER_NOP,
		OPER_JMP
		},&IRList);
	}
    }
  catch(const std::exception& e)
    {
      std::stringstream ss;
      this->printNode(-1,0,ss);
      IRList.push_back(new IRNote_t(std::string("Compile-time eval failed for " + ss.str())));
    } // If cannot evaluate in compile time, do nothing

  switch(this->oper)
    {
      // Define [REVERSEQ], If [ops1] is Immediate, Swap it to [ops2] to make sure that [ops1]
      // is always non-immediate
#define CASE(OPCASE, THEN_OPER, ELSE_OPER, REVERSE_THEN_OPER, REVERSE_ELSE_OPER) \
      case OPCASE:							\
	{								\
	  auto ops1_result = this->ops1.eval_runtime(ctx);		\
	  auto ops2_result = this->ops2.eval_runtime(ctx);		\
	  if(ops1_result.first.type == OPERAND_IMM)			\
	    {								\
	      myAssert(ops2_result.first.type != OPERAND_IMM, ASSERT_EXIT_CODE_58);		\
	      IRList.push_back(ops1_result.second);			\
	      IRList.push_back(ops2_result.second);			\
	      IRList.push_back(new IROper_t(OPER_CMP,			\
					    ops2_result.first,		\
					    ops1_result.first)		\
			       );					\
	      return std::make_pair(cond_result_t { REVERSE_THEN_OPER, REVERSE_ELSE_OPER }, \
				    &IRList);				\
	    }								\
	  else								\
	    {								\
	      IRList.push_back(ops1_result.second);			\
	      IRList.push_back(ops2_result.second);			\
	      IRList.push_back(new IROper_t(OPER_CMP,			\
					    ops1_result.first,		\
					    ops2_result.first)		\
			       );					\
	      return std::make_pair(cond_result_t { THEN_OPER, ELSE_OPER }, \
				    &IRList);				\
	    }								\
	}
      
// #define CASE(OPCASE, THEN_OPER, ELSE_OPER)				\
//       case OPCASE:							\
// 	{								\
// 	  auto ops1_result = this->ops1.eval_runtime(ctx);		\
// 	  auto ops2_result = this->ops2.eval_runtime(ctx);		\
// 	  IRList.push_back(ops1_result.second);				\
// 	  IRList.push_back(ops2_result.second);				\
// 	  IRList.push_back(new IROper_t(OPER_CMP,			\
// 					      ops1_result.first,	\
// 					      ops2_result.first)	\
// 				 );					\
// 	  return std::make_pair(cond_result_t { THEN_OPER, ELSE_OPER }, \
// 				&IRList);				\
// 	}

      CASE(EQ, // Conditional Expression is Equality test
	   OPER_JEQ,   // [a == b] [JEQ <then_target>]
	   OPER_JNE,   //          [JNE <else_target>]
	   OPER_JEQ,   // [b == a] [JEQ]
	   OPER_JNE);  //          [JNE]
      
      CASE(NE, // Conditional Expression is Not-Eq test
	   OPER_JNE,   // [a != b] [JNE <then_target>]
	   OPER_JEQ,   //          [JEQ <else_target>]
	   OPER_JNE,   // [b != a] [JNE]
	   OPER_JEQ);  //          [JEQ]

      CASE(LT,
	   OPER_JLT,   // [a <  b] [JLT]
	   OPER_JGE,   // [a >= b] [JGE]
	   OPER_JGT,   // [b >  a] [JGT]
	   OPER_JLE    // [b <= a] [JLE]
	   );

      CASE(GT,
	   OPER_JGT,   // [a >  b] [JGT]
	   OPER_JLE,   // [a <= b] [JLE]
	   OPER_JLT,   // [b <  a] [JLT]
	   OPER_JGE    // [b >= a] [JGE]
	   );

      CASE(LE,
	   OPER_JLE,   // [a <= b] [JLE]
	   OPER_JGT,   // [a >  b] [JGT]
	   OPER_JGE,   // [b >= a] [JGE]
	   OPER_JLT    // [b <  a] [JLT]
	   );

      CASE(GE,
	   OPER_JGE,   // [a >= b] [JGE]
	   OPER_JLT,   // [a <  b] [JLT]
	   OPER_JLE,   // [b <= a] [JLE]
	   OPER_JGT);  // [b >  a] [JGT]

    default: // Other binary expression, directly evaluate
      {
	auto all_result = this->eval_runtime(ctx);
	IRList.push_back(all_result.second);
	myAssert(all_result.first.type != OPERAND_IMM, ASSERT_EXIT_CODE_59);
	IRList.push_back(new IROper_t(OPER_CMP,
				      all_result.first, 0
					    ));
	return std::make_pair(
			      cond_result_t { OPER_JNE, // Evaluate result is NOT ZERO
				  OPER_JEQ },
			      &IRList);
      }
    }
}
