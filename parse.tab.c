/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "parse.y"

#include "debug.h"
#include "common.h"
#include "lex.h"
#include<iostream>
#include<typeinfo>
#include<stdio.h>
#include<sstream>
#include "AST.h"
using namespace std;
#line 14 "parse.y"

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
    

#line 100 "parse.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PARSE_H_INCLUDED
# define YY_YY_PARSE_H_INCLUDED
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
#line 33 "parse.y"


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

#line 223 "parse.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSE_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   301

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  127
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  202

#define YYUNDEFTOK  2
#define YYMAXUTOK   293


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   435,   435,   448,   461,   473,   489,   521,   553,   567,
     581,   597,   632,   645,   662,   674,   687,   701,   713,   729,
     753,   765,   778,   789,   805,   818,   836,   846,   859,   872,
     887,   901,   912,   928,   937,   952,   965,   978,   991,  1007,
    1018,  1032,  1048,  1063,  1079,  1092,  1107,  1121,  1135,  1149,
    1166,  1178,  1194,  1206,  1220,  1232,  1247,  1261,  1275,  1290,
    1307,  1319,  1333,  1343,  1356,  1371,  1386,  1405,  1417,  1429,
    1441,  1453,  1465,  1477,  1488,  1500,  1506,  1520,  1535,  1549,
    1560,  1575,  1589,  1603,  1617,  1629,  1643,  1649,  1663,  1674,
    1689,  1700,  1717,  1730,  1743,  1759,  1771,  1786,  1799,  1815,
    1829,  1841,  1857,  1868,  1883,  1894,  1909,  1920,  1935,  1938,
    1941,  1952,  1953,  1954,  1957,  1958,  1959,  1962,  1963,  1966,
    1967,  1968,  1969,  1972,  1973,  1976,  1988,  1998
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "DEC_IMM", "OCT_IMM",
  "HEX_IMM", "IF", "ELSE", "WHILE", "BREAK", "CONTINUE", "RETURN", "PLUS",
  "MINUS", "MUL", "DIV", "MOD", "LAND", "LOR", "LNEG", "LT", "LE", "GT",
  "GE", "EQ", "NE", "ASSIGN", "LPAR", "RPAR", "LSQUARE", "RSQUARE",
  "LBRACE", "RBRACE", "COMMA", "SEMICOLON", "TYPE_INTEGER", "TYPE_VOID",
  "CONST", "$accept", "CompUnit", "Decl", "VarDecl", "VarDefClosure",
  "ConstVarDecl", "ConstVarDefClosure", "LVal", "ArrayIdent",
  "ArrayDimClosure", "ConstArrayIdent", "ConstArrayDimClosure", "FuncCall",
  "FuncRealArgClosure", "VarDef", "VarDefOne", "InitValOne", "VarDefArray",
  "InitValArray", "InitValArrayArgClosure", "ConstVarDef",
  "ConstVarDefOne", "ConstInitValOne", "ConstVarDefArray",
  "ConstInitValArray", "ConstInitValArrayArgClosure", "Block",
  "BlockItemClosure", "BlockItem", "FuncDef", "FuncFormalArgClosure",
  "FuncFormalArg", "FuncFormalArgSingle", "FuncFormalArgArray", "Stmt",
  "StmtAssign", "StmtExp", "StmtBlock", "StmtIf", "StmtWhile", "StmtBreak",
  "StmtContinue", "StmtReturn", "StmtEmpty", "Exp", "AddExp", "MulExp",
  "UnaryExp", "PrimaryExp", "ConstExp", "Cond", "LOrExp", "LAndExp",
  "EqExp", "RelExp", "BType", "VType", "Ident", "UnaryClass", "MulClass",
  "AddClass", "RelClass", "EqClass", "Number", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

