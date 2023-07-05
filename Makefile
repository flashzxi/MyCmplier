VPATH = src doc
CC = clang++-9
OBJ = parse.o codegen.o lexical.o operator.o
LLVM = llvm-config-9
TAR := main

$(TAR): main.cpp output.o
	$(CC) $< output.o -o $(TAR)

output.o: input_example.data compiler
	./compiler $<

compiler: topLevel.cpp $(OBJ)
	$(CC) -O3 $< $(OBJ) `$(LLVM) --cxxflags --ldflags  --libs all` -std=c++17 -o compiler

%.o: %.cpp
	$(CC) -c -O3 $< `$(LLVM) --cxxflags --ldflags  --libs all` -std=c++17 -o $@

.PHONY:
clean: 
	rm -rf output.o parse.o lexical.o codegen.o operator.o
cleanall:
	rm -rf output.o main parse.o lexical.o codegen.o compiler operator.o