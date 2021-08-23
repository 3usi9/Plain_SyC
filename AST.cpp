#include "AST.h"
#include "common.h"
#include "tokens.h"
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <set>
#include "context.h"
#include "IR.h"
#include "irtree_optimize.h"
extern std::string input_filename;
extern context_t global_ctx;
extern std::map<std::string, int> argcount;
#ifdef COLOR_CODE
char COLOR_ARRAY[6][10] =
  {
    COLOR_CYAN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_WHITE,
    COLOR_WHITE
  };
#define COLOR_MAX_LEVELS 5
#define PRINT_COLOR_CODE(out, level) \
  (out << COLOR_ARRAY[(level) % COLOR_MAX_LEVELS])
#else
#define PRINT_COLOR_CODE(out, level) ;
#endif

#ifdef COLOR_CODE
char BRIGHT_ARRAY[6][10] =
  {
    COLOR_CYAN,
    COLOR_YELLOW,
    COLOR_WHITE,
    COLOR_PURPLE,
    COLOR_GREEN
  };
#define BRIGHT_MAX_LEVELS 4
#define PRINT_BRIGHT_CODE(out, level) \
  (out << (((level) >= 0)?BRIGHT_ARRAY[(level) % BRIGHT_MAX_LEVELS]:("")))
#else
#define PRINT_BRIGHT_CODE(out, level) ;
#endif
// easy-to-read color levels, for parenthes,...



/*******************************/
// ASTNode
ASTNode_t::ASTNode_t()
{
}

ASTNode_t::~ASTNode_t()
{
}


void ASTNode_t::__print_Indentation(int   levelspec,
				    bool  isEnd,
				    std::ostream& out)
{
  if(levelspec < 0) return;
  for(int i = 0; i < levelspec; i++)
    {
     {
       PRINT_COLOR_CODE(out, i);
       out << "│   ";
     }
  }
  PRINT_COLOR_CODE(out,levelspec);
  if (isEnd)
    {
      
      out << "└──";
    }
  else
    {
    out << "├──";
    }
}

void ASTNode_t::__print_message(const char *msg, std::ostream &out, const std::string note)
{
  out << COLOR_YELLOW << "["
      << COLOR_GREEN << msg
      << COLOR_YELLOW << "]"
      << COLOR_NONE;
  if(note != "")
    {
      out << COLOR_RED <<" // "<< COLOR_GREEN << note;
    }
  out  << std::endl;
}


/*******************************/
// printNode

// virtual
void ASTNode_t::printNode(int levelspec,
			  bool isEnd,
			  std::ostream& out)
{
  assert(0);
  // Should not print node for [ASTNode_t]
}




void root_t::printNode(int levelspec,
		       bool isEnd,
		       std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  __print_message("root_t", out);
  for(auto p = this->body.begin();
      p != this->body.end();
      p++)
    {
      (*p)->printNode(levelspec+1, 0, out);
    }
  // TODO

}

void func_def_t::printNode(int levelspec,
			   bool isEnd,
			   std::ostream& out)
{
  std::stringstream myOut;
  __print_Indentation(levelspec, isEnd, out);
  myOut<<COLOR_RED<<"FuncDef ";
  this->printToken(this->retn_type,levelspec, isEnd, myOut);
  myOut<<" ";
  this->name.printNode(levelspec, isEnd, myOut);
  
  PRINT_BRIGHT_CODE(myOut, levelspec);
  myOut<<"(";
  this->formal_args.printNode(levelspec, isEnd, myOut);
  PRINT_BRIGHT_CODE(myOut, levelspec);
  myOut<<")";
  std::string s = myOut.str();
  __print_message(s.c_str(), out);

  this->body.printNode(levelspec+1, isEnd, out);
}

void expr_t::printNode(int levelspec,
		       bool isEnd,
		       std::ostream& out)
{
  assert(0);
  // TODO
}

void var_def_t::printNode(int levelspec,
			  bool isEnd,
			  std::ostream& out)
{
  assert(0);
  // TODO
}

void _var_def_one_noinit_t::printNode(int levelspec,
				      bool isEnd,
				      std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  // TODO
} // no_indent

void _var_def_one_withinit_t::printNode(int levelspec,
					bool isEnd,
					std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  out<<COLOR_PURPLE<<" = ";
  this->value.printNode(levelspec, isEnd, out);
  // TODO
}

void _var_def_array_noinit_t::printNode(int levelspec,
					bool isEnd,
					std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  // TODO
}

void _var_def_array_withinit_t::printNode(int levelspec,
					  bool isEnd,
					  std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  out<<COLOR_PURPLE<<" = ";  
  this->value.printNode(levelspec, isEnd, out);
  // TODO
}

void _cond_expr_t::printNode(int levelspec,
			     bool isEnd,
			     std::ostream& out)
{
  assert(0);
  // TODO
}

void _binary_expr_t::printNode(int levelspec,
			       bool isEnd,
			       std::ostream& out)
{
  PRINT_BRIGHT_CODE(out, levelspec);
  out << "(";
  this->ops1.printNode(levelspec+1, isEnd, out);
  out << " ";
  this->printToken(this->oper,
		   levelspec,
		   isEnd,
		   out);
  out << " ";
  this->ops2.printNode(levelspec+1, isEnd, out);
  PRINT_BRIGHT_CODE(out, levelspec);
  out << ")";
  // TODO
}

void _unary_expr_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  PRINT_BRIGHT_CODE(out, levelspec);
  out << "(";
  this->printToken(this->oper,
		   levelspec,
		   isEnd,
		   out);
  this->opd.printNode(levelspec+1, isEnd, out);
  PRINT_BRIGHT_CODE(out, levelspec);
  out << ")";
}

void _func_call_t::printNode(int levelspec,
			     bool isEnd,
			     std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  PRINT_BRIGHT_CODE(out, levelspec);
  out << "(";
  this->real_args.printNode(levelspec+1, isEnd, out);
  PRINT_BRIGHT_CODE(out, levelspec);
  out << ")";
}

void _number_t::printNode(int levelspec,
			  bool isEnd,
			  std::ostream& out)
{
  if(this->radix == DEC)
    {
      out<<COLOR_GREEN;
      out<<std::dec<<this->semantic_value;
    }
  else if(this->radix == OCT)
    {
      out<<COLOR_BLUE<<"0";
      out<<COLOR_PURPLE;
      out<<std::oct<<this->semantic_value;
    }
  else if(this->radix == HEX)
    {
      out<<COLOR_CYAN;
      out<<std::hex<<"0x"<<this->semantic_value;
    }
  else
    {
      PANIC_MSG("No known radix for the number.");
    }
  out<<std::dec; // restore the default settings.
  
} // print a number in radix form

void arr_initvallist_t::printNode(int levelspec,
				  bool isEnd,
				  std::ostream& out)
{
  PRINT_BRIGHT_CODE(out, levelspec);
  out<<"{";
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      (*p)->printNode(levelspec+1, isEnd, out);
      if(this->body.end() - p != 1) // non-reaches the end
	{
	  out<<COLOR_YELLOW <<  ", ";
	}
    }
  PRINT_BRIGHT_CODE(out, levelspec);
  out<<"}";

  // TODO
}

void arr_dimlist_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      out<<COLOR_WHITE<<"[";
      if((*p) != NULL)
	(*p)->printNode(levelspec+1, isEnd, out);
      // For formal args, the first element in dimlist vector is NULL
      out<<COLOR_WHITE<<"]";
    }
  // TODO
}

void func_real_arglist_t::printNode(int levelspec,
				    bool isEnd,
				    std::ostream& out)
{
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      (*p)->printNode(levelspec, isEnd, out);
      if(this->body.end() - p != 1) // non-reaches the end
	{
	  out<<COLOR_YELLOW <<  ", ";
	}
    }
  // TODO
}

