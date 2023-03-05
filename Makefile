OUT = 8086

FLAGS = -Wall -fno-exceptions -fno-rtti -Wno-deprecated-declarations -Wno-unused-function -Wno-unused-variable  -Wno-missing-braces -Wno-logical-not-parentheses -Wno-switch -Wno-write-strings -Wno-tautological-compare -Wno-missing-braces -Wno-null-dereference -Wno-writable-strings

default: clean quick

clean: 
	rm -rf $(OUT) 

build: 
	clang 8086.c -o $(OUT) $(FLAGS)

quick: clean build
