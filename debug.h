#ifndef _DEBUG_H
#define _DEBUG_H
#include <assert.h>
#include <iostream>
#include "common.h"
#define token_t int
#define literal_t std::string
#define __BREAK_REASON(reason)			\
{						\
  std::cerr<<COLOR_YELLOW<<reason		\
	   <<COLOR_RED<<" BREAK"		\
	   <<COLOR_YELLOW<<"!"			\
	   <<COLOR_NONE<<std::endl;		\
}

#define TODO_PRINT() { __BREAK_REASON("PRINT");	\
    assert(0); }

#define TODO_GEN_IR() ;
#define TODO_OPTIMIZE() ;

#define TODO_MANUAL() { __BREAK_REASON("MANUAL");	\
                        assert(0);}

#define PANIC_MSG(msg) { __BREAK_REASON(msg);	\
                        assert(0);}

enum RADIX_T { DEC = 0, OCT, HEX };
// insert [TODO] on untested code
#endif