void func_formal_arglist_t::printNode(int levelspec,
				      bool isEnd,
				      std::ostream& out)
{
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      
      this->printToken((*p)->arg_type, levelspec, isEnd, out);
      out<<" ";
      (*p)->arg_name.printNode(levelspec, isEnd, out);

      if(this->body.end() - p != 1) // non-reaches the end
      	{
      	  out<<COLOR_YELLOW <<  ", ";
      	}
    }
  // TODO
}

void func_formal_arg_t::printNode(int levelspec,
				  bool isEnd,
				  std::ostream& out)
{
  assert(0);
  // TODO
}

void stmt_t::printNode(int levelspec,
		       bool isEnd,
		       std::ostream& out)
{
  assert(0);
  // TODO
}

void ident_t::printNode(int levelspec,
			bool isEnd,
			std::ostream& out)
{
  std::string s;
  s += COLOR_GREEN;
  s += (this->name);
  out<<s;
} // print an Identifier, no [levelspec]/ [isend] used.

void arr_ident_t::printNode(int levelspec,
			    bool isEnd,
			    std::ostream& out)
{
  this->name.printNode(levelspec, isEnd, out);
  this->dimlist.printNode(levelspec, isEnd, out);
  // TODO
}

void _assign_stmt_t::printNode(int levelspec,
			       bool isEnd,
			       std::ostream& out)
{
  __print_Indentation(levelspec, isEnd,out);
  std::stringstream myout;
  myout<<COLOR_RED<<"StmtAssign ";
  this->lhs.printNode(levelspec, isEnd, myout);
  myout<<COLOR_PURPLE<<" = ";
  this->rhs.printNode(levelspec, isEnd, myout);
  __print_message(myout.str().c_str(), out);
  // TODO
}

void _eval_stmt_t::printNode(int levelspec,
			     bool isEnd,
			     std::ostream& out)
{
  __print_Indentation(levelspec, isEnd,out);
  std::stringstream myout;
  myout<<COLOR_RED<<"StmtExp ";
  this->expr.printNode(0, 0, myout);
  __print_message(myout.str().c_str(), out);
  // TODO
}

void _block_stmt_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  this->block_body.printNode(levelspec, isEnd, out);
  // TODO
}

void _if_noelse_stmt_t::printNode(int levelspec,
				  bool isEnd,
				  std::ostream& out)
{
  __print_Indentation(levelspec, isEnd,out);
  std::stringstream myout;
  myout<<COLOR_RED<<"StmtIf "<<COLOR_YELLOW<<"if";
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<"(";
  this->cond_expr.printNode(levelspec+1, isEnd, myout);
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<")";
  this->__print_message(myout.str().c_str(), out);

  __print_Indentation(levelspec+1, false,out);
  out << COLOR_GREEN << " // THEN" << std::endl;

  this->then_stmt.printNode(levelspec+1, isEnd, out);

  __print_Indentation(levelspec+1, true,out);
  out << COLOR_GREEN << " // FI" << std::endl;
  // TODO
}

void _if_withelse_stmt_t::printNode(int levelspec,
			   bool isEnd,
			   std::ostream& out)
{
  __print_Indentation(levelspec, isEnd,out);
  std::stringstream myout;
  myout<<COLOR_RED<<"StmtIf"<<COLOR_CYAN<<"Else "<<COLOR_YELLOW<<"if";
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<"(";
  this->cond_expr.printNode(levelspec+1, isEnd, myout);
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<")";
  this->__print_message(myout.str().c_str(), out);
  
  __print_Indentation(levelspec+1, false,out);
  out << COLOR_GREEN << " // THEN" << std::endl;
  
  this->then_stmt.printNode(levelspec+1, isEnd, out);
  
  __print_Indentation(levelspec+1, false,out);
  out << COLOR_GREEN << " // ELSE" << std::endl;

  this->else_stmt.printNode(levelspec+1, isEnd, out);
  
  __print_Indentation(levelspec+1, true,out);
  out << COLOR_GREEN << " // FI" << std::endl;

}

void _while_stmt_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  __print_Indentation(levelspec, isEnd,out);
  std::stringstream myout;
  myout<<COLOR_RED<<"StmtWhile "<<COLOR_YELLOW<<"while";
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<"(";
  this->cond_expr.printNode(levelspec+1, isEnd, myout);
  PRINT_BRIGHT_CODE(myout, levelspec);
  myout<<")";
  this->__print_message(myout.str().c_str(), out);
  
  __print_Indentation(levelspec+1, false,out);
  out << COLOR_GREEN << " // WBEGIN" << std::endl;
  
  this->body_stmt.printNode(levelspec+1, isEnd, out);
  
  __print_Indentation(levelspec+1, true,out);
  out << COLOR_GREEN << " // WEND" << std::endl;

  // TODO
}

void _break_stmt_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  this->__print_message(COLOR_RED "break", out);
  // TODO
}

void _continue_stmt_t::printNode(int levelspec,
				 bool isEnd,
				 std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  this->__print_message(COLOR_RED "continue", out);
  // TODO
}

void _return_novalue_stmt_t::printNode(int levelspec,
				       bool isEnd,
				       std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  this->__print_message(COLOR_RED "return", out);
  // TODO
}

void _return_withvalue_stmt_t::printNode(int levelspec,
					 bool isEnd,
					 std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  std::stringstream myout;
  myout << COLOR_RED << "return ";
  this->retn_value.printNode(levelspec, isEnd, myout);
  this->__print_message(myout.str().c_str(),out);
  
  // TODO
}

void _empty_stmt_t::printNode(int levelspec,
			      bool isEnd,
			      std::ostream& out)
{
  assert(0);
  // TODO
}

void block_t::printNode(int levelspec,
			bool isEnd,
			std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  __print_message("block_t", out);
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      (*p)->printNode(levelspec+1,
		      (p == this->body.end() - 1)? true: false,
		      out);
    }

  // TODO
}

void decl_t::printNode(int levelspec,
		       bool isEnd,
		       std::ostream& out)
{
  __print_Indentation(levelspec, isEnd, out);
  std::stringstream myout;
  myout<<COLOR_RED<<"Decl ";
  if(this->is_const)
    {
      myout<<COLOR_YELLOW<<"const ";
    }
  this -> printToken(this->type, 0,0, myout);
  myout<<" ";
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      (*p)->printNode(0,0,myout);
      if(this->body.end() - p != 1)
	myout<<COLOR_YELLOW<<", ";
    }
  __print_message(myout.str().c_str(), out);
}


