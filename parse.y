/* SysC syntax definition */
%{
#include "debug.h"
#include "common.h"
#include "lex.h"
#include<iostream>
#include<typeinfo>
#include<stdio.h>
#include<sstream>
#include "AST.h"
using namespace std;
%}

%{
// #define YYLTYPE YYLTYPE
// typedef struct YYLTYPE
// {
//     int start_line;
//     int start_column;
//     int end_line;
//     int end_column;
//     char* filename;
// } YYLTYPE;
extern void yyerror(char const* s);

/* #include "syn_common.h" */
#define DEBUG
 
root_t* AST;
    
%}

%union{

    /// Basics
    token_t        token;           // Terminal
    root_t*        root;            // CompUnit
    ident_t*       ident;           // Identifier for [0-dims/func]
    expr_t*        expr;            // Expression, Literals and Logical
    literal_t*     literal;         // Literal Constants (lexical)

    decl_t*        decl;            // Declaration (Definition Lists)
    /// Variable Data Types
    var_def_t*         var_def;            // Variable Definition container
      /// array data types
      arr_ident_t*   arr_ident;   // Array Identifier and Dimlist Container
      arr_dimlist_t* arr_dimlist; // Array Dimension List
      arr_initvallist_t* arr_initvallist; // Array Initial Value List
	
    /// Function Data Types
      /// function definition
      func_def_t*            func_def;
      func_formal_arglist_t* func_formal_arglist;
      func_formal_arg_t*     func_formal_arg;
      
      /// function call
      func_real_arglist_t*   func_real_arglist;
      
      /// block data type
      block_t*       block;
      stmt_t*        stmt;
    // Literals
}

%expect 1
// If-Else shift/reduce conflict

%define parse.error verbose
// More precise verbose message

// Terminal Token Definitions
%token <literal> IDENTIFIER DEC_IMM OCT_IMM HEX_IMM
 // literal constants

%token <token>   IF ELSE WHILE BREAK CONTINUE RETURN
 // control-flow
 // ok
%token <token>   PLUS MINUS MUL DIV MOD
 // arith, '+', '-', '*', '/', '%'
 // ok
%token <token>   LAND LOR LNEG
 // logical operation, '&&', '||', '!'
 // ok
%token <token>   LT LE GT GE EQ NE
 // logical relation, '<', '<=', '>', '>=', '==', '!='
 // ok
%token <token>   ASSIGN
 // assignment, '='
 // ok
%token <token>   LPAR RPAR LSQUARE RSQUARE LBRACE RBRACE
 // parenthesises
 // ok
%token <token>   COMMA SEMICOLON
 // "," and ";"
 // ok
%token <token>   TYPE_INTEGER TYPE_VOID
 // "int" and "void"
 // ok
%token <token>   CONST
 // "const"
 // ok
 //***********************//
 // Layer 1
// Basic Nonterminal definitions

%type <token> BType
 // BType -> { TYPE_INTEGER }

%type <token> VType
 // Vtype -> { TYPE_VOID }

%type <ident> Ident
 // Ident -> { IDENTIFIER }

%type <token> UnaryClass
 // UnaryClass -> { PLUS, MINUS, LNEG }

%type <token> MulClass
 // MulClass -> { MUL, DIV, MOD }

%type <token> AddClass
 // AddClass -> { PLUS, MINUS }

%type <token> RelClass
 // RelClass -> { LT, LE, GT, GE }

%type <token> EqClass
 // EqClass -> { EQ, NE }

%type <expr> Number
 // Number -> { DEC_IMM, OCT_IMM, HEX_IMM }

//**********************//
 // Layer 2 Expression Evaluation

// Arithmetic Evaluation
%type <expr> Exp
 /* Exp -> { AddExp } */

%type <expr> AddExp
 /* AddExp -> { MulExp,
                AddExp AddClass MulExp } 
*/

%type <expr> MulExp
 /* MulExp -> { UnaryExp,
                MulExp MulClass UnaryExp }
  */

