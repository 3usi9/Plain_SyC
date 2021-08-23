%option noyywrap
%{
#include "debug.h"
#include "AST.h"
#include "common.h"
#include "tokens.h"
#include <stdio.h>
#define MAKE_LEX_PRINT(type) \
  void LEX_PRINT_ ## type() \
  { \
    (COLOR_YELLOW "TOKEN " \
	   COLOR_GREEN #type COLOR_NONE "\n" \
	   ); \
  }
#define TOKEN(t) \
   (yylval.token = t)

#define SAVE_LITERAL() (yylval.literal = new std::string(yytext,yyleng))

MAKE_LEX_PRINT(CONST);
MAKE_LEX_PRINT(TYPE_INTEGER);
MAKE_LEX_PRINT(TYPE_VOID);
MAKE_LEX_PRINT(IF);
MAKE_LEX_PRINT(ELSE); 
MAKE_LEX_PRINT(WHILE);
MAKE_LEX_PRINT(BREAK);
MAKE_LEX_PRINT(CONTINUE);
MAKE_LEX_PRINT(RETURN);
MAKE_LEX_PRINT(COMMA);
MAKE_LEX_PRINT(SEMICOLON);
MAKE_LEX_PRINT(LPAR);
MAKE_LEX_PRINT(RPAR);
MAKE_LEX_PRINT(LSQUARE);
MAKE_LEX_PRINT(RSQUARE);
MAKE_LEX_PRINT(LBRACE);
MAKE_LEX_PRINT(RBRACE);
MAKE_LEX_PRINT(EQ);
MAKE_LEX_PRINT(LE);
MAKE_LEX_PRINT(GE);
MAKE_LEX_PRINT(NE);
MAKE_LEX_PRINT(LT);
MAKE_LEX_PRINT(GT);
MAKE_LEX_PRINT(ASSIGN);
MAKE_LEX_PRINT(PLUS);
MAKE_LEX_PRINT(MINUS);
MAKE_LEX_PRINT(MUL);
MAKE_LEX_PRINT(DIV);
MAKE_LEX_PRINT(MOD);
MAKE_LEX_PRINT(LAND);
MAKE_LEX_PRINT(LOR);
MAKE_LEX_PRINT(LNEG);
MAKE_LEX_PRINT(IDENTIFIER);
MAKE_LEX_PRINT(DEC_IMM);
MAKE_LEX_PRINT(HEX_IMM);
MAKE_LEX_PRINT(OCT_IMM);
%}

%%
[ ]                     { ; }
\n                      { ; }
\r                      { ; }
\t                      { ; }
\/\/.*\n?               { ; }
"/*"          		{
                          int buf;
			  while((buf = yyinput()) != 0) {
			    if(buf == '\n')
			      yylineno++;
			    else if(buf == '*') {
			      if((buf = yyinput()) == '/')
				break;
			      else
				unput(buf);
			    }
			  }
                         }
"const"                 { LEX_PRINT_CONST();         return TOKEN(CONST);}
"int"                   { LEX_PRINT_TYPE_INTEGER();  return TOKEN(TYPE_INTEGER);}
"void"                  { LEX_PRINT_TYPE_VOID();     return TOKEN(TYPE_VOID);}
"if"                    { LEX_PRINT_IF();            return TOKEN(IF);}
"else"                  { LEX_PRINT_ELSE();          return TOKEN(ELSE);}
"while"                 { LEX_PRINT_WHILE();         return TOKEN(WHILE);}
"break"                 { LEX_PRINT_BREAK();         return TOKEN(BREAK);}
"continue"              { LEX_PRINT_CONTINUE();      return TOKEN(CONTINUE);}
"return"                { LEX_PRINT_RETURN();        return TOKEN(RETURN);}
","                     { LEX_PRINT_COMMA();         return TOKEN(COMMA);}
";"                     { LEX_PRINT_SEMICOLON();     return TOKEN(SEMICOLON);}
"("                     { LEX_PRINT_LPAR();          return TOKEN(LPAR);}
")"                     { LEX_PRINT_RPAR();          return TOKEN(RPAR);}
"["                     { LEX_PRINT_LSQUARE();       return TOKEN(LSQUARE);}
"]"                     { LEX_PRINT_RSQUARE();       return TOKEN(RSQUARE);}
"{"                     { LEX_PRINT_LBRACE();        return TOKEN(LBRACE);}
"}"                     { LEX_PRINT_RBRACE();        return TOKEN(RBRACE);}
"=="                    { LEX_PRINT_EQ();            return TOKEN(EQ);}
"<="                    { LEX_PRINT_LE();            return TOKEN(LE);}
">="                    { LEX_PRINT_GE();            return TOKEN(GE);}
"!="                    { LEX_PRINT_NE();            return TOKEN(NE);}
"<"                     { LEX_PRINT_LT();            return TOKEN(LT);}
">"                     { LEX_PRINT_GT();            return TOKEN(GT);}
"="                     { LEX_PRINT_ASSIGN();        return TOKEN(ASSIGN);}
"+"                     { LEX_PRINT_PLUS();          return TOKEN(PLUS);}
"-"                     { LEX_PRINT_MINUS();         return TOKEN(MINUS);}
"*"                     { LEX_PRINT_MUL();           return TOKEN(MUL);}
"/"                     { LEX_PRINT_DIV();           return TOKEN(DIV);}
"%"                     { LEX_PRINT_MOD();           return TOKEN(MOD);}
"&&"                    { LEX_PRINT_LAND();          return TOKEN(LAND);}
"||"                    { LEX_PRINT_LOR();           return TOKEN(LOR);}
"!"                     { LEX_PRINT_LNEG();          return TOKEN(LNEG);}
[a-zA-Z_][a-zA-Z0-9_]*  { LEX_PRINT_IDENTIFIER(); SAVE_LITERAL(); return IDENTIFIER;}
[1-9][0-9]*             { LEX_PRINT_DEC_IMM(); SAVE_LITERAL(); return DEC_IMM;}
("0x"|"0X")[0-9a-fA-F]+ { LEX_PRINT_HEX_IMM(); SAVE_LITERAL(); return HEX_IMM;}
0[0-7]*                 { LEX_PRINT_OCT_IMM(); SAVE_LITERAL(); return OCT_IMM;}
.                       { printf(COLOR_RED "UNKNOWN "
				 COLOR_YELLOW "SYMBOL!\n"
				 COLOR_NONE); exit(-1);}
%%

