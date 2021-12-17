// COP 3402: Systems Software
// lex.c

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

// Removes comments starting with '//'
char * removeComments(char *input)
{
	for(int i = 0; i < strlen(input) - 1; i++)
	{
		if(input[i] == '/' && input[i+1] == '/')
		{
			for(int j = i; j < strlen(input) && input[j] != '\n'; j++)
			{
				input[j] = ' ';
			}
		}
	}

	return input;
}


int singleOrDoubleOP(char *string, int pos)
{
	if (string[pos] == '+')
	   return 1;
	else if (string[pos] == '-')
	   return 1;
	else if (string[pos] == '*')
	   return 1;
	else if (string[pos] == '/')
	   return 1;
	else if (string[pos] == '%')
	   return 1;
	else if (string[pos] == '(')
	   return 1;
	else if (string[pos] == ')')
	   return 1;
	else if (string[pos] == ',')
	   return 1;
	else if (string[pos] == '.')
	   return 1;
	else if (string[pos] == ';')
	   return 1;
	else if (string[pos] == '=') {
	   return 1;
   } else if (string[pos] == '>') {
	   if(string[pos + 1] == '=') {
		   return 2;
	   }
	   return 1;
   }
   else if (string[pos] == '<') {
	   if(string[pos + 1] == '=') {
		   return 2;
	   }
	   return 1;
   } else if (string[pos] == ':') {
	   if(string[pos + 1] == '=') {
		   return 2;
	   }
	   return -1;
   }
	return -1;
}


// Returns corresponding value of word string to words val, if not in there, its a variable.
int wordsCorrespondingValue(char *input)
{
	char *words[] = {"const", "var", "procedure", "do", "od", "while", "when", "elsedo", "call", "write", "read", ":=", "+", "-", "*", "/", "%", "=", "!=", "<", "<=", ">", ">=", "odd", "(", ")", ",", ".", ";"};
	int wordsValue[] = {constsym, varsym, procsym, dosym, odsym, whilesym, whensym, elsedosym, callsym, writesym, readsym, assignsym, addsym, subsym, multsym, divsym, modsym, eqlsym, neqsym, lsssym, leqsym, gtrsym, geqsym, oddsym, lparensym, rparensym, commasym, periodsym, semicolonsym};

	for(int i = 0; i < 29; i++)
	{
		if(strcmp(input, words[i]) == 0)
		{
			return wordsValue[i];
		}
	}
	strcpy(list[lex_index].name, input);
	return identsym;
}

lexeme *lexanalyzer(char *input, int printFlag)
{
	input = removeComments(input);

	list = calloc(sizeof(lexeme), MAX_NUMBER_TOKENS);
	lex_index = 0;

	for(int i = 0; i < strlen(input); i++)
	{
		int count = 0;
		int c = 0;
		char *temp = calloc(20, sizeof(char));
		if(isalpha(input[i]))
		{
			// Symbol operations
			while((isalpha(input[i]) || isdigit(input[i])) && i < strlen(input))
			{
				if(c > 11)
				{
					printlexerror(4);
				}
				temp[count++] = input[i++];
				c++;
			}

			// printf("%s\n", temp);
			int symbolType = wordsCorrespondingValue(temp);
			if(symbolType == identsym)
			{
				strcpy(list[lex_index].name, temp);
				list[lex_index].type = symbolType;
				lex_index++;
				i--;
			}
			else
			{
				list[lex_index].type = symbolType;
				lex_index++;
				i--;
			}
		}
		else if(isdigit(input[i]))
		{
			// printf("%c\n", input[i]);
			while(isdigit(input[i]) && i < strlen(input))
			{
				// printf("%c\n", input[i]);
				if(c > 5)
				{
					printlexerror(3);
				}
				temp[count++] = input[i++];
				if(isalpha(input[i]))
				{
					// printf("%c", input[i]);
					// printf("%c", input[i+1]);
					// printf("\n");
					printlexerror(2);
				}
				c++;
			}

			list[lex_index].type = numbersym;
			list[lex_index].value = atoi(temp);
			lex_index++;
			i--;
		}
		else if(ispunct(input[i]))
		{
			if(singleOrDoubleOP(input, i) == 2)
			{
				temp[0] = input[i];
				i++;
				temp[1] = input[i];
				int type = wordsCorrespondingValue(temp);
				if(type != identsym)
				{
					list[lex_index].type = type;
					lex_index++;
				}
				else
				{
					printlexerror(1);
				}
			}
			else if(singleOrDoubleOP(input, i) == 1)
			{
				temp[0] = input[i];
				int type = wordsCorrespondingValue(temp);
				if(type != identsym)
				{
					list[lex_index].type = type;
					lex_index++;
				}
				else
				{
					printlexerror(1);
				}
			}
		}
	}

	if(printFlag == 1)
	{
		printtokens();
	}

	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			case elsedosym:
				printf("%11s\t%d", "elsedo", elsedosym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	exit(0);
	return;
}