%type <expr> UnaryExp
 /* UnaryExp -> { PrimaryExp,
                  FuncCall,
                  UnaryClass UnaryExp }
  */

%type <expr> PrimaryExp
 /* PrimaryExp -> { LPAR Exp RPAR,
                    LVal,
		    Number }
  */

%type <expr> ConstExp
 /* ConstExp -> { AddExp }
  * WITH CONST CHECK
  */

// Logical Evaluation

%type <expr> Cond
 /* Cond -> { LOrExp } */

%type <expr> LOrExp
 /* LOrExp -> { LAndExp,
                LOrExp LOR LAndExp }
  */

%type <expr> LAndExp
 /* LAndExp -> { EqExp,
                 LAndExp LAND EqExp }
  */

%type <expr> EqExp
 /* EqExp -> { RelExp,
               EqExp EqClass RelExp }
  */

%type <expr> RelExp
 /* RelExp -> { AddExp,
                RelExp RelClass AddExp }
  */



//**********************//
 // Layer 3, Higher-end Evaluation & Definition & Statement Code Generation

 // Layer 3, Evaluation Part

// Variable Types Evaluation
%type <ident> LVal
 /* LVal -> { ArrayIdent,
              Ident }
  */

// Array Evaluation

%type <arr_ident> ArrayIdent
 /* ArrayIdent -> { Ident ArrayDimClosure }
 */

%type <arr_dimlist> ArrayDimClosure
 /* ArrayDimClosure -> { LSQUARE Exp RSQUARE,
                         ArrayDimClosure LSQUARE Exp RSQUARE }
 */

%type <arr_ident> ConstArrayIdent
 /* ConstArrayIdent -> { Ident ConstArrayDimClosure }
 */

%type <arr_dimlist> ConstArrayDimClosure
 /* ConstArrayDimClosure -> { LSQUARE ConstExp RSQUARE,
                              ConstArrayDimClosure LSQUARE ConstExp RSQUARE }
 */

// Function Call Evaluation

%type <expr> FuncCall
 /* FuncCall -> { Ident LPAR FuncRealArgClosure RPAR,
                  Ident LPAR RPAR }
 */

%type <func_real_arglist> FuncRealArgClosure
 /* FuncRealArgClosure -> { Exp,
			    FuncRealArgClosure COMMA Exp }
  */

//**********************//
 // Layer 3, Definition Part

// Variable Definition

%type <var_def> VarDef
 /* VarDef -> { VarDefOne, 
                VarDefArray } 
  */

// One-data Definition
%type <var_def> VarDefOne
 /* VarDefOne -> { Ident,
                   Ident ASSIGN InitValOne }
  */

%type <expr> InitValOne
 /* InitValOne -> { Exp } */

// Array-data Definition
%type <var_def> VarDefArray
 /* VarDefArray -> { ConstArrayIdent,
                     ConstArrayIdent ASSIGN InitValArray
  */

%type <arr_initvallist> InitValArray
 /* InitValArray -> { LBRACE RBRACE,
                      LBRACE InitValArrayArgClosure RBRACE }
  */

%type <arr_initvallist> InitValArrayArgClosure
 /* InitValArrayArgClosure -> { InitValOne,
                                InitValArray,
				InitValArrayArgClosure COMMA InitValOne
				InitValArrayArgClosure COMMA InitValArray
			      }
  */

// CONST variable definition

%type <var_def> ConstVarDef
 /* ConstVarDef -> { ConstVarDefOne,
                     ConstVarDefArray }
  */

%type <var_def> ConstVarDefOne
 /* ConstVarDefOne -> { Ident ASSIGN ConstInitValOne }
  */

%type <expr> ConstInitValOne
 /* ConstInitValOne -> { ConstExp } */

%type <var_def> ConstVarDefArray
 /* ConstVarDefArray -> { ConstArrayIdent ASSIGN ConstInitValArray }
  */

%type <arr_initvallist> ConstInitValArray
 /* ConstInitValArray -> { LBRACE RBRACE,
                           LBRACE ConstInitValArrayArgClosure RBRACE }
  */