#define YYPACT_NINF (-144)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      63,  -144,  -144,     1,    14,  -144,  -144,  -144,  -144,    51,
      51,    51,  -144,  -144,  -144,  -144,    36,    16,  -144,  -144,
    -144,    -2,    30,    78,    54,  -144,  -144,  -144,    58,    51,
    -144,    93,    62,    -9,    62,    32,    -6,    51,  -144,   109,
      62,  -144,    84,    28,  -144,  -144,  -144,  -144,  -144,  -144,
    -144,    62,  -144,  -144,  -144,  -144,  -144,   114,   103,  -144,
    -144,    59,    62,  -144,   122,    40,  -144,    51,   114,    79,
      62,   122,    43,  -144,   218,  -144,  -144,  -144,  -144,  -144,
    -144,    96,    92,  -144,  -144,    62,  -144,  -144,  -144,    62,
     272,    62,   126,  -144,   133,  -144,   122,     1,  -144,  -144,
     128,  -144,   124,  -144,   122,  -144,  -144,  -144,    98,  -144,
     239,  -144,   103,  -144,  -144,    44,  -144,   129,    62,   131,
     134,   132,   135,    89,  -144,  -144,  -144,   136,  -144,   169,
    -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,
    -144,   142,    51,  -144,  -144,   153,  -144,  -144,  -144,   252,
    -144,  -144,  -144,    62,  -144,   154,    62,    62,  -144,  -144,
    -144,   151,    62,  -144,  -144,  -144,   157,  -144,  -144,  -144,
    -144,   114,   159,   145,   172,   125,    83,   162,  -144,   158,
     126,   205,    62,    62,  -144,  -144,    62,  -144,  -144,  -144,
    -144,    62,   205,  -144,   184,   172,   125,    83,   114,  -144,
     205,  -144
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,   108,   109,     0,     0,     2,     6,     7,     3,     0,
       0,     0,     1,     4,     5,   110,     0,    31,     9,    26,
      27,    28,     0,     0,     0,    12,    39,    40,     0,     0,
       8,     0,     0,     0,     0,    19,     0,     0,    11,     0,
       0,    10,    28,     0,    32,   125,   126,   127,   111,   112,
     113,     0,    96,    14,    93,    29,    30,    87,    88,    90,
      92,    15,     0,    97,     0,     0,    60,     0,    98,     0,
       0,     0,     0,    13,     0,    43,    41,    42,    33,    35,
      36,     0,     0,   117,   118,     0,   114,   115,   116,     0,
       0,     0,    16,    94,     0,    57,     0,     0,    62,    63,
      64,    20,     0,    59,     0,    44,    46,    47,     0,    34,
       0,    95,    89,    91,    23,     0,    24,     0,     0,     0,
       0,     0,     0,     0,    50,    86,    54,    96,    78,     0,
      52,    55,    67,    68,    69,    70,    71,    72,    73,    74,
      75,     0,     0,    56,    61,     0,    21,    58,    45,     0,
      37,    38,    22,     0,    17,     0,     0,     0,    82,    83,
      84,     0,     0,    51,    53,    77,    65,    48,    49,    25,
      18,   106,     0,    99,   100,   102,   104,     0,    85,     0,
      66,     0,     0,     0,   123,   124,     0,   119,   120,   121,
     122,     0,     0,    76,    79,   101,   103,   105,   107,    81,
       0,    80
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -144,  -144,    35,  -144,  -144,  -144,  -144,   -84,  -144,    29,
       7,  -144,  -144,  -144,   165,  -144,   -31,  -144,   -30,  -144,
     161,  -144,   -65,  -144,   -63,  -144,   -49,  -144,    67,   195,
     164,   106,  -144,  -144,  -143,  -144,  -144,  -144,  -144,  -144,
    -144,  -144,  -144,  -144,   -27,   -34,   121,   -43,  -144,   -17,
      56,  -144,    38,    45,    41,     4,  -144,    -8,  -144,  -144,
    -144,  -144,  -144,  -144
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,   126,     6,    16,     7,    23,    52,    53,    92,
      17,    35,    54,   115,    18,    19,    55,    20,    44,    81,
      25,    26,    76,    27,    75,   108,   128,   129,   130,     8,
      65,    66,    98,    99,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,    57,    58,    59,    60,    77,
     172,   173,   174,   175,   176,    67,    10,    61,    62,    89,
      85,   191,   186,    63
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      68,    21,    22,    28,     9,    56,    68,    11,     9,   106,
     127,   107,    79,    80,    12,    95,    56,    69,    24,    93,
      64,    42,   103,    71,    82,    32,    33,     1,    34,    28,
       1,    15,    45,    46,    47,     5,    68,     1,   194,    13,
      68,    48,    49,    31,    24,   127,   113,   143,    50,   199,
       1,     2,     3,   102,    15,   147,    51,   201,    36,   100,
      43,    78,    70,   116,   117,    15,    45,    46,    47,    96,
      29,    30,   104,   152,    97,    48,    49,    97,   153,   150,
     151,    39,    50,    56,   167,    40,   168,    90,    34,    91,
      51,   155,    15,    45,    46,    47,   161,   127,   142,     1,
       2,     3,    48,    49,   187,   188,   189,   190,   127,    50,
     101,    32,    37,    38,    34,    68,   127,    51,    86,    87,
      88,   111,   171,   171,   160,    43,   169,    83,    84,   109,
     110,   148,   149,   142,    42,   179,    15,    45,    46,    47,
     119,    74,   120,   121,   122,   123,    48,    49,   171,   171,
     184,   185,   171,    50,    94,   146,   118,   198,   145,   156,
     154,    51,   157,   162,   182,    94,   124,   158,   125,     1,
     159,     3,    15,    45,    46,    47,   119,   165,   120,   121,
     122,   123,    48,    49,   166,   170,   178,    91,   181,    50,
     183,   192,   200,   193,    41,   180,   164,    51,    73,    14,
      72,    94,   163,   144,   125,     1,   112,     3,    15,    45,
      46,    47,   119,   177,   120,   121,   122,   123,    48,    49,
     195,    15,    45,    46,    47,    50,     0,   197,   196,     0,
       0,    48,    49,    51,     0,     0,     0,    94,    50,     0,
     125,     0,    15,    45,    46,    47,    51,     0,     0,     0,
      74,   105,    48,    49,     0,    15,    45,    46,    47,    50,
       0,     0,     0,     0,     0,    48,    49,    51,     0,     0,
       0,    43,    50,     0,     0,    15,    45,    46,    47,     0,
      51,     0,     0,     0,    74,    48,    49,     0,     0,     0,
       0,     0,    50,     0,     0,     0,     0,     0,     0,     0,
      51,   114
};

