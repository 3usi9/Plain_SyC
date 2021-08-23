#include <stdio.h>
#include <unistd.h>
#include<fstream>
#include "debug.h"
#include "AST.h"
#include "parse.h"
#include "lex.h"
#include "IR.h"
#include "codegen.h"
#include "irtree_optimize.h"
void yyerror(char const* s)
{
  fprintf(stderr, "%s\n", s);
}
extern context_t global_ctx;
extern root_t* AST;
extern std::map<std::string, int> argcount;
int print_Syntax  = 0;
int print_IRTree  = 0;
int print_OptIRTree = 0;
int print_IRLine  = 0;
int print_OptIRLine = 0;
int print_ASMLine = 0;
int print_OptASMLine = 0;
int print_ASMFlow = 0;
int print_OptASMFlow = 0;
int print_FlowGraph = 0;
int print_optFlowGraph = 0;
int print_Register = 0;
int output_comment = 0;
int optimize_flag = 0;
int print_log     = 0;
char* program_name = NULL;
const char* help_message =
  R"(PlainSyc, A SysY Compiler written by bzq @ hust
       -h     	       show this help message
       -o [file]       select output file (default is [stdout])
       -O0	       basic optimize
       -O1	       aggressive optimize
       -O2	       aggressive optimize
       -g	       output comments in assembly file
       -pSyn	       print Syntax Tree
       -pIRTree	       print IR Tree
       -pOptIRTree     print Optimized IR Tree
       -pIRLine	       print IR List
       -pOptIRLine     print Optimized IR Line
       -pFlow	       print (unoptimized) Flow Graph
       -pOptFlow       print Optimizd Flow Graph
       -pReg           print Register Allocation
       -pASM	       print (highlighted) assembly code
       -pOptASM	       print Optimized assembly code
       -pASMFlow       print Assembly flow graph
       -pOptASMFlow    print Optimizd Assembly Flow Graph
       -plog	       print execution logs
)";
std::string input_filename = "";
std::string output_filename = "";

std::set< std::pair<std::string, int> > colors =
  {
    {COLOR_RED, std::string(COLOR_RED).size()},
    {COLOR_GREEN, std::string(COLOR_GREEN).size()},
    {COLOR_YELLOW, std::string(COLOR_YELLOW).size()},
    {COLOR_BLUE, std::string(COLOR_BLUE).size()},
    {COLOR_PURPLE, std::string(COLOR_PURPLE).size()},
    {COLOR_CYAN, std::string(COLOR_CYAN).size()},
    {COLOR_WHITE, std::string(COLOR_WHITE).size()},
    {COLOR_NONE, std::string(COLOR_NONE).size()},
  };

void print_nocolor(std::ostream& out,
		   const std::string& in)
{

  for(int i = 0; i < in.size(); i++)
    {
#ifdef COLOR_CODE
    test_pass:
      for(auto p : colors)
      	{
      	  if(in.substr(i, p.second) == p.first)
      	    {
      	      i += p.second;
	      goto test_pass;
      	    }
	}
#endif
      out<<in[i];

    }
}

void read_args(int argc, char** argv)
{
  program_name = argv[0];
  for(int i = 1; i < argc; i++)
    {
      std::string liter = argv[i];
      if(liter == "-h")
	{
	  std::cerr<<help_message;
	  exit(0);
	}
      else if(liter == "-o")
	{
	  if(i == argc - 1)
	    {
	      std::cerr<<"No output specified, see -h for help"<<std::endl;
	      exit(-1);
	    }
	  else
	    {
	      output_filename = argv[i+1];
	      i++;
	      continue;
	    }
	}
      else if(liter == "-S")
	{ // For Judgements
	}
      else if(liter == "-O0")
	optimize_flag = 0;
      else if(liter == "-O1")
	{
	  optimize_flag = 1;
	  // optimize_flag = 0;
	}
      else if(liter == "-O2")
	{
	  optimize_flag = 1;
	  // optimize_flag = 0;
	}
      else if(liter == "-g")
	output_comment = 1;
      else if(liter == "-pSyn")
	print_Syntax = 1;
      else if(liter == "-pIRTree")
	print_IRTree = 1;
      else if(liter == "-pOptIRTree")
	print_OptIRTree = 1;
      else if(liter == "-pIRLine")
	print_IRLine = 1;
      else if(liter == "-pOptIRLine")
	print_OptIRLine = 1;
      else if(liter == "-pFlow")
	print_FlowGraph = 1;
      else if(liter == "-pOptFlow")
	print_optFlowGraph = 1;
      else if(liter == "-pReg")
	print_Register = 1;
      else if(liter == "-pASM")
	print_ASMLine = 1;
      else if(liter == "-pOptASM")
	print_OptASMLine = 1;
      else if(liter == "-pASMFlow")
	print_ASMFlow = 1;
      else if(liter == "-pOptASMFlow")
	print_OptASMFlow = 1;
      else if(liter == "-plog")
	print_log = 1;
      else
	{
	  input_filename = liter;
	}
      
    }
}