%type <arr_initvallist> ConstInitValArrayArgClosure
 /* ConstInitValArrayArgClosure -> { ConstInitValOne,
                                     ConstInitValArray,
				     ConstInitValArrayArgClosure COMMA ConstInitValOne
				     ConstInitValArrayArgClosure COMMA ConstInitValArray
			           }
  */

// Block Definition

%type <block> Block
 /* Block -> { LBRACE RBRACE,
               LBRACE BlockItemClosure RBRACE }
  */

%type <block> BlockItemClosure
 /* BlockItemClosure -> { BlockItem,
                          BlockItemClosure BlockItem }
  */

%type <stmt> BlockItem
 /* BlockItem -> { Decl, Stmt } */


// Function Definition

%type <func_def> FuncDef
 /* FuncDef -> { FuncType Ident LPAR FuncFormalArgClosure RPAR Block,
                 FuncType Ident LPAR RPAR Block } */

%type <func_formal_arglist> FuncFormalArgClosure
 /* FuncFormalArgClosure -> { FuncFormalArg,
                              FuncFormalArgClosure COMMA FuncFormalArg }
  */

%type <func_formal_arg> FuncFormalArg
 /* FuncFormalArg -> { BType FuncFormalArgSingle, 
                       BType FuncFormalArgArray }
  */

%type <func_formal_arg> FuncFormalArgSingle
 /* FuncFormalArgSingle -> { Ident } */

%type <func_formal_arg> FuncFormalArgArray
 /* FuncFormalArgArray -> { Ident LSQUARE RSQUARE,
                            Ident LSQUARE RSQUARE ArrayDimClosure }
  */

//**********************//
 // Layer 3, Code Generation Part
%type <stmt> Stmt
 /* Stmt -> { StmtAssign
              StmtExp
	      StmtBlock
	      StmtIf
	      StmtWhile
	      StmtBreak
	      StmtContinue
	      StmtReturn
	      StmtEmpty
            }
    
  */
%type <stmt> StmtAssign
 /* StmtAssign -> { LVal ASSIGN Exp SEMICOLON } */

%type <stmt> StmtExp
 /* StmtExp -> { Exp SEMICOLON } */

%type <stmt> StmtBlock
 /* StmtBlock -> Block */

%type <stmt> StmtIf
 /* StmtIf -> { IF LPAR Cond RPAR Stmt
                IF LPAR Cond RPAR Stmt ELSE Stmt }
  */

%type <stmt> StmtWhile
 /* StmtWhile -> { WHILE LPAR Cond RPAR Stmt } */

%type <stmt> StmtBreak
 /* StmtBreak -> { BREAK SEMICOLON } */

%type <stmt> StmtContinue
 /* StmtContinue -> { CONTINUE SEMICOLON } */

%type <stmt> StmtReturn
 /* StmtReturn -> { RETURN SEMICOLON,
                    RETURN Exp SEMICOLON }
  */

%type <stmt> StmtEmpty
 /* StmtEmpty -> {}
  */

//**********************//
 // Layer 4, Variable Declaration (Definition List)

%type <decl> Decl
 /* Decl -> { VarDecl,
              ConstVarDecl }
  */

%type <decl> VarDecl
 /* VarDecl -> { BType VarDefClosure SEMICOLON }
  */

%type <decl> VarDefClosure
 /* VarDefClosure -> { VarDef,
                       VarDefClosure COMMA VarDef }
  */

%type <decl> ConstVarDecl
 /* ConstVarDecl -> { CONST BType ConstVarDefClosure SEMICOLON }
  */

%type <decl> ConstVarDefClosure
 /* ConstVarDefClosure -> { ConstVarDef,
                            ConstVarDefClosure COMMA ConstVarDef }
  */


//**********************//
 // Layer 5, CompUnit
%type <root> CompUnit
/* CompUnit -> { Decl,
                 FuncDef,
		 CompUnit Decl,
		 CompUnit FuncDef
	       }
*/

%start CompUnit
%%