static const yytype_int16 yycheck[] =
{
      34,     9,    10,    11,     0,    32,    40,     3,     4,    74,
      94,    74,    43,    43,     0,    64,    43,    34,    11,    62,
      29,    29,    71,    29,    51,    27,    28,    36,    30,    37,
      36,     3,     4,     5,     6,     0,    70,    36,   181,     4,
      74,    13,    14,    27,    37,   129,    89,    96,    20,   192,
      36,    37,    38,    70,     3,   104,    28,   200,    28,    67,
      32,    33,    30,    90,    91,     3,     4,     5,     6,    29,
      34,    35,    29,    29,    34,    13,    14,    34,    34,   110,
     110,    27,    20,   110,   149,    27,   149,    28,    30,    30,
      28,   118,     3,     4,     5,     6,   123,   181,    94,    36,
      37,    38,    13,    14,    21,    22,    23,    24,   192,    20,
      31,    27,    34,    35,    30,   149,   200,    28,    15,    16,
      17,    29,   156,   157,    35,    32,   153,    13,    14,    33,
      34,    33,    34,   129,   142,   162,     3,     4,     5,     6,
       7,    32,     9,    10,    11,    12,    13,    14,   182,   183,
      25,    26,   186,    20,    32,    31,    30,   191,    30,    28,
      31,    28,    28,    27,    19,    32,    33,    35,    35,    36,
      35,    38,     3,     4,     5,     6,     7,    35,     9,    10,
      11,    12,    13,    14,    31,    31,    35,    30,    29,    20,
      18,    29,     8,    35,    29,   166,   129,    28,    37,     4,
      36,    32,    33,    97,    35,    36,    85,    38,     3,     4,
       5,     6,     7,   157,     9,    10,    11,    12,    13,    14,
     182,     3,     4,     5,     6,    20,    -1,   186,   183,    -1,
      -1,    13,    14,    28,    -1,    -1,    -1,    32,    20,    -1,
      35,    -1,     3,     4,     5,     6,    28,    -1,    -1,    -1,
      32,    33,    13,    14,    -1,     3,     4,     5,     6,    20,
      -1,    -1,    -1,    -1,    -1,    13,    14,    28,    -1,    -1,
      -1,    32,    20,    -1,    -1,     3,     4,     5,     6,    -1,
      28,    -1,    -1,    -1,    32,    13,    14,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      28,    29
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    36,    37,    38,    40,    41,    42,    44,    68,    94,
      95,    94,     0,    41,    68,     3,    43,    49,    53,    54,
      56,    96,    96,    45,    49,    59,    60,    62,    96,    34,
      35,    27,    27,    28,    30,    50,    28,    34,    35,    27,
      27,    53,    96,    32,    57,     4,     5,     6,    13,    14,
      20,    28,    46,    47,    51,    55,    83,    84,    85,    86,
      87,    96,    97,   102,    29,    69,    70,    94,    84,    88,
      30,    29,    69,    59,    32,    63,    61,    88,    33,    55,
      57,    58,    83,    13,    14,    99,    15,    16,    17,    98,
      28,    30,    48,    86,    32,    65,    29,    34,    71,    72,
      96,    31,    88,    65,    29,    33,    61,    63,    64,    33,
      34,    29,    85,    86,    29,    52,    83,    83,    30,     7,
       9,    10,    11,    12,    33,    35,    41,    46,    65,    66,
      67,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    94,    65,    70,    30,    31,    65,    33,    34,
      55,    57,    29,    34,    31,    83,    28,    28,    35,    35,
      35,    83,    27,    33,    67,    35,    31,    61,    63,    83,
      31,    84,    89,    90,    91,    92,    93,    89,    35,    83,
      48,    29,    19,    18,    25,    26,   101,    21,    22,    23,
      24,   100,    29,    35,    73,    91,    92,    93,    84,    73,
       8,    73
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    39,    40,    40,    40,    40,    41,    41,    42,    43,
      43,    44,    45,    45,    46,    46,    47,    48,    48,    49,
      50,    50,    51,    51,    52,    52,    53,    53,    54,    54,
      55,    56,    56,    57,    57,    58,    58,    58,    58,    59,
      59,    60,    61,    62,    63,    63,    64,    64,    64,    64,
      65,    65,    66,    66,    67,    67,    68,    68,    68,    68,
      69,    69,    70,    70,    71,    72,    72,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    74,    75,    76,    77,
      77,    78,    79,    80,    81,    81,    82,    83,    84,    84,
      85,    85,    86,    86,    86,    87,    87,    87,    88,    89,
      90,    90,    91,    91,    92,    92,    93,    93,    94,    95,
      96,    97,    97,    97,    98,    98,    98,    99,    99,   100,
     100,   100,   100,   101,   101,   102,   102,   102
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     1,     1,     3,     1,
       3,     4,     1,     3,     1,     1,     2,     3,     4,     2,
       3,     4,     4,     3,     1,     3,     1,     1,     1,     3,
       1,     1,     3,     2,     3,     1,     1,     3,     3,     1,
       1,     3,     1,     3,     2,     3,     1,     1,     3,     3,
       2,     3,     1,     2,     1,     1,     6,     5,     6,     5,
       1,     3,     2,     2,     1,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     2,     1,     5,
       7,     5,     2,     2,     2,     3,     1,     1,     1,     3,
       1,     3,     1,     1,     2,     3,     1,     1,     1,     1,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 437 "parse.y"
                {

		    // Auto-generated by MACRO, no-use
		    AST = new root_t;
		    (yyval.root) = AST;
		    (yyval.root) -> body.push_back((yyvsp[0].decl));

		    
		    
		    
		}
#line 1587 "parse.tab.c"
    break;

  case 3:
#line 450 "parse.y"
                {

		    // Auto-generated by MACRO, no-use
		    AST = new root_t;
		    (yyval.root) = AST;
		    (yyval.root) -> body.push_back((yyvsp[0].func_def));

		    
		    
		    
		}
#line 1603 "parse.tab.c"
    break;

  case 4:
#line 463 "parse.y"
                {

		    // Auto-generated by MACRO, no-use
		    (yyval.root) = (yyvsp[-1].root);
		    (yyval.root) -> body.push_back((yyvsp[0].decl));

		    
		    
		    
		}
#line 1618 "parse.tab.c"
    break;

  case 5:
#line 475 "parse.y"
                {

		    // Auto-generated by MACRO, no-use
		    (yyval.root) = (yyvsp[-1].root);
		    (yyval.root) -> body.push_back((yyvsp[0].func_def));

		    
		    
		    
		}
#line 1633 "parse.tab.c"
    break;

  case 6:
#line 491 "parse.y"
                {
		    (yyval.decl) = (yyvsp[0].decl);
		    // Auto-generated by MACRO
		    (yyval.decl) = (yyvsp[0].decl);

		    // propagate
		    for(auto p = (yyval.decl)->body.begin();
			p != (yyval.decl)->body.end();
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
#line 1668 "parse.tab.c"
    break;

  case 7:
#line 523 "parse.y"
                {
		    (yyval.decl) = (yyvsp[0].decl);
		    // Auto-generated by MACRO
		    (yyval.decl) = (yyvsp[0].decl);		    
		    
		    // propagate
		    for(auto p = (yyval.decl)->body.begin();
			p != (yyval.decl)->body.end();
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
#line 1701 "parse.tab.c"
    break;

  case 8:
#line 555 "parse.y"
                {

		    // Auto-generated by MACRO, no-use
		    (yyval.decl) = (yyvsp[-1].decl);
		    (yyval.decl) -> type = (yyvsp[-2].token);
		    (yyval.decl) -> is_const = false;
		    
		    
		    
		}
#line 1716 "parse.tab.c"
    break;

  case 9:
#line 569 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.decl) = new decl_t;
		    (yyval.decl) -> body.push_back((yyvsp[0].var_def));
		    

		    
		    
		    
		}
#line 1733 "parse.tab.c"
    break;

  case 10:
#line 583 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.decl) = (yyvsp[-2].decl);
		    (yyval.decl) -> body.push_back((yyvsp[0].var_def));
		    

		    
		    
		    
		}
#line 1750 "parse.tab.c"
    break;

  case 11:
#line 599 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.decl) = (yyvsp[-1].decl);
		    (yyval.decl) -> type = (yyvsp[-2].token);
		    (yyval.decl) -> is_const = true;
#ifdef DEBUG
		    for(auto p = (yyvsp[-1].decl)->body.begin();
			p != (yyvsp[-1].decl)->body.end();
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
#line 1785 "parse.tab.c"
    break;

  case 12:
#line 634 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.decl) = new decl_t;
		    (yyval.decl) -> body.push_back((yyvsp[0].var_def));

		    
		    
		    
		}
#line 1801 "parse.tab.c"
    break;

  case 13:
#line 647 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.decl) = (yyvsp[-2].decl);
		    (yyval.decl) -> body.push_back((yyvsp[0].var_def));

		    
		    
		    
		}
#line 1816 "parse.tab.c"
    break;

  case 14:
#line 664 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.ident) = (yyvsp[0].arr_ident);

		    
		    
		    
		}
#line 1831 "parse.tab.c"
    break;

  case 15:
#line 676 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.ident) = (yyvsp[0].ident);

		    
		    
		    
		}
