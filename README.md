Edward Rosales
Alan Pons
# PL/0 Lexical Analyzer

## Description
This is a simple lexical analyzer for the programming language PL/0.
The program reads in a source program written in PL/0 which is then processed to tokens. A list is created with these tokens.


From this list we use the EBNF grammer to process the tokens with correct syntax. Using a Recursive Descent Parser 
EBNF of tiny PL/0:
program ::= block "." . block ::= const-declaration var-declaration statement. constdeclaration ::= [ “const” ident "=" number {"," ident "=" number} “;"]. var-declaration ::= [ "var" ident {"," ident} “;"]. statement ::= [ ident ":=" expression | "begin" statement { ";" statement } "end" | "if" condition "then" statement "fi" | "while" condition "do" statement | "read" ident | "write" expression | empty ] . condition ::= "odd" expression | expression rel-op expression. rel-op ::= "="|“<>"|"<"|"<="|">"|">=“. expression ::= term { ("+"|"-") term}. term ::= factor {("*"|"/") factor}. factor ::= ident | number | "(" expression ")“. number ::= digit {digit}. ident ::= letter {letter | digit}. digit ;;= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9“. letter ::= "a" | "b" | ... | "y" | "z" | "A" | "B" | ... |"Y" | "Z". Based on Wirth’s definition for EBNF we have the following rule: [ ] means an optional item.

## Usage
1. Compile the program using a C compiler.
2. Run the compiled executable, providing the input file containing the PL/0 source code.
3. The program will generate the token list as output.

## Input Format
The input file should contain the PL/0 source code.
Compile with gcc parsercodegen.c
./a.out input.txt

## Output
If the program does not follow the grammar, a message indicating the type of error presented.
Else if the program follows the grammer then the generated OPCODE is printed as well as the symbol table holding variables and constants.