//**********************//
// Layer 5
CompUnit:	Decl
	     //	[1]
		{

		    // Auto-generated by MACRO, no-use
		    AST = new root_t;
		    $$ = AST;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	FuncDef
	     //	[1]
		{

		    // Auto-generated by MACRO, no-use
		    AST = new root_t;
		    $$ = AST;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	CompUnit Decl
	     //	[1]      [2]
		{

		    // Auto-generated by MACRO, no-use
		    $$ = $1;
		    $$ -> body.push_back($2);

		    
		    
		    
		}
	|	CompUnit FuncDef
	     //	[1]      [2]
		{

		    // Auto-generated by MACRO, no-use
		    $$ = $1;
		    $$ -> body.push_back($2);

		    
		    
		    
		}
	;

//**********************//
// Layer 4
Decl:		VarDecl
	     // [1]
		{
		    $$ = $1;
		    // Auto-generated by MACRO
		    $$ = $1;

		    // propagate
		    for(auto p = $$->body.begin();
			p != $$->body.end();
			++p)
			{
			    if( typeid( *(*p)) == typeid(_var_def_one_withinit_t))
			    {
			      ((_var_def_one_withinit_t*)(*p))->is_const = false;
			    }
			  else if( typeid( *(*p)) == typeid(_var_def_array_withinit_t))
			    {
			      ((_var_def_array_withinit_t*)(*p))->is_const = false;
			    }
			  else
			    {
				assert( typeid( *(*p)) == typeid(_var_def_one_noinit_t) ||
					typeid( *(*p)) == typeid(_var_def_array_noinit_t));
			    }
			       
			}
		 
		    
		    
		    
		}
	|	ConstVarDecl
	     // [1]
		{
		    $$ = $1;
		    // Auto-generated by MACRO
		    $$ = $1;		    
		    
		    // propagate
		    for(auto p = $$->body.begin();
			p != $$->body.end();
			++p)
			{
			    if( typeid( *(*p)) == typeid(_var_def_one_withinit_t))
			    {
			      ((_var_def_one_withinit_t*)(*p))->is_const = true;
			    }
			  else if( typeid( *(*p)) == typeid(_var_def_array_withinit_t))
			    {
			      ((_var_def_array_withinit_t*)(*p))->is_const = true;
			    }
			  else
			    {
			      assert(0);
			    }
			       
			}
		    
		    
		    
		}
	;

VarDecl:	BType VarDefClosure SEMICOLON
	     //	[1]   [2]           [3]
		{

		    // Auto-generated by MACRO, no-use
		    $$ = $2;
		    $$ -> type = $1;
		    $$ -> is_const = false;
		    
		    
		    
		}
	;

VarDefClosure:	VarDef
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = new decl_t;
		    $$ -> body.push_back($1);
		    

		    
		    
		    
		}
	|	VarDefClosure COMMA VarDef
	     // [1]           [2]   [3]
		{

		    // Auto-generated by MACRO

		    $$ = $1;
		    $$ -> body.push_back($3);
		    

		    
		    
		    
		}
	;

ConstVarDecl:	CONST BType ConstVarDefClosure SEMICOLON
	     // [1]   [2]   [3]                [4]
		{

		    // Auto-generated by MACRO
		    $$ = $3;
		    $$ -> type = $2;
		    $$ -> is_const = true;
#ifdef DEBUG
		    for(auto p = $3->body.begin();
			p != $3->body.end();
			++p)
			{
			    std::stringstream ss;
			    assert(typeid(**p) == typeid(_var_def_one_withinit_t) ||
				   typeid(**p) == typeid(_var_def_array_withinit_t) );

			    if(typeid(**p) == typeid(_var_def_one_withinit_t))
			      {
				assert( ((_var_def_one_withinit_t*)(*p))->value.const_required == true);
			      }
			    else
			      {
				/* assert( ((_var_def_array_withinit_t*)(*p))->value.const_required == true); */
				  // skipped...
			      }
			}
#endif
		    
		    
		    
		}
	;

ConstVarDefClosure:
		ConstVarDef
	     // [1]
		{

		    // Auto-generated by MACRO

		    $$ = new decl_t;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	ConstVarDefClosure COMMA ConstVarDef
	     // [1]                [2]   [3]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	;

//**********************//

// Layer 3, Evaluation
LVal:		ArrayIdent
	     // [1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	Ident
	     // [1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	;

ArrayIdent:	Ident ArrayDimClosure
	     // [1]   [2]
		{

		    // Auto-generated by MACRO
		    $$ = new arr_ident_t(*($1),
					 *($2));
		    
		    
		    
		}
	;

ArrayDimClosure:
		LSQUARE Exp RSQUARE
	     // [1]     [2] [3]
		{

		    // Auto-generated by MACRO
		    $$ = new arr_dimlist_t;
		    $$ -> body.push_back($2);

		    
		    
		    
		}
	|	ArrayDimClosure LSQUARE Exp RSQUARE
	     // [1]             [2]     [3] [4]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	;

ConstArrayIdent:
		Ident ConstArrayDimClosure
	     //	[1]   [2]
		{

		    // Auto-generated by MACRO
		    $$ = new arr_ident_t(*($1),
					 *($2));
#ifdef DEBUG
		    for(auto p = $2->body.begin();
			p != $2->body.end();
			++p)
			{
			    assert( (*p) ->const_required == true);
			}
#endif
		    

		    
		    
		    
		}
	;

ConstArrayDimClosure:
		LSQUARE ConstExp RSQUARE
	     //	[1]     [2]      [3]
		{

		    // Auto-generated by MACRO
		    $$ = new arr_dimlist_t;
		    $$ -> body.push_back($2);

		    
		    
		    
		}
	|	ConstArrayDimClosure LSQUARE ConstExp RSQUARE
	     //	[1]                  [2]     [3]      [4]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    $$ -> body.push_back($3);
		    
		    
		    
		}
	;

FuncCall:	Ident LPAR FuncRealArgClosure RPAR
	     //	[1]   [2]  [3]                [4]
		{

		    // Auto-generated by MACRO
		    $$ = new _func_call_t(*($1),
					  *($3));
		    
		    
		    
		}
	|	Ident LPAR RPAR
	     //	[1]   [2]  [3]
		{

		    // Auto-generated by MACRO
		    $$ = new _func_call_t(*($1),
					  *(new func_real_arglist_t)
					  );

		    
		    
		    
		}
	;

FuncRealArgClosure:
		Exp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = new func_real_arglist_t;
		    $$ -> body.push_back( $1 );
		    

		    
		    
		    
		}
	|	FuncRealArgClosure COMMA Exp
	     //	[1]                [2]   [3]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    $$ -> body.push_back( $3 );
		    

		    
		    
		    
		}
	;

