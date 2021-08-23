CC = gcc
CPP = g++
CFLAGS = -std=c++11 -x c++ -Wno-potentially-evaluated-expression
LINK = g++
ARM_LINK = clang++-9 -march=armv8-a
PI = 192.168.1.12
ALL_TARGETS = AST.o IR.o context.o common.o exceptions.o eval.o spec.o extra.o lex.yy.o parse.tab.o codegen.o irtree_optimize.o flow_optimize.o asm_optimize.o asm_flow_optimize.o
all: $(ALL_TARGETS)
	$(LINK) $^ -o compiler
	git add .
	git commit -m "[Compilation] Automatic Commit at `date`"
	git push
lex.yy.o: sysy.l
	flex --header-file=lex.h sysy.l
	$(CPP) $(CFLAGS) -c lex.yy.c       -o lex.yy.o

parse.tab.o: parse.y 
	bison parse.y --defines=parse.h
	$(CPP) $(CFLAGS) -c parse.tab.c -o parse.tab.o

extra.o: extra.cpp debug.h AST.h parse.h lex.h IR.h codegen.h irtree_optimize.h
	$(CPP) $(CFLAGS) -c extra.cpp      -o extra.o
common.o: common.cpp
	$(CPP) $(CFLAGS) -c common.cpp      -o common.o
AST.o: AST.cpp AST.h common.h tokens.h context.h IR.h
	$(CPP) $(CFLAGS) -c AST.cpp        -o AST.o
IR.o: IR.cpp IR.h AST.h common.h
	$(CPP) $(CFLAGS) -c IR.cpp         -o IR.o
spec.o: spec.cpp AST.h common.h
	$(CPP) $(CFLAGS) -c spec.cpp       -o spec.o
context.o: context.cpp context.h common.h exceptions.h
	$(CPP) $(CFLAGS) -c context.cpp    -o context.o
eval.o: eval.cpp AST.h context.h tokens.h IR.h exceptions.h
	$(CPP) $(CFLAGS) -c eval.cpp       -o eval.o
exceptions.o: exceptions.cpp exceptions.h
	$(CPP) $(CFLAGS) -c exceptions.cpp -o exceptions.o
codegen.o: codegen.cpp codegen.h common.h flow_optimize.h asm_optimize.h
	$(CPP) $(CFLAGS) -c codegen.cpp -o codegen.o
assembly.o: assembly.cpp assembly.h
	$(CPP) $(CFLAGS) -c assembly.cpp -o assembly.o
irtree_optimize.o: irtree_optimize.cpp irtree_optimize.h codegen.h 
	$(CPP) $(CFLAGS) -c irtree_optimize.cpp -o irtree_optimize.o
flow_optimize.o: flow_optimize.cpp flow_optimize.h
	$(CPP) $(CFLAGS) -c flow_optimize.cpp -o flow_optimize.o
asm_optimize.o: asm_optimize.cpp asm_optimize.h asm_flow_optimize.h
	$(CPP) $(CFLAGS) -c asm_optimize.cpp -o asm_optimize.o
asm_flow_optimize.o: asm_flow_optimize.cpp asm_flow_optimize.h
	$(CPP) $(CFLAGS) -c asm_flow_optimize.cpp -o asm_flow_optimize.o
lex.h: lex.yy.o
debug.h: common.h
AST.h: debug.h common.h IR.h context.h
parse.h: parse.tab.o
IR.h:
context.h: exceptions.h IR.h
tokens.h: sysy.l
codegen.h: IR.h
exceptions.h:
assembly.h: IR.h codegen.h common.h
irtree_optimize.h: IR.h AST.h common.h flow_optimize.h
flow_optimize.h: codegen.h common.h IR.h
asm_optimize.h: codegen.h common.h IR.h 
asm_flow_optimize.h: codegen.h common.h asm_optimize.h
clean:
	rm -f lex.h lex.yy.c parse.tab.c
	rm -f parse.cpp.grep.tab.c parse.cpp.tab.c compiler
	rm -f *.o
	rm -f *.gch
	rm -f parse.cpp.y parse.cpp.grep.y parse.cpp.bison.y parse.cpp.bison.tab.c

remote: 
	rsync -azr --exclude="*.o" --exclude=".git" ../src pi@$(PI):~/
	ssh pi@$(PI) "cd src; make clean; make _remote"
	git add .
	git commit -m "[Run-Remote] Automatic Commit at `date`"
	git push

_remote: $(ALL_TARGETS)
	$(ARM_LINK) $^ -o compiler

remote_exec_only:
	scp ./test.in pi@$(PI):~/src/
	ssh pi@$(PI) "cd src; make _rm_exec"
_rm_exec:
	./compiler test.in -o test.S
	$(ARM_LINK) test.S libsysy.a
	./a.out
remote_gcc:
	scp ./test.in pi@$(PI):~/src/
	ssh pi@$(PI) "cd src; make _rm_gcc"
_rm_gcc:
	cat gcc_header.sy test.in > test.gcc.sy
	$(ARM_LINK) -x c++ test.gcc.sy -L. -lsysy
	./a.out
remote_difftest:
	ssh pi@$(PI) "cd src; make _difftest"
_difftest:
	./compiler test.in -o test.S
	$(ARM_LINK) test.S libsysy.a
	./a.out > _naive.out
	cat gcc_header.sy test.in > test.gcc.sy
	$(ARM_LINK) -x c++ test.gcc.sy -L. -lsysy
	./a.out > _gcc.out
	echo "naive:"
	cat _naive.out
	echo "gcc:"
	cat _gcc.out
	diff -Z _naive.out _gcc.out
	rm _naive.out
	rm _gcc.out