int main(int argc, char** argv)
{
  read_args(argc, argv);
  if(input_filename == "")
    {
      std::cerr<<"No input file specified, see -h for help"<<std::endl;
      exit(-1);
    }
  // freopen(input_filename.c_str(), "r", stdin);


  yyin = fopen(input_filename.c_str(), "r");
  // {//debug
  //   char buf[50];
  //   fread(buf, 50, 1, yyin);
  //   std::cerr<<"lines:"<<buf;

  // }

  std::ostream* out = &std::cout;
  std::ofstream fout(output_filename);
  if(output_filename != "")
    {
      out = &fout;
   }
  // Lexical & Syntax check.
  if(yyparse())
    {
      std::cerr<<"Parsing failed."<<std::endl;
      exit(254);
      // ExitCode 254
    }

  if(print_Syntax)
    {
      AST->printNode();
      std::cerr<<COLOR_YELLOW<<"Syntax Check "
	       <<COLOR_GREEN<<"Finished"
	       <<COLOR_YELLOW<<"!"
	       <<COLOR_NONE<<std::endl;
    }

  // External Functions
  global_ctx.var_table_push_globl("getint",
				  sym_t(GLOBL_FNC(std::string("getint")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({}))
			   );
  argcount.insert({"getint",0});
  global_ctx.var_table_push_globl("getch",
				  sym_t(GLOBL_FNC(std::string("getch")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({}))
			   );
  argcount.insert({"getch",0});
  std::vector<int> dimlist = {-1};
  global_ctx.var_table_push_globl("getarray",
				  sym_t(GLOBL_FNC(std::string("getarray")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({new arg_t(ARG_ARRAY, dimlist)}
					      )
					));
  argcount.insert({"getarray",1});
  global_ctx.var_table_push_globl("putint",
				  sym_t(GLOBL_FNC(std::string("putint")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({new arg_t(ARG_SINGLE)}
					      )
					));
  argcount.insert({"putint",1});
  global_ctx.var_table_push_globl("putch",
				  sym_t(GLOBL_FNC(std::string("putch")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({new arg_t(ARG_SINGLE)}
					      )
					));
  argcount.insert({"putch",1});
  global_ctx.var_table_push_globl("putarray",
				  sym_t(GLOBL_FNC(std::string("putarray")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({new arg_t(ARG_SINGLE), new arg_t(ARG_ARRAY, dimlist)}
					      )
					));
  argcount.insert({"putarray",2});
  global_ctx.var_table_push_globl("starttime",
  				  sym_t(GLOBL_FNC(std::string("starttime")),
  					SYMTYPE_FUNC,
  					std::vector<arg_t*>({})
  					));
  argcount.insert({"starttime",1}); // use [r0]
  global_ctx.var_table_push_globl("stoptime",
  				  sym_t(GLOBL_FNC(std::string("stoptime")),
  					SYMTYPE_FUNC,
  					std::vector<arg_t*>({})
  					));
  argcount.insert({"stoptime",1}); // use [r0]
  global_ctx.var_table_push_globl("_sysy_starttime",
				  sym_t(GLOBL_FNC(std::string("_sysy_starttime")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({})
					));
  argcount.insert({"_sysy_starttime",1}); // use [r0]
  global_ctx.var_table_push_globl("_sysy_stoptime",
				  sym_t(GLOBL_FNC(std::string("_sysy_stoptime")),
					SYMTYPE_FUNC,
					std::vector<arg_t*>({})
					));
  argcount.insert({"_sysy_stoptime",1}); // use [r0]
  argcount.insert({"__aeabi_idivmod",2}); // use [r0, r1]
  argcount.insert({"memset",3}); // use [r0, r1, r2]
  
  // IR Generating
  IR_t* IRNode = AST->make_IR(global_ctx);
  // Set LineNumber for each operation in IRTree
  IRNode->set_linenum(1);
  if(print_IRTree)
    {
      IRNode->print(std::cerr, 0);
      std::cerr<<COLOR_YELLOW<<"IR Generating "
	       <<COLOR_GREEN<<"Finished"
	       <<COLOR_YELLOW<<"!"
	       <<COLOR_NONE<<std::endl;
    }

  
  IRTree_Optimize(IRNode);


  if(print_OptIRTree)
    {
      IRNode->print(std::cerr, 0);
      std::cerr<<COLOR_YELLOW<<"IRTree Optimization "
	       <<COLOR_GREEN<<"Finished"
	       <<COLOR_YELLOW<<"!"
	       <<COLOR_NONE<<std::endl;
    }
  // LinearIR Generating
  std::list<IR_t*> LinearIR;
  IRNode->collect(LinearIR);
   


  if(print_IRLine)
    {
      for(auto& it : LinearIR)
	{
	  it->print(std::cerr, -1);
	}
      std::cerr<<COLOR_YELLOW<<"LinearIR Generating "
	       <<COLOR_GREEN<<"Finished"
	       <<COLOR_YELLOW<<"!"
	       <<COLOR_NONE<<std::endl;
    }

  if(optimize_flag == 1)
    IRLine_Optimize(LinearIR, global_ctx.get_unique_id(), true);
  // Fake optimize

  if(optimize_flag == 1)
    {
      std::list<IR_t*> Fake_LinearIR;
      // Make a deep copy for each IROper, others are just keeped
      for(auto it : LinearIR)
	{
	  if(typeid(*it) == typeid(IROper_t))
	    {
	      Fake_LinearIR.push_back(new IROper_t(*(IROper_t*)it));
	    }
	  else
	    {
	      Fake_LinearIR.push_back(it);
	    }
	}
      // printf("Fake LinearIR complete.");
      asm_context_t fake_asm_ctx;
      std::list<instr_t*> fake_asm_list;

      fake_asm_ctx.make_ASM(Fake_LinearIR.begin(),
			    Fake_LinearIR.end(),
			    fake_asm_list, true);
      // Post-flow Optimize
      IRLine_Optimize_PostFlow(LinearIR, Fake_LinearIR);
      for(int i = 0; i < 5; i++)
	{
	  // printf("IRLine Pass %d\n", i);
	  IRLine_Optimize(LinearIR, global_ctx.get_unique_id(), false);
	}
    }
  // Real optimize
  if(print_OptIRLine)
    {
      for(auto& it : LinearIR)
	{
	  it->print(std::cerr, -1);
	}
      std::cerr<<COLOR_YELLOW<<"Optimized LinearIR Generating "
	       <<COLOR_GREEN<<"Finished"
	       <<COLOR_YELLOW<<"!"
	       <<COLOR_NONE<<std::endl;
    }

  
  asm_context_t asm_ctx;
  std::list<instr_t*> asm_list;
  asm_ctx.make_ASM(LinearIR.begin(),
  		   LinearIR.end(),
  		   asm_list, false);
  
  if(print_OptASMLine)
    {
      for(auto p : asm_list)
	{
	  p->print(std::cout);
	}
    }

  if((print_ASMLine || print_OptASMLine) && out == &std::cout)
    {
      // do nothing
    }
  else
    {
      std::stringstream ss;
      for(auto p : asm_list)
	{
	  if(p->mnemonic == MNEMO_NOTE || p->mnemonic == MNEMO_PASSNOTE) // If it's comment
	    {
	      if(output_comment)
		p->print(ss);
	    }
	  else // Not a comment
	    {
	      p->print(ss);
	    }
	  
	}
      print_nocolor(*out, ss.str());
    }
  fclose(yyin);
  fout.close();
  return 0;
}