void ASTNode_t::printToken(token_t token,
			   int levelspec,
			   bool isEnd,
			   std::ostream& out,
			   bool RequireIdent)
{
  if(RequireIdent)
    {
      ASTNode_t::__print_Indentation(levelspec, isEnd, out);
    }
  
  switch(token)
    {
    case TYPE_VOID:
    case TYPE_INTEGER:
      out << COLOR_CYAN <<( (token == TYPE_VOID)?"void":"int");
      break;
    case PLUS:
      out << COLOR_WHITE << "+";
      break;
    case MINUS:
      out << COLOR_WHITE << "-";
      break;
    case MUL:
      out << COLOR_WHITE << "*";
      break;
    case DIV:
      out << COLOR_WHITE << "/";
      break;
    case MOD:
      out << COLOR_WHITE << "%";
      break;

    case GT:
      out << COLOR_WHITE << ">";
      break;

    case GE:
      out << COLOR_WHITE << ">=";
      break;

    case LT:
      out << COLOR_WHITE << "<";
      break;

    case LE:
      out << COLOR_WHITE << "<=";
      break;

    case EQ:
      out << COLOR_WHITE << "==";
      break;

    case NE:
      out << COLOR_WHITE << "!=";
      break;
      
    case LAND:
      out << COLOR_WHITE << "&&";
      break;

    case LOR:
      out << COLOR_WHITE << "||";
      break;

    case LNEG:
      out << COLOR_WHITE << "!";
      break;
      
    default:
      std::cerr << COLOR_YELLOW << "Token "
		<< COLOR_GREEN << token
		<< COLOR_YELLOW <<" not implemented"<<std::endl;
      PANIC_MSG("");
    }
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/// S E M A N T I C //////////////////////////////////////////
/////////////////////////////////// A N A L Y S I S //////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void ASTNode_t::check_semantic()
{
  assert(0);
}

void expr_t::check_semantic()
{
  assert(0);
}

void ident_t::check_semantic()
{
  assert(0);
}

void _cond_expr_t::check_semantic()
{
  assert(0);
}

void _binary_expr_t::check_semantic()
{
  assert(0);
}

void _unary_expr_t::check_semantic()
{
  assert(0);
}

void _func_call_t::check_semantic()
{
  assert(0);
}

void _number_t::check_semantic()
{
  assert(0);
}

void arr_initvallist_t::check_semantic()
{
  assert(0);
}

void arr_dimlist_t::check_semantic()
{
  assert(0);
}

void func_real_arglist_t::check_semantic()
{
  assert(0);
}

void func_formal_arg_t::check_semantic()
{
  assert(0);
}

void func_formal_arglist_t::check_semantic()
{
  assert(0);
}

void stmt_t::check_semantic()
{
  assert(0);
}

void arr_ident_t::check_semantic()
{
  assert(0);
}

void _assign_stmt_t::check_semantic()
{
  assert(0);
}

void _eval_stmt_t::check_semantic()
{
  assert(0);
}

void _block_stmt_t::check_semantic()
{
  assert(0);
}

void _if_noelse_stmt_t::check_semantic()
{
  assert(0);
}

void _if_withelse_stmt_t::check_semantic()
{
  assert(0);
}

void _while_stmt_t::check_semantic()
{
  assert(0);
}

void _break_stmt_t::check_semantic()
{
  assert(0);
}

void _continue_stmt_t::check_semantic()
{
  assert(0);
}

void _return_novalue_stmt_t::check_semantic()
{
  assert(0);
}

void _return_withvalue_stmt_t::check_semantic()
{
  assert(0);
}

void _empty_stmt_t::check_semantic()
{
  assert(0);
}

void block_t::check_semantic()
{
  assert(0);
}

void func_def_t::check_semantic()
{
  assert(0);
}

void var_def_t::check_semantic()
{
  assert(0);
}

void _var_def_one_noinit_t::check_semantic()
{
  assert(0);
}

void _var_def_one_withinit_t::check_semantic()
{
  assert(0);
}

void _var_def_array_noinit_t::check_semantic()
{
  assert(0);
}

void _var_def_array_withinit_t::check_semantic()
{
  assert(0);
}

void decl_t::check_semantic()
{
  assert(0);
}

void root_t::check_semantic()
{
  assert(0);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////// M A K E //////////////////////////////////////////
/////////////////////////////////// I R //////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
IR_t* ASTNode_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
  // This function shall not be called
}

IR_t* expr_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* ident_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _cond_expr_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _binary_expr_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _unary_expr_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _func_call_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _number_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* arr_initvallist_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* arr_dimlist_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* func_real_arglist_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* func_formal_arg_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* func_formal_arglist_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* arr_ident_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
}

IR_t* _assign_stmt_t::make_IR(context_t& ctx)
// Use [SSA] form with renaming
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  {
    std::stringstream ss;
    this->lhs.printNode(0, 0, ss);
    ss<<COLOR_PURPLE<<" = ";
    this->rhs.printNode(0, 0, ss);
    IRList.note = "Assignment at " + ss.str();
  }
  if(typeid(this->lhs) == typeid(arr_ident_t))
    // Array assignment
    { // If
      // assert(
      // 	     !ctx.var_table_find(((arr_ident_t*) &this->lhs) ->name.name).is_arg()
      // 	     );
      // // The elements of two sides MUST NOT be arg type
      
      auto rhs_result = this->rhs.eval_runtime(ctx);
      IRList.push_back(rhs_result.second);
      arr_ident_t& lhs = *((arr_ident_t*)(&this->lhs));
      auto arr = ctx.var_table_find(lhs.name.name);
      if(!arr.is_array())
	{
	  COMPILE_ERROR("Trying to assign to a non-array as array");
	}
	
      if(arr.dimlist.size() != lhs.dimlist.body.size())
	{
	  COMPILE_ERROR("Array Dimension Conflict");
	}

      // try to evaluate the offset directly
	{
	  try
	    {
	      int idx = 0;
	      int len = 4;
	      for(int i = lhs.dimlist.body.size() - 1;
		  i >= 0;
		  i--)
		{
		  idx += lhs.dimlist.body[i]->eval_compile(ctx, 0) * len;
		  len *= arr.dimlist[i];
		}
	      // If succeed
	      {
		std::stringstream ss;
		ss<<COLOR_GREEN "Compile-time eval succeed for ";
		this->lhs.printNode(-1, 0, ss);
		ss<<COLOR_GREEN " at " << COLOR_GREEN << lhs.name.name <<COLOR_WHITE"+";
		ss<<COLOR_GREEN<<idx;
		IRList.push_back(new IRNote_t(ss.str()));
		IRList.push_back(new IROper_t(OPER_STORE,
					      arr.name ,
					      idx, 
					      rhs_result.first
					      ));
		return &IRList;
	      }
	    }
	  catch(...)
	    {

	    }
	}
      
      // If the offset cannot be evaluated directly
      { // Evaluate the offset at runtime


	{ // Note
	  std::stringstream ss;
	  ss<<COLOR_GREEN "Compile-time eval " COLOR_RED "FAILED" COLOR_GREEN " for ";
	  this->lhs.printNode(-1, 0, ss);
	  IRList.push_back(new IRNote_t(ss.str()));
	}

	// Calculate INDEX and LENGTH
	int dims = lhs.dimlist.body.size() - 1;
	auto& ve = lhs.dimlist.body;
	IRList_t& cur_IRList = *(new IRList_t);

	cur_IRList.push_back(new IRNote_t("Current Position is " COLOR_WHITE + std::to_string(dims)));
	auto pr = ve[dims]->eval_runtime(ctx);
	operand_t idx(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
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
	operand_t len(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
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
	  this->lhs.printNode(-1, 0, ss);
	  ss <<COLOR_GREEN " is ";
	  idx.print(ss);
	  IRList.push_back(new IRNote_t(ss.str()));
	}

	// LOAD Data
	IRList.push_back(new IROper_t(OPER_STORE,arr.name, idx, rhs_result.first));
	
	return &IRList;
      }
    } // EndIf
  else // Single assignment
    {
      assert(typeid(this->lhs) == typeid(ident_t));
      
      auto  rhs_result =  this->rhs.eval_runtime(ctx);
      auto& lhs_lval   =  ctx.var_table_find(this->lhs.name);

      assert(!lhs_lval.is_arg());
      assert(lhs_lval.is_array() == false);
      // Must not be array

      IRList.push_back(rhs_result.second);
      // Variable Rewriting
      // Requirements:
      //    1. [rhs_result] is VAR, not IMM
      //    2. [rhs_result] is [LOCAL]
      //    3. [lhs_lval]   is [LOCAL]
      //    4???. [lhs] and [rhs] shouldn't appear in the same
      //          condition expression
      //          (hence, they can't appear in the same loop [COND])
      if(rhs_result.first.type == OPERAND_VAR && // 1
	 rhs_result.first.name[0] == '%' &&      // 2
	 ! lhs_lval.is_global())                 // 3
	{ // can be rewritten
	  auto new_label = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
	  ctx.var_table_rewrite(this->lhs.name, new_label);
	  assert(lhs_lval.name == new_label);
	  
	  IRList.push_back(new IROper_t(OPER_MOVE,
					lhs_lval.name,
					rhs_result.first));
	  if(rhs_result.first.type == OPERAND_IMM)
	    {
	      ctx.const_assign_push(lhs_lval.name, rhs_result.first.imm);
	    }
	}
      else if(lhs_lval.is_global()) // global variable can't be rewritten
	{
	  IRList.push_back(new IROper_t(OPER_MOVE,
					lhs_lval.name,
					rhs_result.first));
	  
	}
      else // [lhs] is local
	{
	  auto new_label = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
	  ctx.var_table_rewrite(this->lhs.name, new_label);
	  assert(lhs_lval.name == new_label);
	  
	  IRList.push_back(new IROper_t(OPER_MOVE,
					lhs_lval.name,
					rhs_result.first));
	  if(rhs_result.first.type == OPERAND_IMM)
	    {
	      ctx.const_assign_push(lhs_lval.name, rhs_result.first.imm);
	    }
	}
    }
  return &IRList;
}