// Layer 3, Definition

// Variable Definition
VarDef:		VarDefOne
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    
		    
		    
		}
	|	VarDefArray
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	;

VarDefOne:	Ident
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse

		    $$ = new _var_def_one_noinit_t(*($1));
		    

		    
		    
		    
		}
	|	Ident ASSIGN InitValOne
	     //	[1]   [2]    [3]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new _var_def_one_withinit_t(*($1),
						     *($3));
				

		    
		    
		    
		}
	;

InitValOne:	Exp
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;

		    
		    
		    
		}
	;

VarDefArray:	ConstArrayIdent
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = new _var_def_array_noinit_t(*($1));

		    
		    
		    
		}
	|	ConstArrayIdent ASSIGN InitValArray
	     //	[1]             [2]    [3]
		{

		    // Auto-generated by MACRO

		    $$ = new _var_def_array_withinit_t(*($1),
						       *($3));
		    

		    
		    
		    
		}
	;

InitValArray:	LBRACE RBRACE
	     //	[1]    [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new arr_initvallist_t;

		}
	|	LBRACE InitValArrayArgClosure RBRACE
	     //	[1]    [2]                    [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $2;

		    
		    
		    
		}
	;

InitValArrayArgClosure:
		InitValOne
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new arr_initvallist_t;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	InitValArray
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new arr_initvallist_t;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	InitValArrayArgClosure COMMA InitValOne
	     //	[1]                    [2]   [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	|	InitValArrayArgClosure COMMA InitValArray
	     //	[1]                    [2]   [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	;