#line 1845 "parse.tab.c"
    break;

  case 16:
#line 689 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.arr_ident) = new arr_ident_t(*((yyvsp[-1].ident)),
					 *((yyvsp[0].arr_dimlist)));
		    
		    
		    
		}
#line 1859 "parse.tab.c"
    break;

  case 17:
#line 703 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.arr_dimlist) = new arr_dimlist_t;
		    (yyval.arr_dimlist) -> body.push_back((yyvsp[-1].expr));

		    
		    
		    
		}
#line 1874 "parse.tab.c"
    break;

  case 18:
#line 715 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_dimlist) = (yyvsp[-3].arr_dimlist);
		    (yyval.arr_dimlist) -> body.push_back((yyvsp[-1].expr));

		    
		    
		    
		}
#line 1890 "parse.tab.c"
    break;

  case 19:
#line 731 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.arr_ident) = new arr_ident_t(*((yyvsp[-1].ident)),
					 *((yyvsp[0].arr_dimlist)));
#ifdef DEBUG
		    for(auto p = (yyvsp[0].arr_dimlist)->body.begin();
			p != (yyvsp[0].arr_dimlist)->body.end();
			++p)
			{
			    assert( (*p) ->const_required == true);
			}
#endif
		    

		    
		    
		    
		}
#line 1914 "parse.tab.c"
    break;

  case 20:
