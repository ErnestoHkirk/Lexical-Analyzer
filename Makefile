
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++14 -Wall

run_lexer: lexer
	./lexer
	 
headers: FileReader.hpp readTable.hpp
 
lexer: headers Main.cpp
	${CXX} FileReader.cpp readTable.cpp Main.cpp -o lexer
	
clean: 
	rm -f lexer