// Const Variable Definition
ConstVarDef:	ConstVarDefOne
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	ConstVarDefArray
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	;

ConstVarDefOne:	Ident ASSIGN ConstInitValOne
	     //	[1]   [2]    [3]
		{

		    // Auto-generated by MACRO
		    $$ = new _var_def_one_withinit_t(*($1),
						     *($3));
		    

		    
		    
		    
		}
	;

ConstInitValOne:
		ConstExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	;

ConstVarDefArray:
		ConstArrayIdent ASSIGN ConstInitValArray
	     //	[1]             [2]    [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _var_def_array_withinit_t(*($1),
						       *($3));

		    
		    
		    
		}
	;

ConstInitValArray:
		LBRACE RBRACE
	     //	[1]    [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new arr_initvallist_t;
		    
		    
		    
		    
		    
		}
	|	LBRACE ConstInitValArrayArgClosure RBRACE
	     //	[1]    [2]                         [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $2;

		    
		    
		    
		}
	;

ConstInitValArrayArgClosure:
		ConstInitValOne
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new arr_initvallist_t;
		    $$ -> body.push_back($1);


		    
		    
		    
		}
	|	ConstInitValArray
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = new arr_initvallist_t;
		    $$ -> body.push_back($1);
		    

		    
		    
		    
		}
	|	ConstInitValArrayArgClosure COMMA ConstInitValOne
	     //	[1]                         [2]   [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;
		    $$ -> body.push_back($3);


		    
		    
		    
		}
	|	ConstInitValArrayArgClosure COMMA ConstInitValArray
	     //	[1]                         [2]   [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	;


// Block Definition
Block:		LBRACE RBRACE
	     //	[1]    [2]
		{

		    // Auto-generated by MACRO

		    $$ = new block_t;
		    
		    
		    
		    
		}
	|	LBRACE BlockItemClosure RBRACE
	     //	[1]    [2]              [3]
		{

		    // Auto-generated by MACRO

		    $$ = $2;
		    
		    
		    
		    
		    
		}
	;

BlockItemClosure:
		BlockItem
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = new block_t;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	BlockItemClosure BlockItem
	     //	[1]              [2]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    $$ -> body.push_back($2);

		    
		    
		    
		}
	;

BlockItem:	Decl
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;

		    
		    
		    
		}
	|	Stmt
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;

		    
		    
		    
		}
	;

// Function Definition
FuncDef:	BType Ident LPAR FuncFormalArgClosure RPAR Block
	     // [1]   [2]   [3]  [4]                  [5]  [6]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new func_def_t($1,
					*($2),
					*($4),
					*($6));

		    
		    
		    
		}
	|	BType Ident LPAR RPAR Block
	     // [1]   [2]   [3]  [4]  [5]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new func_def_t($1,
					*($2),
					*(new func_formal_arglist_t),
					*($5));

		    
		    
		    
		}
	|	VType Ident LPAR FuncFormalArgClosure RPAR Block
	     // [1]   [2]   [3]  [4]                  [5]  [6]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new func_def_t($1,
					*($2),
					*($4),
					*($6));


		    
		    
		    
		}
	|	VType Ident LPAR RPAR Block
	     // [1]   [2]   [3]  [4]  [5]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new func_def_t($1,
					*($2),
					*(new func_formal_arglist_t),
					*($5));

		    
		    
		    
		}
	;

FuncFormalArgClosure:
		FuncFormalArg
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = new func_formal_arglist_t;
		    $$ -> body.push_back($1);

		    
		    
		    
		}
	|	FuncFormalArgClosure COMMA FuncFormalArg
	     //	[1]                  [2]   [3]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    $$ -> body.push_back($3);

		    
		    
		    
		}
	;