#line 755 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.arr_dimlist) = new arr_dimlist_t;
		    (yyval.arr_dimlist) -> body.push_back((yyvsp[-1].expr));

		    
		    
		    
		}
#line 1929 "parse.tab.c"
    break;

  case 21:
#line 767 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.arr_dimlist) = (yyvsp[-3].arr_dimlist);
		    (yyval.arr_dimlist) -> body.push_back((yyvsp[-1].expr));
		    
		    
		    
		}
#line 1943 "parse.tab.c"
    break;

  case 22:
#line 780 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = new _func_call_t(*((yyvsp[-3].ident)),
					  *((yyvsp[-1].func_real_arglist)));
		    
		    
		    
		}
#line 1957 "parse.tab.c"
    break;

  case 23:
#line 791 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = new _func_call_t(*((yyvsp[-2].ident)),
					  *(new func_real_arglist_t)
					  );

		    
		    
		    
		}
#line 1973 "parse.tab.c"
    break;

  case 24:
#line 807 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.func_real_arglist) = new func_real_arglist_t;
		    (yyval.func_real_arglist) -> body.push_back( (yyvsp[0].expr) );
		    

		    
		    
		    
		}
#line 1989 "parse.tab.c"
    break;

  case 25:
#line 820 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.func_real_arglist) = (yyvsp[-2].func_real_arglist);
		    (yyval.func_real_arglist) -> body.push_back( (yyvsp[0].expr) );
		    

		    
		    
		    
		}
#line 2005 "parse.tab.c"
    break;

  case 26:
#line 838 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.var_def) = (yyvsp[0].var_def);
		    
		    
		    
		}
#line 2018 "parse.tab.c"
    break;

  case 27:
#line 848 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.var_def) = (yyvsp[0].var_def);

		    
		    
		    
		}
#line 2032 "parse.tab.c"
    break;

  case 28:
#line 861 "parse.y"
                {

		    // Auto-generated by MACRO, nouse

		    (yyval.var_def) = new _var_def_one_noinit_t(*((yyvsp[0].ident)));
		    

		    
		    
		    
		}
#line 2048 "parse.tab.c"
    break;

  case 29:
#line 874 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.var_def) = new _var_def_one_withinit_t(*((yyvsp[-2].ident)),
						     *((yyvsp[0].expr)));
				

		    
		    
		    
		}
#line 2064 "parse.tab.c"
    break;

  case 30:
#line 889 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 2079 "parse.tab.c"
    break;

  case 31:
#line 903 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.var_def) = new _var_def_array_noinit_t(*((yyvsp[0].arr_ident)));

		    
		    
		    
		}
#line 2093 "parse.tab.c"
    break;

  case 32:
#line 914 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.var_def) = new _var_def_array_withinit_t(*((yyvsp[-2].arr_ident)),
						       *((yyvsp[0].arr_initvallist)));
		    

		    
		    
		    
		}
#line 2110 "parse.tab.c"
    break;

  case 33:
#line 930 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = new arr_initvallist_t;

		}
#line 2122 "parse.tab.c"
    break;

  case 34:
#line 939 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-1].arr_initvallist);

		    
		    
		    
		}
#line 2137 "parse.tab.c"
    break;

  case 35:
#line 954 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = new arr_initvallist_t;
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].expr));

		    
		    
		    
		}
#line 2153 "parse.tab.c"
    break;

  case 36:
#line 967 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = new arr_initvallist_t;
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].arr_initvallist));

		    
		    
		    
		}
#line 2169 "parse.tab.c"
    break;

  case 37:
#line 980 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-2].arr_initvallist);
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].expr));

		    
		    
		    
		}
#line 2185 "parse.tab.c"
    break;

  case 38:
