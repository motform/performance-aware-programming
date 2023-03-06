OUT = target
BIN = 8086
MAIN = src/8086.c

FLAGS = -Wall -fno-exceptions -fno-rtti -Wno-deprecated-declarations -Wno-unused-function -Wno-unused-variable  -Wno-missing-braces -Wno-logical-not-parentheses -Wno-switch -Wno-write-strings -Wno-tautological-compare -Wno-missing-braces -Wno-null-dereference -Wno-writable-strings

default: quick

clean: 
	rm -rf $(OUT)/*

build: 
	clang $(MAIN) -o $(OUT)/$(BIN) $(FLAGS)

debug:
	clang $(MAIN) -g -o $(OUT)/$(BIN) $(FLAGS)

run: 
	./$(OUT)/$(BIN) $(p)

quick: clean debug