IR_t* _eval_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  auto p = this->expr.eval_runtime(ctx);
  IRList.push_back(p.second);
  return &IRList;
}

IR_t* _block_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  ctx.scope_push();
  for(auto p = this->block_body.body.begin();
      p != this->block_body.body.end();
      ++p)
    {
      IRList.push_back((*p)->make_IR(ctx));
      // construct IR tree
    }
  ctx.scope_pop();
  return &IRList;
}

IR_t* _if_noelse_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);  
  auto& it = *(new _if_withelse_stmt_t(this->cond_expr,
				       this->then_stmt,
				       *(new _empty_stmt_t)));
  IRList.note = "if_noelse stmt";
  IRList.push_back(it.make_IR(ctx));
  return &IRList;
}

IR_t* _if_withelse_stmt_t::make_IR(context_t& ctx)
/*
 * The [IF_ELSE] statement include 3 parts:
 * if [COND] then [THEN_BLK] else [ELSE_BLK]
 * and the structure is
 */
 /*
  *               ┌────────────┐
  IF_STMT_[label]:│ COND BODY  │
                  ├────────────┤
                  │ Jcc [THEN] │
                  ├────────────┤
                  │ Jcc [ELSE] │
                  ├────────────┤
     THEN_[label]:│ ...        │
                  │ PHI_MOV    │
                  │ jmp END    │
                  ├────────────┤
     ELSE_[label]:│ ...        │
                  │ PHI_MOV    │
                  ├────────────┤
   IF_END_[label]:│            │
                  └────────────┘
  */  
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  IRList.note = typeid(*this).name();
  ctx.scope_push();
  // create a new var scope
  std::string label = std::to_string(ctx.get_unique_id());
  // get a unique id for this statement

  auto pr = this->cond_expr.eval_cond(ctx);
  if(pr.first.then_op == OPER_JMP) // only [then] is valid
    {
      IRList.push_back(pr.second); // for Note
      IRList.push_back(this->then_stmt.make_IR(ctx));
      ctx.scope_pop();
      return &IRList;
    }
  if(pr.first.else_op == OPER_JMP) // only [else] is valid
    {
      IRList.push_back(pr.second); // for Note
      IRList.push_back(this->else_stmt.make_IR(ctx));
      ctx.scope_pop();
      return &IRList;
    }

  // normal routine
  IRList.push_back(new IROper_t(OPER_LABEL, "IF_STMT_" + label));
  
  IRList.push_back(pr.second);
  // condition evaluation result

  IRList.push_back(new IROper_t(pr.first.else_op, "ELSE_" + label));
  // jump to else branch
  
  // divide into two states
  context_t ctx_then = ctx;
  context_t ctx_else = ctx;
  ctx_then.scope_push();
  IRList_t* ir_then = new IRList_t;
  ir_then->push_back(this->then_stmt.make_IR(ctx_then));
  ctx_then.scope_pop();
  // evaluate [then]
  
  ctx_else.scope_push();
  IRList_t* ir_else = new IRList_t;
  ir_else->push_back(this->else_stmt.make_IR(ctx_else));
  ctx_else.scope_pop();
  // evaluate [else]

  IRList_t* ir_end = new IRList_t;
  ir_end->push_back(new IROper_t(OPER_LABEL, "IF_END_" + label));

  ///////////////////EDIT//////////////////////////////////////////////////
  ////////Consider THEN and ELSE dividely//////////////////////////////////
  ////////Consider them with OUTER's relation, but not themselves//////////
  // find PHI move, traverse two contexts to
  // lookup if there is some variable rewrote
  // noted that in SSA form, if a variable is changed,
  // it must be assigned a new [label], hence it's impossible
  // that [then_scope] is equal to [else_scope] but not equal
  // to [global_scope]


  for(int i = 0; i < ctx_then.symbol_table.size(); i++)
    {
      auto& then_scope = ctx_then.symbol_table[i];
      auto& else_scope = ctx_else.symbol_table[i];
      for(auto it : then_scope)
	{
	  if(it.second.name !=
	     else_scope.find(it.first)->second.name)
	    // two symbols has different name
	    // we need to assign the origin symbol
	    // in [ctx] with a new name, and
	    // MOV [ctx_name] <- [rewrote_name]
	    // without renaming [ctx_name], we introduce
	    // [OPER_PHI_MOV] to address this method
	    {
	    // if(else_scope.find(it.first) == else_scope.end())
	    //   {
	    // 	printf("Could not found [%s->%s] in ELSE's scope\n",
	    // 	       it.first.c_str(),
	    // 	       it.second.name.c_str());
	    // 	continue;
	    // 	// assert(0);
	    //   }
	    // printf("Trying to rewrite: [%s->%s] [%s->%s]\n", it.first.c_str(),
	    // 	   it.second.name.c_str(),
	    // 	   else_scope.find(it.first)->first.c_str(),
	    // 	   else_scope.find(it.first)->second.name.c_str());
	      
	      auto& outer_sym = ctx.var_table_find(it.first);
	      auto& then_sym   = it.second;
	      auto& else_sym   = else_scope.find(it.first)->second;
	      assert(!outer_sym.is_array());
	      // can't be array
	      assert(!outer_sym.is_global());
	      // can't be global

	      std::string new_label = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
	      ctx.var_table_rewrite(it.first, new_label);
	      assert(outer_sym.name == new_label);
	      // rewrite with a new name

	      IROper_t* phi1 = new IROper_t(OPER_PHI_MOV,
					    outer_sym.name,
					    then_sym.name);
	      phi1->phi_target =(IROper_t*) ir_end->body.front();
	      ir_then->push_back(phi1);

	      IROper_t* phi2 = new IROper_t(OPER_PHI_MOV,
					    outer_sym.name,
					    else_sym.name);
	      phi2->phi_target =(IROper_t*) ir_end->body.front();
	      ir_else->push_back(phi2);
	    }
	}
    }
  IRList.push_back(new IROper_t(OPER_LABEL, "THEN_" + label));
  IRList.push_back(ir_then);
  IRList.push_back(new IROper_t(OPER_JMP, "IF_END_" + label));

  IRList.push_back(new IROper_t(OPER_LABEL, "ELSE_" + label));
  IRList.push_back(ir_else);
  /* IRList.push_back(new IROper_t(OPER_JMP, "IF_END_" + label)); */
  // no-need
  IRList.push_back(ir_end);

  // If it's in a loop, we need to add the
  // continue symbols and break symbols into
  // [global_ctx]'s context
  // if(!ctx.loop_break_symbol_snapshot.empty())
  //   {
  //     auto& glob_br = ctx     .loop_break_symbol_snapshot.top();
  //     auto& then_br = ctx_then.loop_break_symbol_snapshot.top();
  //     auto& else_br = ctx_else.loop_break_symbol_snapshot.top();
  //     glob_br.insert(glob_br.end(),
  // 		     then_br.begin(),
  // 		     then_br.end());
  //     glob_br.insert(glob_br.end(),
  // 		     else_br.begin(),
  // 		     else_br.end());
  //     auto& glob_co = ctx     .loop_conti_symbol_snapshot.top();
  //     auto& then_co = ctx_then.loop_conti_symbol_snapshot.top();
  //     auto& else_co = ctx_else.loop_conti_symbol_snapshot.top();
  //     glob_co.insert(glob_co.end(),
  // 		     then_co.begin(),
  // 		     then_co.end());
  //     glob_co.insert(glob_co.end(),
  // 		     else_co.begin(),
  // 		     else_co.end());
    // }
  // printf("global_pop\n");
  ctx.scope_pop();
  // TODO_MANUAL(); // Not tested yet
  return &IRList;
}

