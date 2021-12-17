// Sebastian Scharager
// se432372
// COP 3402: Systems Software
// parser.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define UNMARKED 1
#define MARKED 0

instruction *code;
int cIndex;
symbol *table;
int tIndex;
int lexLevel = 0;
int tokenIndex = 0;


void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

void program(lexeme *list);
void block(lexeme *list);
void constDeclaration(lexeme *list);
int varDeclaration(lexeme *list);
void procedureDeclaration(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
void mark(lexeme *list);
int multipleDeclarationCheck(lexeme token);
int findSymbol(lexeme token, int x);

// Basic running of program
void program(lexeme *list)
{
	emit(7, 0, 0);
	// Adds to symbol table
	addToSymbolTable(3, "main", 0, 0, 0, UNMARKED);
	lexLevel = -1;
	block(list);
	if(list[tokenIndex].type != periodsym)
	{
		// Not closed by period
		printparseerror(1);
	}
	// HALT
	emit(9, 0, 3);
	for(int i = 0; i < cIndex; i++)
	{
		// OPR 5 (CALL)
		if(code[i].opcode == 5)
		{
			code[i].m = table[code[i].m].addr;
		}
	}
	code[0].m = table[0].addr;
}

// Blocks of code
void block(lexeme *list)
{
	lexLevel++;
	int procedure_idx = tIndex - 1;
	// call const dec
	constDeclaration(list);
	// call var dec
	int x = varDeclaration(list);
	// call proc dec
	procedureDeclaration(list);
	table[procedure_idx].addr = cIndex * 3;
	if (lexLevel == 0)
	{
		// INC
		emit(6, 0, x);
	}
	else
	{
		// INC
		emit(6, 0, x + 3);
	}
	statement(list);
	mark(list);
	lexLevel--;
}

// Constant declaration checker
void constDeclaration(lexeme *list)
{
	// Checks if constant
	if (list[tokenIndex].type == constsym)
	{
		do {
			tokenIndex++;
			// If next is not an identifier, error
			if(list[tokenIndex].type != identsym)
			{
				printparseerror(2);
			}
			int symidx = multipleDeclarationCheck(list[tokenIndex]);
			if(symidx != -1)
			{
				printparseerror(18);
			}
			char *name = list[tokenIndex].name;
			// If next is not an assigner, error
			tokenIndex++;
			if(list[tokenIndex].type != assignsym)
			{
				printparseerror(2);
			}

			tokenIndex++;
			// If next is not a number, error
			if(list[tokenIndex].type != numbersym)
			{
				printparseerror(2);
			}
			// This might have to be the value of the token 2 indexes behind? not sure.
			addToSymbolTable(1, name, list[tokenIndex].value, lexLevel, 0, UNMARKED);
			tokenIndex++;

		} while(list[tokenIndex].type == commasym);

		// If it doesnt end in a semicolon, then multiple constants are declared in one line
		if(list[tokenIndex].type != semicolonsym)
		{
			if(list[tokenIndex].type == identsym)
			{
				printparseerror(2);
			}
			else
			{
				printparseerror(13);
			}
		}
		tokenIndex++;
	}
}

// Variable declaration checker
int varDeclaration(lexeme *list)
{
	int numVars = 0;
	if(list[tokenIndex].type == varsym)
	{
		do {
			numVars++;
			tokenIndex++;
			// If next is not an identifier
			if (list[tokenIndex].type != identsym) {
				printparseerror(3);
			}
			int symidx = multipleDeclarationCheck(list[tokenIndex]);
			if(symidx != -1)
			{
				printparseerror(18);
			}
			if(lexLevel == 0)
			{
				addToSymbolTable(2, list[tokenIndex].name, 0, lexLevel, numVars - 1, UNMARKED);
			} else
			{
				addToSymbolTable(2, list[tokenIndex].name, 0, lexLevel, numVars + 2, UNMARKED);
			}
			tokenIndex++;
			// While theres commas separating each variable
		} while(list[tokenIndex].type == commasym);

		// If there isn't a semicolon, then theres an error
		if(list[tokenIndex].type != semicolonsym)
		{
			if(list[tokenIndex].type == identsym)
			{
				printparseerror(3);
			}
			else
			{
				printparseerror(13);
			}
		}
		tokenIndex++;
	}
	// Num vars of that kind
	return numVars;
}

// Procedure declaration checker
void procedureDeclaration(lexeme *list)
{
	while(list[tokenIndex].type == procsym)
	{
		tokenIndex++;
		// If next is not an identifier, error
		if(list[tokenIndex].type != identsym)
		{
			printparseerror(4);
		}
		int symidx = multipleDeclarationCheck(list[tokenIndex]);
		if(symidx != -1)
		{
			printparseerror(18);
		}
		addToSymbolTable(3, list[tokenIndex].name, 0, lexLevel, 0, UNMARKED);

		tokenIndex++;
		// If next is not a semicolon, error
		if(list[tokenIndex].type != semicolonsym)
		{
			printparseerror(4);
		}

		tokenIndex++;
		block(list);
		// If next is not a semicolon, error
		if(list[tokenIndex].type != semicolonsym)
		{
			printparseerror(4);
		}

		tokenIndex++;
		// RETURN (M = 0)
		emit(2, 0, 0);
	}
}

// Statements such as identifier := ()
void statement(lexeme *list)
{
	if(list[tokenIndex].type == identsym)
	{
		// finds the symbol
		int symIdx = findSymbol(list[tokenIndex], 2);
		if(symIdx == -1)
		{
			if(findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 3))
			{
				printparseerror(10);
			}
			else
			{
				printparseerror(10);
			}
		}
		// If next isnt assigning a value, then its an eror
		tokenIndex++;
		if(list[tokenIndex].type != assignsym)
		{
			printparseerror(5);
		}

		tokenIndex++;
		expression(list);

		// STO
		//printf("EMIT 1\n");
		//printf("%d\n", symIdx);
		//printf("%d\n", table[2].addr);

		emit(4, lexLevel - table[symIdx].level, table[symIdx].addr);
		return;
	}

	if(list[tokenIndex].type == dosym)
	{
		// Goes until the end calling statement until theres a semicolon
		do {
			tokenIndex++;
			statement(list);
		} while(list[tokenIndex].type == semicolonsym);

		// Errors if there isnt an end symbol
		if(list[tokenIndex].type != odsym)
		{
			if(list[tokenIndex].type == identsym || list[tokenIndex].type == dosym
				|| list[tokenIndex].type == whensym	|| list[tokenIndex].type == elsedosym
				|| list[tokenIndex].type == whilesym || list[tokenIndex].type == readsym
				|| list[tokenIndex].type == writesym || list[tokenIndex].type == callsym)
			{
				printparseerror(15);
			}
			else
			{
				printparseerror(16);
			}
		}

		tokenIndex++;
		return;
	}

	if(list[tokenIndex].type == whensym)
	{
		tokenIndex++;

		condition(list);
		int jpcIdx = cIndex;

		// JPC
		emit(8, 0, jpcIdx);
		// Errors if theres not a then
		if(list[tokenIndex].type != dosym)
		{
			printparseerror(8);
		}

		tokenIndex++;
		statement(list);

		// Errors if theres not an else
		if(list[tokenIndex].type == elsedosym)
		{
			int jmpIdx = cIndex;
			// JUMP
			emit(7, 0, 0);
			code[jpcIdx].m = cIndex * 3;
			tokenIndex++;
			statement(list);
			code[jmpIdx].m = cIndex * 3;
		}
		else
		{
			code[jpcIdx].m = cIndex * 3;
		}
		return;
	}
	// Whilesym
	if(list[tokenIndex].type == whilesym)
	{
		tokenIndex++;

		int loopIdx = cIndex;
		condition(list);
		// Errors if theres not a do after the while
		if(list[tokenIndex].type != dosym)
		{
			printparseerror(9);
		}

		tokenIndex++;

		int jpcIdx = cIndex;
		// JPC
		emit(8, 0, jpcIdx);

		statement(list);

		// JMP (M = loopIdx * 3)
		emit(7, 0, loopIdx * 3);
		code[jpcIdx].m = cIndex * 3;
		return;
	}
	// Readsym
	if(list[tokenIndex].type == readsym)
	{
		tokenIndex++;
		// Error if next isnt an identifier
		if(list[tokenIndex].type != identsym)
		{
			printparseerror(6);
		}

		int symIdx = findSymbol(list[tokenIndex], 2);

		// Error if symbol not found
		if(symIdx == -1)
		{
			if(findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 3))
			{
				printparseerror(7);
			}
			else
			{
				printparseerror(6);
			}
		}

		tokenIndex++;
		// READ
		emit(9, 0, 2);
		// STO
		//printf("EMIT 2\n");
		emit(4, lexLevel - table[symIdx].level, table[symIdx].addr);
		return;
	}
	// Write sym
	if(list[tokenIndex].type == writesym)
	{
		tokenIndex++;
		expression(list);
		// WRITE
		emit(9, 0, 1);
		return;
	}

	// Call sym
	if(list[tokenIndex].type == callsym)
	{
		tokenIndex++;
		int symIdx = findSymbol(list[tokenIndex], 3);


		// If symbol not found, error.
		if(symIdx == -1)
		{
			if(findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 2))
			{
				printparseerror(7);
			}
			else
			{
				printparseerror(19);
			}
		}

		tokenIndex++;
		// CAL
		emit(5, lexLevel - table[symIdx].level, symIdx);
	}
}