FuncFormalArg:	BType FuncFormalArgSingle
	     //	[1]   [2]
		{

		    // Auto-generated by MACRO
		    $$ = $2;
		    $$ -> arg_type = $1;

		    
		}
	|	BType FuncFormalArgArray
	     //	[1]   [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $2;
		    $$ -> arg_type = $1;
		    
		}
	;

FuncFormalArgSingle:
		Ident
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = new func_formal_arg_t(*($1));

		    
		    
		    
		}
	;

FuncFormalArgArray:
		Ident LSQUARE RSQUARE
	     //	[1]   [2]     [3]
		{

		    // Auto-generated by MACRO
		    auto diml = new arr_dimlist_t;
		    diml -> body.push_back(NULL);
		    // Reserved.
		    auto p = new arr_ident_t(*($1), *(diml));
		    $$ = new func_formal_arg_t(*p);

		    
		    
		    
		}
	|	Ident LSQUARE RSQUARE ArrayDimClosure
	     //	[1]   [2]     [3]     [4]
		{

		    // Auto-generated by MACRO

		    $4 -> body.insert( $4 -> body.begin(),
				       NULL);
		    auto p = new arr_ident_t(*($1), *($4));
		    $$ = new func_formal_arg_t(*p);

		    
		    
		    
		}
	;

// Layer 3, Code Generation Part

Stmt:		StmtAssign
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	StmtExp
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = $1;
		    

		    
		    
		    
		}
	|	StmtBlock
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = $1;
		    

		    
		    
		    
		}
	|	StmtIf
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;

		    
		    
		    
		}
	|	StmtWhile
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	StmtBreak
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	StmtContinue
	     // [1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	StmtReturn
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	StmtEmpty
		{
		    $$ = $1;
		}
	;

StmtAssign:	LVal ASSIGN Exp SEMICOLON
	     //	[1]  [2]    [3] [4]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _assign_stmt_t( *($1),
					     *($3) );
		    
		    
		    
		}
	;

StmtExp:	Exp SEMICOLON
	     //	[1] [2]
		{

		    // Auto-generated by MACRO, nouse
		    
		    $$ = new _eval_stmt_t(*($1));		    
		    

		    
		    
		    
		}
	;

StmtBlock:	Block
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse
		    
		    $$ = new _block_stmt_t(*($1));

		    
		    
		    
		}
	;

StmtIf:		IF LPAR Cond RPAR Stmt
	     //	[1][2]  [3]  [4]  [5]
		{

		    // Auto-generated by MACRO
		    $$ = new _if_noelse_stmt_t(*($3),
					       *($5));
		    
		    
		    
		}
	|	IF LPAR Cond RPAR Stmt ELSE Stmt
	     //	[1][2]  [3]  [4]  [5]  [6]  [7]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _if_withelse_stmt_t(*($3),
						 *($5),
						 *($7));
		    
		    
		    
		}
	;

StmtWhile:	WHILE LPAR Cond RPAR Stmt
	     //	[1]   [2]  [3]  [4]  [5]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _while_stmt_t(*($3), *($5));

		    
		    
		    
		}
	;

StmtBreak:	BREAK SEMICOLON
	     //	[1]   [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _break_stmt_t;

		    
		    
		    
		}
	;

StmtContinue:	CONTINUE SEMICOLON
	     //	[1]      [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _continue_stmt_t;

		    
		    
		    
		}
	;

StmtReturn:	RETURN SEMICOLON
	     //	[1]    [2]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _return_novalue_stmt_t;

		    
		    
		    
		}
	|	RETURN Exp SEMICOLON
	     //	[1]    [2] [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _return_withvalue_stmt_t(*($2));

		    
		    
		    
		}
	;

StmtEmpty:      SEMICOLON
		{
		    $$ = new _empty_stmt_t;
		}
// Layer 2

Exp:		AddExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	;

AddExp:		MulExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	AddExp AddClass MulExp
	     //	[1]    [2]      [3]
		{

		    // Auto-generated by MACRO
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));
		    
		    
		    
		    
		}
	;