IR_t* _while_stmt_t::make_IR(context_t& ctx)
/*
 * The [WHILE] statement include 2 parts:
 * while [COND] do [DO_BLK] done 
 * and control-flow [BREAK]/[CONTINUE]
 * [WARNING: Be careful for nested code blocks]
 */
 /*
  *                  ┌───────────-─┐
  WHILE_STMT_[label]:│ COND BODY   │
                     ├───────────-─┤
        (shortcut)   │ JSUCC [DO]  │
	             │ [PHI MOVES] │
                     │ JFAIL [DONE]│
                     ├───────────-─┤
          DO_[label]:│ ...         │
	             │ [PHI MOVES] │
 BREAK_[other_label]:│ JMP [DONE]  │
	             │ ...         │
	             │             │
	             │ [PHI MOVES] │
  CONTINUE_[oth_lab]:│ JMP [WHILE] │
	             │             │
	             │ [PHI MOVES] │
                     │ JMP [WHILE] │
                     ├────────────-┤
        DONE_[label]:│             │
                     └───────────-─┘
  */
/** We need to pass all [PHI_MOVES] information
    and [JUMP_TARGET] information to each
    [BREAK]/[CONTINUE]/[JUMP_END] nodes.
    Hence, we need to scan the parse tree twice.
    At the first pass, we collect all information needed,
    and before the second pass, we inject the proper information
    into scope (This step is finished at first pass)
    then, generate real code at second pass
*/
/** [WHILE] Statement might be nested, that's to say
    WHILE(COND1)
    DO {
    BREAK1;
      WHILE(COND2)
      DO {
      BREAK2;
      }
    }
    BREAK1 has to take effect for the outside WHILE statement,
    BREAK2 has to take effect for the inside  WHILE statement
    
    Hence, we cannot justc ollect all BREAKs inside the WHILE
    and assign the outmost DONE to them. We need to nested them
    properly.
**/
/** Similarly, the [PHI_MOVES] before each [CONTINUE]/[BREAK] 
    differs too. Consider the code below
    a = 5;
    WHILE(COND1)
    {
    if(...) {
      a = 6;
      BREAK1;
    }
      a = 7;
      BREAK2;
    }
    printf("%d", a);

    Obviously, PHI_MOVE for BREAK1 differs from BREAK2. If they're
    the same
    ;; IR CODE
    ;; BREAK1
    ;; %TMP = 6
    PHI_MOV %a <- %TMP
    JMP DONE
    ;; BREAK2
    PHI_MOV %a <- %TMP
    JMP DONE
    
    Then, variable [a] cannot be assigned to different 
    values at different path.
    
    Hence, at each BREAK/CONTINUE label, we need to evaluate
    PHI_MOVES and assign them properly.
 **/