#line 993 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-2].arr_initvallist);
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].arr_initvallist));

		    
		    
		    
		}
#line 2201 "parse.tab.c"
    break;

  case 39:
#line 1009 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.var_def) = (yyvsp[0].var_def);

		    
		    
		    
		}
#line 2215 "parse.tab.c"
    break;

  case 40:
#line 1020 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.var_def) = (yyvsp[0].var_def);

		    
		    
		    
		}
#line 2230 "parse.tab.c"
    break;

  case 41:
#line 1034 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.var_def) = new _var_def_one_withinit_t(*((yyvsp[-2].ident)),
						     *((yyvsp[0].expr)));
		    

		    
		    
		    
		}
#line 2246 "parse.tab.c"
    break;

  case 42:
#line 1050 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 2261 "parse.tab.c"
    break;

  case 43:
#line 1065 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.var_def) = new _var_def_array_withinit_t(*((yyvsp[-2].arr_ident)),
						       *((yyvsp[0].arr_initvallist)));

		    
		    
		    
		}
#line 2277 "parse.tab.c"
    break;

  case 44:
#line 1081 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = new arr_initvallist_t;
		    
		    
		    
		    
		    
		}
#line 2293 "parse.tab.c"
    break;

  case 45:
#line 1094 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-1].arr_initvallist);

		    
		    
		    
		}
#line 2308 "parse.tab.c"
    break;

  case 46:
#line 1109 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = new arr_initvallist_t;
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].expr));


		    
		    
		    
		}
#line 2325 "parse.tab.c"
    break;

  case 47:
#line 1123 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.arr_initvallist) = new arr_initvallist_t;
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].arr_initvallist));
		    

		    
		    
		    
		}
#line 2342 "parse.tab.c"
    break;

  case 48:
#line 1137 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-2].arr_initvallist);
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].expr));


		    
		    
		    
		}
#line 2359 "parse.tab.c"
    break;

  case 49:
#line 1151 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.arr_initvallist) = (yyvsp[-2].arr_initvallist);
		    (yyval.arr_initvallist) -> body.push_back((yyvsp[0].arr_initvallist));

		    
		    
		    
		}
#line 2375 "parse.tab.c"
    break;

  case 50:
#line 1168 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.block) = new block_t;
		    
		    
		    
		    
		}
#line 2390 "parse.tab.c"
    break;

  case 51:
#line 1180 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.block) = (yyvsp[-1].block);
		    
		    
		    
		    
		    
		}
#line 2406 "parse.tab.c"
    break;

  case 52:
#line 1196 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.block) = new block_t;
		    (yyval.block) -> body.push_back((yyvsp[0].stmt));

		    
		    
		    
		}
#line 2421 "parse.tab.c"
    break;

  case 53:
#line 1208 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.block) = (yyvsp[-1].block);
		    (yyval.block) -> body.push_back((yyvsp[0].stmt));

		    
		    
		    
		}
#line 2436 "parse.tab.c"
    break;

  case 54:
#line 1222 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.stmt) = (yyvsp[0].decl);

		    
		    
		    
		}
#line 2451 "parse.tab.c"
    break;

  case 55:
#line 1234 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2466 "parse.tab.c"
    break;

  case 56:
#line 1249 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.func_def) = new func_def_t((yyvsp[-5].token),
					*((yyvsp[-4].ident)),
					*((yyvsp[-2].func_formal_arglist)),
					*((yyvsp[0].block)));

		    
		    
		    
		}
#line 2483 "parse.tab.c"
    break;

  case 57:
#line 1263 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.func_def) = new func_def_t((yyvsp[-4].token),
					*((yyvsp[-3].ident)),
					*(new func_formal_arglist_t),
					*((yyvsp[0].block)));

		    
		    
		    
		}
#line 2500 "parse.tab.c"
    break;

  case 58:
#line 1277 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.func_def) = new func_def_t((yyvsp[-5].token),
					*((yyvsp[-4].ident)),
					*((yyvsp[-2].func_formal_arglist)),
					*((yyvsp[0].block)));


		    
		    
		    
		}
#line 2518 "parse.tab.c"
    break;

  case 59:
#line 1292 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.func_def) = new func_def_t((yyvsp[-4].token),
					*((yyvsp[-3].ident)),
					*(new func_formal_arglist_t),
					*((yyvsp[0].block)));

		    
		    
		    
		}
#line 2535 "parse.tab.c"
    break;

  case 60:
#line 1309 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.func_formal_arglist) = new func_formal_arglist_t;
		    (yyval.func_formal_arglist) -> body.push_back((yyvsp[0].func_formal_arg));

		    
		    
		    
		}
#line 2550 "parse.tab.c"
    break;

  case 61:
#line 1321 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.func_formal_arglist) = (yyvsp[-2].func_formal_arglist);
		    (yyval.func_formal_arglist) -> body.push_back((yyvsp[0].func_formal_arg));

		    
		    
		    
		}
