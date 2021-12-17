// COP 3402: Systems Software
// vm.c

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#define MAX_PAS_LENGTH 500

// Globals
int BP;
int pas[500];


int base(int L)
{
	int arb = BP;	// arb = activation record base
	while ( L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}



void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas2, int GP, int OP, int print)
{
	if(print == 1)
	{
		// If jump operation, print out info
		if(OP > 0) {
			printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[PC-OP+1], IR[PC-OP+2], PC, BP, SP, DP);
		} else {
			printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[PC-2], IR[PC-1], PC, BP, SP, DP);
		}

		// print data section
		int i;
		for (i = GP; i <= DP; i++)
			printf("%d ", pas2[i]);
		printf("\n");

		// print stack
		printf("\tstack : ");
		for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
			printf("%d ", pas2[i]);
	}
}


void execute_program(instruction *code, int printFlag)
{
	if(printFlag == 1)
    	printf("				PC	BP	SP	DP	data\n");

    int OP;
    int L;
    int M;
    int PC = 0;
    int SP = MAX_PAS_LENGTH;
    int DP = 0;
    int GP = 0;
	int temp;

    int arr[500];

    int counter = 0;

	int c = 0;
	while(code[c].opcode != -1)
	{
		arr[counter] = code[c].opcode;
		arr[counter + 1] = code[c].l;
		arr[counter + 2] = code[c].m;
		counter+=3;
		c++;
	}

    BP = counter;
	GP = BP;
    DP = BP - 1;

	if(printFlag == 1)
    	printf("Initial values:\t\t\t%d\t%d\t%d\t%d\t\n", PC, BP, SP, DP);

    int line = 0;
    for(PC = 0; PC <= counter; PC+=3){
		// If its the first line ran, to not do PC-3
		// (this is because of the PC increment following the next step through for loop)
        if(!PC == 0) {
            line = PC - 3;
            OP = arr[PC - 3];
            L = arr[PC - 2];
            M = arr[PC - 1];
        } else {
            line = PC;
            OP = arr[PC];
            L = arr[PC + 1];
            M = arr[PC + 2];
        }

		// Switch case for operations
        switch(OP) {
			// LIT
            case 1:
                if (BP == GP) {
                    DP = DP + 1;
                    pas[DP] = M;
                }
                else {
                    SP--;
                    pas[SP] = M;
                }
                print_execution(line/3, "LIT", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// OPR
            case 2:
                if(M == 0) {
                    SP = BP + 1;
					PC = pas[SP - 4];
                    BP = pas[SP - 3];
                    print_execution(line/3, "RTN", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 1) {
                    if (BP == GP) {
                        pas[DP] = -1 * pas[DP];
                    } else {
                        pas[SP] = -1 * pas[SP];
                    }
                    print_execution(line/3, "NEG", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 2) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] + pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] + pas[SP - 1];
                    }
                    print_execution(line/3, "ADD", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 3) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] - pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] - pas[SP - 1];
                    }
                    print_execution(line/3, "SUB", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 4) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] * pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] * pas[SP - 1];
                    }
                    print_execution(line/3, "MUL", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 5) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] / pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] / pas[SP - 1];
                    }
                    print_execution(line/3, "DIV", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 6) {
                    if (BP == GP) {
                        pas[DP] = pas[DP] % 2;
                    } else {
                        pas[SP]  = pas[SP] % 2;
                    }
                    print_execution(line/3, "ODD", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 7) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] % pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] % pas[SP - 1];
                    }
                    print_execution(line/3, "MOD", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 8) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] == pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] == pas[SP - 1];
                    }
                    print_execution(line/3, "EQL", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 9) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] != pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] != pas[SP - 1];
                    }
                    print_execution(line/3, "NEQ", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 10) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] < pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] < pas[SP - 1];
                    }
                    print_execution(line/3, "LSS", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 11) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] <= pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] <= pas[SP - 1];
                    }
                    print_execution(line/3, "LEQ", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 12) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] > pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] > pas[SP - 1];
                    }
                    print_execution(line/3, "GTR", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                } else if(M == 13) {
                    if (BP == GP) {
                        DP = DP - 1;
                        pas[DP] = pas[DP] >= pas[DP + 1];
                    } else {
                        SP = SP + 1;
                        pas[SP] = pas[SP] >= pas[SP - 1];
                    }
                    print_execution(line/3, "GEQ", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                }
                break;
			// LOD
            case 3:
                if (BP  == GP) {
                    DP = DP + 1;
					pas[DP] = pas[GP + M];
                } else if (base(L) == GP) {
                    SP = SP - 1;
                    pas[SP] = pas[GP + M];
                } else {
                    SP = SP - 1;
                    pas[SP] = pas[base(L) - M];
                }
                print_execution(line/3, "LOD", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// STO
            case 4:
                if (BP == GP) {
                    pas[GP + M] = pas[DP];
                    DP--;
                } else
				{
					if (base(L) == GP) {
	                    pas[GP + M] = pas[SP];
	                    SP = SP + 1;
					}
					else
					{
						pas[base(L) - M] = pas[SP];
                        SP = SP + 1;
					}
				}
                print_execution(line/3, "STO", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// CAL
            case 5:
				pas[SP - 1] = 0;
                pas[SP - 2]  =  base(L);
                pas[SP - 3]  = BP;
                pas[SP - 4]  = PC;
                BP = SP - 1;
                PC = M;
                print_execution(line/3, "CAL", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// INC
            case 6:
                if (BP == GP) {
                    DP = DP + M;
                } else {
                    SP = SP - M;
                }
                print_execution(line/3, "INC", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// JMP
            case 7:
				PC = M;
                print_execution(line/3, "JMP", arr, PC, BP, SP, DP, pas, GP, PC, printFlag);
                break;
			// JPC
            case 8:
                if (BP == GP) {
                    if (pas[DP] == 0) {
                        PC = M;
                    }
                    DP = DP - 1;
                } else {
                    if (pas[SP] == 0) {
                        PC = M;
                    }
					SP = SP + 1;
                }
                print_execution(line/3, "JPC", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
			// SYS
            case 9:
                if(M == 1) {
                    if (BP == GP) {
						if(printFlag == 1)
						{
							printf("Top of Stack Value: ");
	                        printf("%d\n", pas[DP]);
						}
                        DP = DP -1;
                    } else {
						if(printFlag == 1)
						{
							printf("Top of Stack Value: ");
	                        printf("%d\n", pas[SP]);
						}
                        SP = SP + 1;
                    }
                } else if(M == 2) {
                    if (BP == GP) {
                        DP = DP + 1;
						if(printFlag == 1)
						{
							printf("Please Enter an Integer: ");
	                        scanf("%d", &pas[DP]);
						}

                    } else {
                        SP = SP - 1;
						if(printFlag == 1)
						{
							printf("Please Enter an Integer: ");
							scanf("%d", &pas[SP]);
						}
                    }

                } else {
					print_execution(line/3, "SYS", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
					if(printFlag == 1)
						printf("\n");
                    exit(0);
                }
                print_execution(line/3, "SYS", arr, PC, BP, SP, DP, pas, GP, 0, printFlag);
                break;
            default:
                break;
        }
		if(printFlag == 1)
        	printf("\n");
    }
}