/** Consider this code:
    WHILE(COND1)
    {
      a = a + 1;
    }
    If we generate
    ;; IRCode
    OPER_ADD %6 <- %5, 1
    Noted that at the path end, we need to insert
    PHI_MOVE %5 <- %6
    %6 is assigned multiple times, but in the loop body,
    there's only one assignment for %6 -- we can still optimize it
 **/
{
  this->_before_make_IR();

  // Create [WHILE]'s scope
  ctx.scope_push();

  // Consider the nested loop
  ctx.loop_label.push(ctx.get_unique_id());

  // Loop variables at each (nested) WHILE loop
  ctx.loop_vars.push({});
  ctx.rewritten.push({});
  ctx.phi_target.push(NULL);

#define WHILE_STMT(x) ("WHILE_STMT_"+std::to_string(x))
#define DO_ENTRY(x) ("DO_" + std::to_string(x))
#define DONE(x) ("DONE_" + std::to_string(x))
  
  context_t original_ctx = ctx;
  ///////////// P A S S 1 ////////////////////////////////
  // pass 1, find all variables that needs to be rewritten
  //
  // Remember the original context,
  // to track updates
  context_t pass1_ctx = original_ctx;

  // evaluate [COND] to find if any symbols are modified
  this->cond_expr.eval_cond(pass1_ctx);
  // evaluate [BODY] to find if any symbols are modified
  this->body_stmt.make_IR(pass1_ctx);

  // prepare for pass2_ctx
  context_t pass2_ctx = original_ctx;
  
  // for Layer [i]
  for(int i = 0; i < original_ctx.symbol_table.size(); i++)
    // For Each symbol
    for(auto entry_symbol : original_ctx.symbol_table[i])
      // For Each CONTINUE Statement
      {
	  // find if the symbol's name is rewritten
	  if(entry_symbol.second.name !=
	     pass1_ctx.symbol_table[i].find(entry_symbol.first)->second.name)
	    {
	      // Need to be rewritten
	      pass2_ctx.rewritten.top().
		insert({
		    {i, entry_symbol.first},
		      LOCAL_VAR(std::to_string(pass2_ctx.get_unique_id()))
			});
	    }
	}

  /////////////////////P A S S 2////////////////////////////////////
  // Firstly, rewrite all variables we've seen at pass 1
  // Before all operation starts
  IRList_t& IRList = *(new IRList_t);
  IRList.note = "Begin of [WHILE] Stmt";
  IRList.push_back(new IRNote_t("Overall Rewrite before all"));
  IRList_t& outer_list = *(new IRList_t); // for Optimizing
  IRList_t& inner_list = *(new IRList_t); // for Optimizing
  IRList_t& rwIRList = *(new IRList_t);
  IRList_t& end_list = *(new IRList_t);
  end_list.push_back(new IROper_t(OPER_LABEL, DONE(pass2_ctx.loop_label.top())));
  end_list.push_back(new IRNote_t("End of [WHILE] Stmt"));
  pass2_ctx.phi_target.top() = (IROper_t*) end_list.body.front();
  for(auto& it : pass2_ctx.rewritten.top())
    {
      auto level   = it.first.first;
      auto liter   = it.first.second;
      auto newname = it.second;
      rwIRList.push_back(new IRNote_t("Rewrite [" COLOR_CYAN + liter +
				    "->"+pass2_ctx.symbol_table[level].at(liter).name+
				    COLOR_GREEN "] to " COLOR_CYAN + newname));
      IROper_t* phi1 = new IROper_t(OPER_PHI_MOV, newname,
				    pass2_ctx.symbol_table[level].at(liter).name
				    );
      phi1->phi_target = pass2_ctx.phi_target.top();
      rwIRList.push_back(phi1);
      // operation_ctx.symbol_table[level].at(liter).name = newname;
      // Manually rewrite (at a specificed level)
      pass2_ctx.symbol_table[level].at(liter).name = newname;
      // Assign a newname at [OPERATION CONTEXT], its previous name is at [PASS2_CTX] and we has already drop it
    }

  outer_list.push_back(&rwIRList);

  { // CONDITION PART
    
    // Then, evaluate COND body
    inner_list.push_back(new IROper_t(OPER_LABEL, WHILE_STMT(pass2_ctx.loop_label.top())));
    inner_list.push_back(new IRNote_t("Entering COND"));
    auto condition_result = this->cond_expr.eval_cond(pass2_ctx);
    inner_list.push_back(condition_result.second);

    // Then, rewrite all symbols that is modified at [COND]
    inner_list.push_back(new IRNote_t("COND REWRITE"));
    IRList_t& cond_rewrite = *(new IRList_t);

#define REWRITE(orig_ctx, irlist)					\
    for(int i = 0; i < orig_ctx.symbol_table.size(); i++)		\
      {									\
	auto& orig_scope = orig_ctx.symbol_table[i];			\
	for(auto& it : orig_scope)					\
	  {								\
	    if(orig_ctx.rewritten.top().count({i, it.first}))		\
	      {								\
		if(orig_ctx.rewritten.top().at({i,it.first}) != it.second.name) \
		  {							\
		    auto newname = orig_ctx.rewritten.top().at({i,it.first}); \
		    IROper_t* phi2 = new IROper_t(OPER_PHI_MOV,		\
						  newname,		\
						  it.second.name);	\
		    phi2->phi_target = orig_ctx.phi_target.top();	\
		    irlist.push_back(phi2);				\
		    it.second.name = newname;				\
		  }							\
	      }								\
	  }								\
      }

    REWRITE(pass2_ctx, cond_rewrite);
    inner_list.push_back(&cond_rewrite);

    // Then, insert Jcc
  inner_list.push_back(new IRNote_t("COND JMPFAIL"));
  inner_list.push_back(new IROper_t(condition_result.first.else_op,
  				DONE(ctx.loop_label.top())));
  }

  { // DO PART
    inner_list.push_back(new IRNote_t("DO part"));
    inner_list.push_back(this->body_stmt.make_IR(pass2_ctx));
  }

  { // AT THE END OF [WHILE] STMT, we need to insert a [CONTINUE] to the beginning
    inner_list.push_back(new IRNote_t("End Rewrite"));
    IRList_t& end_rewrite = *(new IRList_t);
    REWRITE(pass2_ctx, end_rewrite);
    inner_list.push_back(&end_rewrite);
    inner_list.push_back(new IROper_t(OPER_JMP, WHILE_STMT(pass2_ctx.loop_label.top())
				  )
		     );
    inner_list.push_back(new IROper_t(OPER_LABEL, DONE(pass2_ctx.loop_label.top())));
    inner_list.push_back(new IRNote_t("End of [WHILE] Stmt"));
  }
  std::list<IR_t*> outer_line;
  outer_list.collect(outer_line);
  std::list<IR_t*> inner_line;
  inner_list.collect(inner_line);

  // Optimize Switch
  {
    // while(Invariant_Code_Motion(outer_line, inner_line))
    //   {
    //   }
    // outer_list.body = outer_line;
    // inner_list.body = inner_line;
  }
  
  // printf("Outer Line = \n");
  // for(auto it : outer_line)
  //   {
  //     it->print(std::cerr, -1);
  //   }
  // printf("\n\n");
  IRList.push_back(&outer_list);
  IRList.push_back(&inner_list);
  { // Clean
    ctx = pass2_ctx;
    ctx.scope_pop();
    ctx.loop_label.pop();
    ctx.loop_vars.pop();
    ctx.rewritten.pop();
    ctx.phi_target.pop();
  }
  return &IRList;
  // assert(0);
}

IR_t* _break_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  // Check rewrite
  IRList_t& IRList = *(new IRList_t);
  REWRITE(ctx, IRList);
  // jump to [END]
  IRList.push_back(new IROper_t(OPER_JMP, DONE(ctx.loop_label.top())));
  return &IRList;
}

IR_t* _continue_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  // Check rewrite
  IRList_t& IRList = *(new IRList_t);
  REWRITE(ctx, IRList);
  // jump to [BEGIN]
  IRList.push_back(new IROper_t(OPER_JMP, WHILE_STMT(ctx.loop_label.top())));
  return &IRList;
}

IR_t* _return_novalue_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  IRList.push_back(new IROper_t(OPER_RET));
  return &IRList;
}

IR_t* _return_withvalue_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  auto pr = this->retn_value.eval_runtime(ctx);
  IRList.push_back(pr.second);
  IRList.push_back(new IROper_t(OPER_RETN, pr.first));
  return &IRList;
}

IR_t* _empty_stmt_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  return NULL;
}

IR_t* block_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
  // been replaced by a non-virtual function
}

IR_t* block_t::make_IR(bool create_scope, context_t& ctx)
{
  IRList_t& IRList = *(new IRList_t);
  
  if(create_scope)
    ctx.scope_push();
  
  for(auto it : this->body)
    {
      IRList.push_back(it->make_IR(ctx));
    }
  
  if(create_scope)
    ctx.scope_pop();

  return &IRList;
}


IR_t* func_def_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  ctx.scope_push();
  int argcnt = this->formal_args.body.size();
  argcount.insert({this->name.name, argcnt});
  // Function Begin
  IRList.push_back(new IROper_t(OPER_FUNC,
				this->name.name,
				argcnt)
		   );

  std::vector<arg_t*> arglist;
  // record the shape of formal arguments
  // to push into Symbol Table
  
  // process args
  for(int i = 0; i < argcnt; i++)
    {
      assert(this->formal_args.body[i]->arg_type == TYPE_INTEGER);
      // check type
      
      ident_t& arg = this->formal_args.body[i]->arg_name;
      if(typeid(arg) == typeid(ident_t)) // Single var
	{
	  std::string arg_tag = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
	  ctx.var_table_push(arg.name, sym_t(arg_tag, SYMTYPE_SING));
	  // push into symbol table
	  // MOVE %i <- @arg[i]
	  IRList.push_back(new IROper_t(OPER_LOAD_ARG,
					arg_tag,
					ARG(std::to_string(i))
					)
				 );
	  arglist.push_back(new arg_t(ARG_SINGLE));
	}
      else // Array
	{
	  assert(typeid(arg) == typeid(arr_ident_t));
	  std::string arg_tag = REFERENCE_ARR(std::to_string(ctx.get_unique_id()));
	  std::vector<int> dimlist;
	  arr_ident_t* p = (arr_ident_t*) &arg;
	  for(auto it : p->dimlist.body)
	    {
	      if(it == NULL)
		{
		  dimlist.push_back(-1);
		}
	      else
		{
		  dimlist.push_back(it->eval_compile(ctx, -1));
		}
	    } // find the static length


	  ctx.var_table_push(p->name.name,
			     sym_t(arg_tag, SYMTYPE_ARRAY, dimlist)
			     );
	  // tag with '@' WON'T be pushed into [ctx], but a copy.
	  IRList.push_back(new IROper_t(OPER_LOAD_ARG,
					arg_tag,
					ARG(std::to_string(i))
					)
				 );
	  arglist.push_back(new arg_t(ARG_ARRAY, dimlist));
	}
    }


  // push the function into symbol table
  ctx.var_table_push_globl(this->name.name,
			   sym_t(GLOBL_FNC(this->name.name),
				 SYMTYPE_FUNC,
				 arglist)
			   );
  
  // Body
  // printf("Before Make Block\n");
  IRList.push_back(this->body.make_IR(false,ctx));
  // Do NOT create a scope
  // printf("After Make Block\n");
  // default return instruction
  if(this->retn_type == TYPE_INTEGER)
    {
      IRList.push_back(new IROper_t(OPER_RETN, 0));
    }
  else
    {
      IRList.push_back(new IROper_t(OPER_RET));
    }

  IRList.push_back(new IROper_t(OPER_ENDF, this->name.name));

  // Context
  ctx.scope_pop();
  return &IRList;
}

