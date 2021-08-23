/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_TOKENS_H_INCLUDED
# define YY_YY_TOKENS_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENTIFIER = 258,
    DEC_IMM = 259,
    OCT_IMM = 260,
    HEX_IMM = 261,
    IF = 262,
    ELSE = 263,
    WHILE = 264,
    BREAK = 265,
    CONTINUE = 266,
    RETURN = 267,
    PLUS = 268,
    MINUS = 269,
    MUL = 270,
    DIV = 271,
    MOD = 272,
    LAND = 273,
    LOR = 274,
    LNEG = 275,
    LT = 276,
    LE = 277,
    GT = 278,
    GE = 279,
    EQ = 280,
    NE = 281,
    ASSIGN = 282,
    LPAR = 283,
    RPAR = 284,
    LSQUARE = 285,
    RSQUARE = 286,
    LBRACE = 287,
    RBRACE = 288,
    COMMA = 289,
    SEMICOLON = 290,
    TYPE_INTEGER = 291,
    TYPE_VOID = 292,
    CONST = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 34 "parse.y" /* yacc.c:1909  */


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

#line 124 "tokens.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOKENS_H_INCLUDED  */