// Conditioner checker
void condition(lexeme *list)
{
	if(list[tokenIndex].type == oddsym)
	{
		tokenIndex++;
		expression(list);
		// ODD
		emit(2, 0, 6);
	}
	else
	{
		expression(list);
		if(list[tokenIndex].type == eqlsym)
		{
			tokenIndex++;
			expression(list);
			// EQL
			emit(2, 0, 8);
		}
		else if(list[tokenIndex].type == neqsym)
		{
			tokenIndex++;
			expression(list);
			// NEQ
			emit(2, 0, 9);
		}
		else if(list[tokenIndex].type == lsssym)
		{
			tokenIndex++;
			expression(list);
			// LSS
			emit(2, 0, 10);
		}
		else if(list[tokenIndex].type == leqsym)
		{
			tokenIndex++;
			expression(list);
			// LEQ
			emit(2, 0, 11);
		}
		else if(list[tokenIndex].type == gtrsym)
		{
			tokenIndex++;
			expression(list);
			// GTR
			emit(2, 0, 12);
		}
		else if(list[tokenIndex].type == geqsym)
		{
			tokenIndex++;
			expression(list);
			// GEQ
			emit(2, 0, 13);
		}
		else
		{
			printparseerror(10);
		}
	}
}

// Expression checker
void expression(lexeme *list)
{
	// Sub sym
	if(list[tokenIndex].type == subsym)
	{
		tokenIndex++;
		term(list);

		// NEG
		emit(2, lexLevel, 1);
		while(list[tokenIndex].type == addsym || list[tokenIndex].type == subsym)
		{
			if(list[tokenIndex].type == addsym)
			{
				tokenIndex++;
				term(list);
				// ADD
				emit(2, 0, 2);
			}
			else{
				tokenIndex++;
				term(list);
				// SUB
				emit(2, 0, 3);
			}
		}
	}
	else
	{
		if(list[tokenIndex].type == addsym)
		{
			tokenIndex++;
		}

		term(list);
		// While they are subsyms or addsyms
		while(list[tokenIndex].type == addsym || list[tokenIndex].type == subsym)
		{
			if(list[tokenIndex].type == addsym)
			{
				tokenIndex++;
				term(list);
				// ADD
				emit(2, 0, 2);
			}
			else
			{
				tokenIndex++;
				term(list);
				// SUB
				emit(2, 0, 3);
			}
		}
	}

	// error
	if(list[tokenIndex].type == lparensym || list[tokenIndex].type == identsym
		|| list[tokenIndex].type == numbersym || list[tokenIndex].type == oddsym)
	{
		printparseerror(17);
	}
}