IR_t* var_def_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  assert(0);
  // Should not reach here, this is a pure-virtual class
}

IR_t* _var_def_one_noinit_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);  
  if(ctx.is_global_scope())
    // global-scope, insert and alloc
    {
      // push into symbol table
      sym_t sym(GLOBL_VAR(this->name.name), SYMTYPE_SING);

      ctx.var_table_push(this->name.name, sym);
      // allocate memory region
      IRList.push_back(new IRLabel_t(DATA_AREA_BEGIN,
					   sym.name
					   )
			     );
      IRList.push_back(new IRData_t(sizeof(uint32_t)));
      IRList.push_back(new IRLabel_t(DATA_AREA_END));
    }
  else // not global scope
    {
      
      // use anonymous tag
      std::string lab = LOCAL_VAR(std::to_string(ctx.get_unique_id()));
      sym_t sym(lab,
		SYMTYPE_SING);
      
      ctx.var_table_push(this->name.name, sym);
      // no-need of memory allocation, we don't know it's in register
      // or memory
      IRList.push_back(new IRNote_t("Create new variable " COLOR_CYAN +
				    this->name.name + COLOR_RED " -> " COLOR_CYAN + lab));
    }
  return &(IRList);
}

IR_t* _var_def_one_withinit_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  if(ctx.is_global_scope())
    // global scope, insert and alloc
    {
      // push into symbol table
      sym_t sym(GLOBL_VAR(this->name.name), SYMTYPE_SING);

      ctx.var_table_push(this->name.name, sym);
      // allocate memory region
      IRList.push_back(new IRLabel_t(DATA_AREA_BEGIN,
					   sym.name
					   )
			     );

      uint32_t compile_val = this->value.eval_compile(ctx, -1);
      _irdata_global_area li = (1);
      li[0] = compile_val;
      IRList.push_back(new IRData_t(sizeof(uint32_t),
				    li)
		       );
      IRList.push_back(new IRLabel_t(DATA_AREA_END));

      // if it's a constant, push into constant table
      if(this->is_const)
	{
	  constsym_t csym(this->value.eval_compile(ctx, -1));
	  ctx.const_table_push(this->name.name, csym);
	}
    }
  else
    // local scope, dynamically eval, insert, no alloc, new IRNode for eval
    {
      // push into symbol table, after evaluate
      sym_t sym(LOCAL_VAR(std::to_string(ctx.get_unique_id())),
      		SYMTYPE_SING);
      ctx.var_table_push(this->name.name, sym);

      _assign_stmt_t* tmpAssignNode =
      	new _assign_stmt_t(this->name, this->value);
      IRList.push_back(tmpAssignNode->make_IR(ctx));

      
      // locally-defined values should not be pushed into const table,
      // its value cannot be determined at compile time
      
      // if(this->is_const)
      // 	{
      // 	  constsym_t csym(this->value.eval_compile());
      // 	  ctx.const_table_push(this->name.name, csym);
      // 	}
    }

  return &(IRList);

}

IR_t* _var_def_array_noinit_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  if(ctx.is_global_scope())
    // global scope, insert and alloc
    {
      sym_t sym(GLOBL_ARR(this->name.name.name), SYMTYPE_ARRAY);
      for(auto it : this->name.dimlist.body)
	{
	  sym.dimlist.push_back(it->eval_compile(ctx, -1));
	} // dimension list

      // push into symbol table
      ctx.var_table_push(this->name.name.name, sym);

      // calculate data area size
      int length = 4;
      for(auto it : sym.dimlist)
	{
	  length *= it;
	}

      // allocate memory region
      IRList.push_back(new IRLabel_t(DATA_AREA_BEGIN,
					   sym.name
					   ));
      IRList.push_back(new IRData_t(length));
      // filled to zero

      IRList.push_back(new IRLabel_t(DATA_AREA_END));
    }
  else
    // local scope, no alloc
    {
      sym_t sym(LOCAL_ARR(std::to_string(ctx.get_unique_id())),
		SYMTYPE_ARRAY);
      for(auto it : this->name.dimlist.body)
	{
	  sym.dimlist.push_back(it->eval_compile(ctx, -1));
	  // dynamic-length array is not supported yet.
	  
	} // dimension list
      int length = 4;
      for(auto it : sym.dimlist)
	{
	  length *= it;
	} // data area size
      ctx.var_table_push(this->name.name.name, sym);
      // context

      // IR, require stack allocation
      IROper_t* irp = new IROper_t(OPER_STACK_ALLOC,
				   operand_t(ctx.var_table_find(this->name.name.name).name),
				   operand_t(length));
      
      IRList.push_back(irp);
      // Implicit Initialize
      
      // memset(void* dest, uint8 value, size_t count)
      IRList.push_back(new IROper_t(OPER_SET_ARG, 2, operand_t(length)));
      IRList.push_back(new IROper_t(OPER_SET_ARG, 1, 0));
      IRList.push_back(new IROper_t(OPER_SET_ARG, 0, operand_t(ctx.var_table_find(this->name.name.name).name)));
      operand_t opd(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
      IRList.push_back(new IROper_t(OPER_CALL, opd, std::string("memset")));
    }
  return &IRList;
}

class _array_setter_t
{
public:
  virtual
  void operator()(int index, int value) = 0;

  virtual
  void operator()(int index, operand_t value_oper) = 0;

};

class _global_array_setter_t : public _array_setter_t
// for [GLOBAL] arrays, we need to alloc a segment
// initialized to zero, and set some correspond location
// to specified values
// same for both [CONST] and [NON_CONST] [GLOBAL] arrays
{
public:
  IRData_t* data_pointer;

  virtual
  void operator()(int index, int value)
  {
    data_pointer->data[index] = value;
  }

  virtual
  void operator()(int index, operand_t value_oper)
  {
    assert(0);
  }
  
  // virtual
  // void export_to_const_table(std::vector<int>& vallist)
  // {
  //   for(int i = 0; i < this->data_pointer->length / 4; i++)
  //     {
  // 	vallist.push_back(this->data_pointer->data[i]);
  //     }
  //   return; //okay
  // }

};

class _local_array_setter_t : public _array_setter_t
// for [LOCAL] arrays, we don't need to alloc a segment,
// but need a series of instructions to initialize the correspond
// location to zero, same for [CONST] and [NON_CONST] [LOCAL] arrays
// but for [CONST] arrays, if you access non-initialized elements,
// the behavior is [UNDEFINED]
{
public:
  IRList_t* instruction_list_pointer;
  std::string tagged_name;
  int counts; // data counts
  virtual
  void operator()(int index, int value)
  {
    IROper_t* irp = new IROper_t(OPER_STORE,
				 tagged_name,
				 index * 4,
				 value);
    this->instruction_list_pointer->push_back(irp);
  }

  virtual
  void operator()(int index, operand_t value_oper)
  {
    IROper_t* irp = new IROper_t(OPER_STORE,
				 tagged_name,
				 index * 4,
				 value_oper);
    this->instruction_list_pointer->push_back(irp);
  }

};