#line 2565 "parse.tab.c"
    break;

  case 62:
#line 1335 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.func_formal_arg) = (yyvsp[0].func_formal_arg);
		    (yyval.func_formal_arg) -> arg_type = (yyvsp[-1].token);

		    
		}
#line 2578 "parse.tab.c"
    break;

  case 63:
#line 1345 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.func_formal_arg) = (yyvsp[0].func_formal_arg);
		    (yyval.func_formal_arg) -> arg_type = (yyvsp[-1].token);
		    
		}
#line 2591 "parse.tab.c"
    break;

  case 64:
#line 1358 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.func_formal_arg) = new func_formal_arg_t(*((yyvsp[0].ident)));

		    
		    
		    
		}
#line 2606 "parse.tab.c"
    break;

  case 65:
#line 1373 "parse.y"
                {

		    // Auto-generated by MACRO
		    auto diml = new arr_dimlist_t;
		    diml -> body.push_back(NULL);
		    // Reserved.
		    auto p = new arr_ident_t(*((yyvsp[-2].ident)), *(diml));
		    (yyval.func_formal_arg) = new func_formal_arg_t(*p);

		    
		    
		    
		}
#line 2624 "parse.tab.c"
    break;

  case 66:
#line 1388 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyvsp[0].arr_dimlist) -> body.insert( (yyvsp[0].arr_dimlist) -> body.begin(),
				       NULL);
		    auto p = new arr_ident_t(*((yyvsp[-3].ident)), *((yyvsp[0].arr_dimlist)));
		    (yyval.func_formal_arg) = new func_formal_arg_t(*p);

		    
		    
		    
		}
#line 2642 "parse.tab.c"
    break;

  case 67:
#line 1407 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2657 "parse.tab.c"
    break;

  case 68:
#line 1419 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.stmt) = (yyvsp[0].stmt);
		    

		    
		    
		    
		}
#line 2672 "parse.tab.c"
    break;

  case 69:
#line 1431 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.stmt) = (yyvsp[0].stmt);
		    

		    
		    
		    
		}
#line 2687 "parse.tab.c"
    break;

  case 70:
#line 1443 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2702 "parse.tab.c"
    break;

  case 71:
#line 1455 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2717 "parse.tab.c"
    break;

  case 72:
#line 1467 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2732 "parse.tab.c"
    break;

  case 73:
#line 1479 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2746 "parse.tab.c"
    break;

  case 74:
#line 1490 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = (yyvsp[0].stmt);

		    
		    
		    
		}
#line 2761 "parse.tab.c"
    break;

  case 75:
#line 1501 "parse.y"
                {
		    (yyval.stmt) = (yyvsp[0].stmt);
		}
#line 2769 "parse.tab.c"
    break;

  case 76:
#line 1508 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _assign_stmt_t( *((yyvsp[-3].ident)),
					     *((yyvsp[-1].expr)) );
		    
		    
		    
		}
#line 2784 "parse.tab.c"
    break;

  case 77:
#line 1522 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    
		    (yyval.stmt) = new _eval_stmt_t(*((yyvsp[-1].expr)));		    
		    

		    
		    
		    
		}
#line 2800 "parse.tab.c"
    break;

  case 78:
#line 1537 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    
		    (yyval.stmt) = new _block_stmt_t(*((yyvsp[0].block)));

		    
		    
		    
		}
#line 2815 "parse.tab.c"
    break;

  case 79:
#line 1551 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.stmt) = new _if_noelse_stmt_t(*((yyvsp[-2].expr)),
					       *((yyvsp[0].stmt)));
		    
		    
		    
		}
#line 2829 "parse.tab.c"
    break;

  case 80:
#line 1562 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _if_withelse_stmt_t(*((yyvsp[-4].expr)),
						 *((yyvsp[-2].stmt)),
						 *((yyvsp[0].stmt)));
		    
		    
		    
		}
#line 2845 "parse.tab.c"
    break;

  case 81:
#line 1577 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _while_stmt_t(*((yyvsp[-2].expr)), *((yyvsp[0].stmt)));

		    
		    
		    
		}
#line 2860 "parse.tab.c"
    break;

  case 82:
#line 1591 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _break_stmt_t;

		    
		    
		    
		}
#line 2875 "parse.tab.c"
    break;

  case 83:
#line 1605 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _continue_stmt_t;

		    
		    
		    
		}
#line 2890 "parse.tab.c"
    break;

  case 84:
#line 1619 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _return_novalue_stmt_t;

		    
		    
		    
		}
#line 2905 "parse.tab.c"
    break;

  case 85:
#line 1631 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.stmt) = new _return_withvalue_stmt_t(*((yyvsp[-1].expr)));

		    
		    
		    
		}
#line 2920 "parse.tab.c"
    break;

  case 86:
#line 1644 "parse.y"
                {
		    (yyval.stmt) = new _empty_stmt_t;
		}
#line 2928 "parse.tab.c"
    break;

  case 87:
#line 1651 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 2943 "parse.tab.c"
    break;

  case 88:
