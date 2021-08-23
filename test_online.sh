lheader=`ls *.h *.hh *.hpp *.H *.hxx 2>/dev/null`
lcpp=`ls *.c *.cpp *.cp *.cxx *.cc *.c++ *.C *.CPP 2>/dev/null`
echo "CPP=$lcpp"
rm -rf ../run_test/*
cp $lheader ../run_test/
cp $lcpp ../run_test/
cd ../run_test
clang++-9 -std=c++17 -O2 -lm -lantlr4-runtime $lcpp -o compiler