int _set_correspond_array_initial_value(int cur, // Current array index
					int total,  // Total number of elements
					arr_initvallist_t* value, // Array initial value list for now
					int dimlist_border,       /* Dimension list border, 
								   * exclusive [0, dimlist_border)
								   */
					std::vector<int>& dimlist, /* Dimension list
								    * need for finding border
								    */
					_array_setter_t& setter, // Value-setter callback function
					// setter(int index, int value)
					bool dynamic_eval,        // fuck
					context_t& ctx            // fuck
					)
// Return Value: the index of next assignment position
{

  if(cur >= total) // Range Exceed
    {
      COMPILE_WARNING("Range Exceed");
      return total;
    }

  if(dimlist_border >= dimlist.size()) // reaches the end
    {
      COMPILE_ERROR("You input tooooo many brackets!!!");
    }


  int sz = 1;
  for(int i = dimlist.size() - 1; i >= dimlist_border; i--)
    {
      sz *= dimlist[i];
    }
  int next = ROUNDUP((cur+1), sz);
  for(auto it : value->body)
    {
      if(typeid(*it) == typeid(arr_initvallist_t)) // The type is [arr_initvallist_t]
	{
	  cur = _set_correspond_array_initial_value(cur,
						    total,
						    (arr_initvallist_t*) it,
						    dimlist_border + 1,
						    dimlist,
						    setter,
						    dynamic_eval,
						    ctx);
	}
      else
	{
	  if(cur >= total) // Range Exceed
	    {
	      COMPILE_WARNING("Range Exceed");
	      return total;
	    }
	  else
	    {
	      if(!dynamic_eval)
		{
		  setter(cur++, ((expr_t*) it)->eval_compile(ctx, -1));
		}
	      else // dynamic eval
		{
		  assert(typeid(setter) == typeid(_local_array_setter_t));
		  auto re = ((expr_t*) it)->eval_runtime(ctx);
		  _local_array_setter_t* p = (_local_array_setter_t*) &setter;
		  p->instruction_list_pointer->push_back(re.second);
		  setter(cur++,re.first);
		}
	    }
	}
    }
  return std::max(next, ROUNDUP(cur, sz));
}


IR_t* _var_def_array_withinit_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  if(ctx.is_global_scope())
    // global scope, insert and alloc
    // initializer [must] be constant
    {
      // Symbol type [sym] processing
      sym_t sym(GLOBL_ARR(this->name.name.name), SYMTYPE_ARRAY);
      for(auto it : this->name.dimlist.body)
	{
	  sym.dimlist.push_back(it->eval_compile(ctx, -1));
	} // dimension list

      // Push into symbol table
      ctx.var_table_push(this->name.name.name, sym);

      // data area size
      int length = 4;
      for(auto it : sym.dimlist)
	{
	  length *= it;
	} 

      // allocate memory region
      IRList.push_back(new IRLabel_t(DATA_AREA_BEGIN,
					   sym.name
					   ));
      IRData_t* datap = new IRData_t(length);
      _global_array_setter_t setter;
      setter.data_pointer = datap;

      _set_correspond_array_initial_value(0, // index
					  length/4, // number of elems
					  &this->value, // array initval list
					  0, // dimension border
					  sym.dimlist, // dimension list
					  setter, // setter
					  false, // global-variable dynamic eval is [BANNED]
					  ctx);
      
      IRList.push_back(datap);
      IRList.push_back(new IRLabel_t(DATA_AREA_END));

      if(this->is_const)
	{
	  // DO NOTHING!!!
	  
	  // std::vector<int> vallist;
	  // setter.export_to_const_table(vallist);
	  // constsym_t csym(sym.dimlist, vallist);
	  // ctx.const_table_push(this->name.name.name, csym);
	}
    }
  else
    // local scope, insert a series of operations
    // initializer may [NOT] be constant
    // even if for [const] variables
    // in fact, the [const] qualifier has little usage in this condition
    {
      // symbol processing
      sym_t sym(LOCAL_ARR(std::to_string(ctx.get_unique_id())), SYMTYPE_ARRAY);
      for(auto it : this->name.dimlist.body)
	{
	  sym.dimlist.push_back(it->eval_compile(ctx, -1));
	} // no support for flexible array

      ctx.var_table_push(this->name.name.name, sym);


      // data area size
      int length = 4;
      for(auto it : sym.dimlist)
	{
	  length *= it;
	} 

      IR_t* irp = new IROper_t(OPER_STACK_ALLOC,
			       operand_t(ctx.var_table_find(this->name.name.name).name),
			       operand_t(length));
      IRList.push_back(irp);
      IRList.push_back(new IROper_t(OPER_SET_ARG, 2, operand_t(length)));
      IRList.push_back(new IROper_t(OPER_SET_ARG, 1, 0));
      IRList.push_back(new IROper_t(OPER_SET_ARG, 0, operand_t(ctx.var_table_find(this->name.name.name).name)));
      operand_t opd(LOCAL_VAR(std::to_string(ctx.get_unique_id())));
      IRList.push_back(new IROper_t(OPER_CALL, opd, std::string("memset")));

      // no data area, just instruction flow
      irp = new IRList_t;
      _local_array_setter_t setter;
      setter.tagged_name = ctx.var_table_find(this->name.name.name).name;
      setter.instruction_list_pointer = (IRList_t*) irp;

      setter.counts = length / 4;


      _set_correspond_array_initial_value(0, // index
					  length/4, // number of elems
					  &this->value, // array initval list
					  0, // dimension border
					  sym.dimlist, // dimension list
					  setter, // setter
					  true, // enable dynamic evaluation
					  ctx);

      IRList.push_back(irp);
      // TODO_MANUAL(); // Not tested yet
    }
  return &IRList;
}

IR_t* decl_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      IRList.push_back( (*p)->make_IR(ctx));
    }
  return &IRList;
}

IR_t* root_t::make_IR(context_t& ctx)
{
  this->_before_make_IR();
  IRList_t& IRList = *(new IRList_t);
  for(auto p = this->body.begin();
      p != this->body.end();
      ++p)
    {
      IRList.push_back((*p)->make_IR(ctx));
      // construct IR tree
    }
  return &IRList;
  // return current IRNode
}



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///////// B E F O R E ////////////////////////////////////////
/////////// M A K E //////////////////////////////////////////
/////////////////////////////////// I R //////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void ASTNode_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "ASTNode_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void expr_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "expr_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void ident_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "ident_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _cond_expr_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_cond_expr_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _binary_expr_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_binary_expr_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _unary_expr_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_unary_expr_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _func_call_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_func_call_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _number_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_number_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void arr_initvallist_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "arr_initvallist_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void arr_dimlist_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "arr_dimlist_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void func_real_arglist_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "func_real_arglist_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void func_formal_arg_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "func_formal_arg_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void func_formal_arglist_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "func_formal_arglist_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void arr_ident_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "arr_ident_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _assign_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_assign_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _eval_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_eval_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _block_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_block_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _if_noelse_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_if_noelse_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _if_withelse_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_if_withelse_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _while_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_while_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _break_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_break_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _continue_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_continue_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _return_novalue_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_return_novalue_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _return_withvalue_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_return_withvalue_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _empty_stmt_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_empty_stmt_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void block_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "block_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}


void func_def_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "func_def_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void var_def_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "var_def_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _var_def_one_noinit_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_var_def_one_noinit_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _var_def_one_withinit_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_var_def_one_withinit_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _var_def_array_noinit_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_var_def_array_noinit_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void _var_def_array_withinit_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "_var_def_array_withinit_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void decl_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "decl_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

void root_t::_before_make_IR()
{
  // std::cerr<<COLOR_YELLOW "[" << COLOR_GREEN << "root_t"
  // 	   << COLOR_YELLOW "]" << ": before make IR" << std::endl;
}