// Terms
void term(lexeme *list)
{
	factor(list);
	// While mult or div or mod
	while(list[tokenIndex].type == multsym || list[tokenIndex].type == divsym || list[tokenIndex].type == modsym)
	{
		if(list[tokenIndex].type == multsym)
		{
			tokenIndex++;
			factor(list);
			// MULTIPLY
			emit(2, 0, 4);
		}
		else if (list[tokenIndex].type == divsym) {
			tokenIndex++;
			factor(list);
			// DIVIDE
			emit(2, 0, 5);
		}
		else
		{
			tokenIndex++;
			factor(list);
			// MOD
			emit(2, 0, 7);
		}
	}
}

// Factor
void factor(lexeme *list)
{
	if(list[tokenIndex].type == identsym)
	{
		// Find symbol for the var and const
		int symIdx_var = findSymbol(list[tokenIndex], 2);
		int symIdx_const = findSymbol(list[tokenIndex], 1);
		if(symIdx_var == -1 && symIdx_const == -1)
		{
			if(findSymbol(list[tokenIndex], 3) != -1)
			{
				printparseerror(11);
			}
			else
			{
				printparseerror(19);
			}
		}

		if(symIdx_var == -1)
		{
			// LIT
			//printf("LIT 1\n");
			emit(1, 0, table[symIdx_const].val);
		}
		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
		{
			// LOD
			// for(int i = 0; i < tIndex; i++)
			// {
			// 	printf("%d ", table[i].addr);
			// }
			// printf("\n");
			emit(3, lexLevel - table[symIdx_var].level, table[symIdx_var].addr);
		}
		else
		{
			// LIT
			//printf("LIT 2\n");
			emit(1, 0, table[symIdx_const].val);
		}
		tokenIndex++;
	}
	else if(list[tokenIndex].type == numbersym)
	{
		// LIT
		//printf("LIT 3\n");
		emit(1, 0, list[tokenIndex].value);
		tokenIndex++;
	}
	else if(list[tokenIndex].type == lparensym)
	{
		// End of statement
		tokenIndex++;
		expression(list);
		if(list[tokenIndex].type != rparensym)
		{
			printparseerror(11);
		}
		tokenIndex++;
	}
	else
	{
		printparseerror(11);
	}
}