MulExp:		UnaryExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	MulExp MulClass UnaryExp
	     //	[1]    [2]      [3]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));
		    
		    

		    
		    
		    
		}
	;

UnaryExp:	PrimaryExp
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;
		    

		    
		    
		    
		}
	|	FuncCall
	     //	[1]
		{

		    // Auto-generated by MACRO

		    $$ = $1;
		    

		    
		    
		    
		}
	|	UnaryClass UnaryExp
	     //	[1]        [2]
		{

		    // Auto-generated by MACRO

		    $$ = new _unary_expr_t($1,
					   *($2));
		    

		    
		    
		    
		}
	;

PrimaryExp:	LPAR Exp RPAR
	     //	[1]  [2] [3]
		{

		    // Auto-generated by MACRO
		    $$ = $2;
		    

		    
		    
		    
		}
	|	LVal
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    // Noted that
		    // expr_t PrimaryExp
		    // ident_t LVal
		    // expr_t -> {ident_t}

		    
		    
		    
		}
	|	Number
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;
		    // Set directly
		    
		    
		    
		}
	;

ConstExp:	AddExp
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse

		    $$ = $1;
		    $$ -> const_required = true;
		    

		    
		    
		    
		}
	;

Cond:		LOrExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;


		    
		    
		    
		}
	;

LOrExp:		LAndExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    
		    $$ = $1;

		    
		    
		    
		}
	|	LOrExp LOR LAndExp
	     //	[1]    [2] [3]
		{

		    // Auto-generated by MACRO
		    
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));

		    
		    
		    
		}
	;

LAndExp:	EqExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	LAndExp LAND EqExp
	     //	[1]     [2]  [3]
		{

		    // Auto-generated by MACRO
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));

		    
		    
		    
		}
	;

EqExp:		RelExp
	     //	[1]
		{

		    // Auto-generated by MACRO
		    $$ = $1;

		    
		    
		    
		}
	|	EqExp EqClass RelExp
	     //	[1]   [2]     [3]
		{

		    // Auto-generated by MACRO
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));

		    
		    
		    
		}
	;

RelExp:		AddExp
	     //	[1]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = $1;

		    
		    
		    
		}
	|	RelExp RelClass AddExp
	     //	[1]    [2]      [3]
		{

		    // Auto-generated by MACRO, nouse
		    $$ = new _binary_expr_t(*($1),
					    $2,
					    *($3));
		    
		    
		    
		}
	;

// Layer 1
BType:		TYPE_INTEGER
	;

VType:		TYPE_VOID
	;

Ident:		IDENTIFIER
	     //	[1]
		{
		    $$ = new ident_t(*($1));

		    
		    
		    
		}
	;

UnaryClass:	PLUS
	|	MINUS
	|	LNEG
	;

MulClass:	MUL
	|	DIV
	|	MOD
	;

AddClass:	PLUS
	|	MINUS
	;

RelClass:	LT
	|	LE
	|	GT
	|	GE
	;

EqClass:	EQ
	|	NE
	;

Number:		DEC_IMM
		{
		    long long tst;
		    $$ = new _number_t;
		    sscanf($1->c_str(),
			   "%lld", &tst);
		    (((_number_t*)$$) -> semantic_value) = (int)tst;
		    ((_number_t*)$$) -> radix = DEC;
		    $$ -> const_valid = CONST_TRUE;
		    // number literal is always constant
				    
		}
	|	OCT_IMM
		{
		    $$ = new _number_t;
		    sscanf($1->c_str(),
			   "%o",
			   &(((_number_t*)$$) -> semantic_value));
		    ((_number_t*)$$) -> radix = OCT;
		    $$ -> const_valid = CONST_TRUE;
		    // number literal is always constant
		}
	|	HEX_IMM
		{
		    $$ = new _number_t;
		    sscanf($1->c_str(),
			   "%x",
			   &(((_number_t*)$$) -> semantic_value));
		    ((_number_t*)$$) -> radix = HEX;
		    $$ -> const_valid = CONST_TRUE;
		    // number literal is always constant
		}
	;

