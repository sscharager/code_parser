# code_parser
Parses a text file with written cod, and processes it into Virtual Machine language code with a certain object executable language (PL/0). This was done for my Systems Software class at the University of Central Florida.


---------------------------------------------------------------------------------------------------
Authors: Sebastian Scharager

	How to compile:
		Put driver.c, lex.c, parser.c, and vm.c all in the same directory
		
		Call this following command in terminal:
			gcc driver.c lex.c parser.c vm.c -lm

		Compiler supports the following compiler directives:
            		-l : print the list and table of lexemes/tokens (HW2 output) to the screen

            		-s : print the symbol table

            		-a : print the generated assembly code (parser/codegen output) to the screen

            		-v : print virtual machine execution trace to the screen

        	Therefore, run specific [input.txt] test case with whatever directives you
	        want printed out in your console:
	            ./a.out input.txt -l -s -a -v
              
---------------------------------------------------------------------------------------------------


Code Grammar (PL/0):

program ::= block "." . 
block ::= const-declaration  var-declaration  procedure-declaration statement.
const-declaration ::= ["const" ident “:=" number {"," ident “:=" number} ";"].
var-declaration  ::= [ "var "ident {"," ident} “;"].
procedure-declaration ::= { "procedure" ident ";" block ";" }.
statement   ::= [ ident ":=" expression
      | "call" ident  
      | "do" statement { ";" statement } "od" 
      | "when" condition "do" statement ["elsedo" statement]
      | "while" condition "do" statement
      | "read" ident
      | "write" expression
      | ε ] .  
condition ::= "odd" expression 
      | expression  rel-op  expression.  
rel-op ::= “="|“!="|"<"|"<="|">"|">=“.
expression ::= [ "+"|"-"] term { ("+"|"-") term}.
term ::= factor {("*"|"/"|”%”) factor}. 
factor ::= ident | number | "(" expression ")“.
number ::= digit {digit}.
ident ::= letter {letter | digit}.
digit ;;= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9“.
letter ::= "a" | "b" | ... | "y" | "z" | "A" | "B" | ... | "Y" | "Z".