#line 1665 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 2957 "parse.tab.c"
    break;

  case 89:
#line 1676 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));
		    
		    
		    
		    
		}
#line 2973 "parse.tab.c"
    break;

  case 90:
#line 1691 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 2987 "parse.tab.c"
    break;

  case 91:
#line 1702 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));
		    
		    

		    
		    
		    
		}
#line 3005 "parse.tab.c"
    break;

  case 92:
#line 1719 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.expr) = (yyvsp[0].expr);
		    

		    
		    
		    
		}
#line 3021 "parse.tab.c"
    break;

  case 93:
#line 1732 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.expr) = (yyvsp[0].expr);
		    

		    
		    
		    
		}
#line 3037 "parse.tab.c"
    break;

  case 94:
#line 1745 "parse.y"
                {

		    // Auto-generated by MACRO

		    (yyval.expr) = new _unary_expr_t((yyvsp[-1].token),
					   *((yyvsp[0].expr)));
		    

		    
		    
		    
		}
#line 3054 "parse.tab.c"
    break;

  case 95:
#line 1761 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[-1].expr);
		    

		    
		    
		    
		}
#line 3069 "parse.tab.c"
    break;

  case 96:
#line 1773 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].ident);
		    // Noted that
		    // expr_t PrimaryExp
		    // ident_t LVal
		    // expr_t -> {ident_t}

		    
		    
		    
		}
#line 3087 "parse.tab.c"
    break;

  case 97:
#line 1788 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);
		    // Set directly
		    
		    
		    
		}
#line 3101 "parse.tab.c"
    break;

  case 98:
#line 1801 "parse.y"
                {

		    // Auto-generated by MACRO, nouse

		    (yyval.expr) = (yyvsp[0].expr);
		    (yyval.expr) -> const_required = true;
		    

		    
		    
		    
		}
#line 3118 "parse.tab.c"
    break;

  case 99:
#line 1817 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);


		    
		    
		    
		}
#line 3133 "parse.tab.c"
    break;

  case 100:
#line 1831 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 3148 "parse.tab.c"
    break;

  case 101:
#line 1843 "parse.y"
                {

		    // Auto-generated by MACRO
		    
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));

		    
		    
		    
		}
#line 3165 "parse.tab.c"
    break;

  case 102:
#line 1859 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 3179 "parse.tab.c"
    break;

  case 103:
#line 1870 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));

		    
		    
		    
		}
#line 3195 "parse.tab.c"
    break;

  case 104:
#line 1885 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 3209 "parse.tab.c"
    break;

  case 105:
#line 1896 "parse.y"
                {

		    // Auto-generated by MACRO
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));

		    
		    
		    
		}
#line 3225 "parse.tab.c"
    break;

  case 106:
#line 1911 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.expr) = (yyvsp[0].expr);

		    
		    
		    
		}
#line 3239 "parse.tab.c"
    break;

  case 107:
#line 1922 "parse.y"
                {

		    // Auto-generated by MACRO, nouse
		    (yyval.expr) = new _binary_expr_t(*((yyvsp[-2].expr)),
					    (yyvsp[-1].token),
					    *((yyvsp[0].expr)));
		    
		    
		    
		}
#line 3254 "parse.tab.c"
    break;

  case 110:
#line 1943 "parse.y"
                {
		    (yyval.ident) = new ident_t(*((yyvsp[0].literal)));

		    
		    
		    
		}
#line 3266 "parse.tab.c"
    break;

  case 125:
#line 1977 "parse.y"
                {
		    long long tst;
		    (yyval.expr) = new _number_t;
		    sscanf((yyvsp[0].literal)->c_str(),
			   "%lld", &tst);
		    (((_number_t*)(yyval.expr)) -> semantic_value) = (int)tst;
		    ((_number_t*)(yyval.expr)) -> radix = DEC;
		    (yyval.expr) -> const_valid = CONST_TRUE;
		    // number literal is always constant
				    
		}
#line 3282 "parse.tab.c"
    break;

  case 126:
#line 1989 "parse.y"
                {
		    (yyval.expr) = new _number_t;
		    sscanf((yyvsp[0].literal)->c_str(),
			   "%o",
			   &(((_number_t*)(yyval.expr)) -> semantic_value));
		    ((_number_t*)(yyval.expr)) -> radix = OCT;
		    (yyval.expr) -> const_valid = CONST_TRUE;
		    // number literal is always constant
		}
#line 3296 "parse.tab.c"
    break;

  case 127:
#line 1999 "parse.y"
                {
		    (yyval.expr) = new _number_t;
		    sscanf((yyvsp[0].literal)->c_str(),
			   "%x",
			   &(((_number_t*)(yyval.expr)) -> semantic_value));
		    ((_number_t*)(yyval.expr)) -> radix = HEX;
		    (yyval.expr) -> const_valid = CONST_TRUE;
		    // number literal is always constant
		}
#line 3310 "parse.tab.c"
    break;


#line 3314 "parse.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