// Mark
void mark(lexeme *list)
{
	// Works bakwards to find an unmarked entry whose level is less than the current level.
	for(int i = tIndex; i <= 0; i--)
	{
		if(table[i].mark == UNMARKED && table[i].level < lexLevel)
		{
			return;
		}
		if(table[i].mark == UNMARKED && table[i].level == lexLevel)
		{
			table[i].mark = MARKED;
		}
	}
}

// Multiple of the same variable checker
int multipleDeclarationCheck(lexeme token)
{
	// Returns the index of the name given that is unmarked in symbol table.
	for(int i = 0; i < tIndex; i++)
	{
		if(strcmp(table[i].name, token.name) == 0 && table[i].mark == UNMARKED)
		{
			if(lexLevel == table[i].level)
			{
				return i;
			}
		}
	}
	// -1 if not found
	return -1;
}

// Finds the maximum level of a symbol
int findSymbol(lexeme token, int x)
{
	int y = -1;
	int max = -1;
	// Finds an unmarked entry whose level is less than the current level.
	// need max and a separate variable
	for(int i = 0; i < tIndex; i++)
	{
		if((strcmp(table[i].name, token.name) == 0) && (table[i].kind == x) && (table[i].mark == UNMARKED))
		{
			// printf("table name: %s\t|table kind: %d\t|table level: %d\n", table[i].name, table[i].kind, table[i].level);
			if(table[i].level >= max)
			{
				// printf("\n\t\ti: %d\n", i);
				// max = table[i].level + 1;
				max = table[i].level;
				y = i;
			}
		}
	}
	return y;
}

// Parse main method
instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;
	// Memory allocation
	code = calloc(sizeof(instruction), MAX_CODE_LENGTH);
	table = calloc(sizeof(instruction), MAX_SYMBOL_COUNT);
	program(list);

	// If prints which are passed into by function
	if(printTable == 1)
	{
		printsymboltable();
	}
	if(printCode == 1)
	{
		printassemblycode();
	}

	code[cIndex].opcode = -1;
	return code;
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
	exit(0);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
