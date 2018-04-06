/*
 * COP3402 - Spring 2018
 * System Software Assignment 1
 * Submitted by: N***** P*****, Tyler Glorie
 */

#include <stdio.h>
#include <stdlib.h>

//prototypes
char* opCode(int i);
void printAll();
void printStack(int sp, int bp, int* stack, int lex);
void executeInstruction();
void sioInstructions();
int fetchInstruction();
int readFile(char* fileName);
int base(int l, int base);

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct {
    int op;
    int r;
    int l;
    int m;
} instruction;



FILE *fp;
instruction instructions[MAX_CODE_LENGTH];
instruction ir;
int stack[MAX_STACK_HEIGHT];
int registers[8];
int haltFlag;
int index;
int lex;
int sp;
int bp;
int pc;

int main(int argc, char* argv[]) {
    char* fileName = argv[1];
    int ins_length = 0;
    index = 0;
    haltFlag = 0;
    lex = 0;
    sp = 0;
    bp = 1;
    pc = 0;

    // Read file into instructions[]
    ins_length = readFile(fileName);

    // Error with finding file
    if(ins_length == -1) {
        return 2;
    }

    //printInstructions(ins_length);

    printf("\n OP Rg Lx Vl[ PC BP SP]\n");

    // Fetch and execute until halt flag gets triggered
    while(!haltFlag) {
        index = fetchInstruction();
        executeInstruction();
        printAll();
    }

    return 0;
}

char* opCode(int i) {
    switch(instructions[i].op) {

        case 1:
            return "LIT";

        case 2:
            return "RTN";

        case 3:
            return "LOD";

        case 4:
            return "STO";

        case 5:
            return "CAL";

        case 6:
            return "INC";

        case 7:
            return "JMP";

        case 8:
            return "JPC";

        case 9:
            return "SIO";

        case 10:
            return "NEG";

        case 11:
            return "ADD";

        case 12:
            return "SUB";

        case 13:
            return "MUL";
        case 14:
            return "DIV";

        case 15:
            return "ODD";

        case 16:
            return "MOD";

        case 17:
            return "EQL";

        case 18:
            return "NEQ";

        case 19:
            return "LSS";

        case 20:
            return "LEQ";

        case 21:
            return "GTR";

        case 22:
            return "GEQ";
        }
}

void printAll() {
    char* op = opCode(index);
    int i;

    printf("%-4s%3d%3d%3d[%3d%3d%3d] ", op, instructions[index].r, lex, instructions[index].m, pc, bp, sp);

     if (bp == 1) {
     	if (lex > 0) {
            printf("|");
        }
     }
     else {
     	//Print the lesser lexical level
     	printStack(bp-1, stack[bp + 2], stack, lex-1);
        printf("|");
     }
     //Print the stack contents - at the current level
     for (i = bp; i <= sp; i++) {
     	 printf("%3d ", stack[i]);
     }
     printf("\n\tRegisters:[%3d%3d%3d%3d%3d%3d%3d%3d]\n", registers[0], registers[1], registers[2], registers[3], registers[4], registers[5], registers[6], registers[7]);
}

void printStack(int sp, int bp, int* stack, int lex){
     int i;
     if (bp == 1) {
     	if (lex > 0) {
	   printf("|");
	   }
     }
     else {
     	  //Print the lesser lexical level
     	  printStack(bp-1, stack[bp + 2], stack, lex-1);
	  printf("|");
     }
     //Print the stack contents - at the current level
     for (i = bp; i <= sp; i++) {
     	 printf("%3d ", stack[i]);
     }
}

// Handles commands
void executeInstruction() {
    switch(ir.op) {

    case 1: // LIT
        registers[ir.r] = ir.m;
        break;

    case 2: // RTN
        sp = bp - 1;
        bp = stack[sp + 3];
        pc = stack[sp + 4];
        lex--; //instructions[pc].l--;
        break;

    case 3: // LOD
        registers[ir.r] = stack[base(ir.l, bp) + ir.m];
        break;

    case 4: // STO
        stack[base(ir.l, bp) + ir.m - ((lex == 0) ? 1: 0)] = registers[ir.r];   // band-aid fix for off-by-1 error
        break;

    case 5: // CAL
        stack[sp + 1] = 0;
        stack[sp + 2] = base(ir.l, bp);
        stack[sp + 3] = bp;
        stack[sp + 4] = pc;
        bp = sp + 1;
        sp = sp + 4;
        pc = ir.m;
        lex++;
        break;

    case 6: // INC
        sp = sp + ir.m;
        break;

    case 7: // JMP
        pc = ir.m;
        break;

    case 8: // JPC
        if(registers[ir.r] == 0) {
            pc = ir.m;
        }
        break;

    case 9: // SIO
        sioInstructions();
        break;

    case 10: // NEG
        registers[ir.r] = (registers[ir.l] * -1);
        break;

    case 11: // ADD
        registers[ir.r] = registers[ir.l] + registers[ir.m];
        break;

    case 12: // SUB
        registers[ir.r] = registers[ir.l] - registers[ir.m];
        break;

    case 13: // MUL
        registers[ir.r] = registers[ir.l] * registers[ir.m];
        break;

    case 14: // DIV
        registers[ir.r] = registers[ir.l] / registers[ir.m];
        break;

    case 15: // ODD
        registers[ir.r] = registers[ir.r] % 2;
        break;

    case 16: // MOD
      registers[ir.r] = registers[ir.l] % registers[ir.m];
      break;

    case 17: // EQL
        registers[ir.r] = (registers[ir.l] == registers[ir.m]);
        break;

    case 18: // NEQ
        registers[ir.r] = registers[ir.l] != registers[ir.m];
        break;

    case 19: // LSS
        registers[ir.r] = (registers[ir.l] < registers[ir.m]) ? 1 : 0;
        break;

    case 20: // LEQ
        registers[ir.r] = (registers[ir.l] <= registers[ir.m]) ? 1 : 0;
        break;

    case 21: // GTR
        registers[ir.r] = (registers[ir.l] > registers[ir.m]) ? 1 : 0;
        break;

    case 22: // GEQ
        registers[ir.r] = (registers[ir.l] >= registers[ir.m]) ? 1 : 0;
        break;
    }
}

// Handles sio commands
void sioInstructions() {
    if(ir.m == 1)
        printf("%d\n", registers[ir.r]);
    else if(ir.m == 2)
        registers[ir.r] = stack[sp];
    else if(ir.r == 0 && ir.m == 3) {
        haltFlag = 1;
    }
}

// Updates instruction register and pc is incremented
// Returns index of current instruction
int fetchInstruction() {
    ir.op = instructions[pc].op;
    ir.r = instructions[pc].r;
    ir.l = instructions[pc].l;
    ir.m = instructions[pc].m;
    pc++;

    return (pc - 1);
}

// Read in input to instructions[] and prints input values
int readFile(char* fileName) {
    char line[12];
    int i = 0;
    int len = 0;

    // change to fileName when done
    fp = fopen(fileName, "r");

    if(fp == NULL) {
        printf("Error: File was not found");
        return -1;
    }

    // scan in and print instructions
    // Doesn't like leading zeros
    while (fgets(line, sizeof(line), fp)) {
        //sscanf(line, "%2s %2s %2s %2s", op, r, l, m);

        sscanf(line, "%d %d %d %d", &instructions[i].op, &instructions[i].r, &instructions[i].l, &instructions[i].m);
        //printf("%d %d %d %d\n", instructions[i].op, instructions[i].r, instructions[i].l, instructions[i].m);

        // if fgets read in some bullshit-ass 0 0 0 0 line, we finna ignore and write over that bitch
        if(instructions[i].op == 0 && instructions[i].r == 0 && instructions[i].l == 0 && instructions[i].m == 0){
            i--;
        }

        i++;
    }
    len = i;

    fclose(fp);

    return i;
}

// l stand for L in the instruction format
int base(int l, int base) {
    int b1;
    //find base L levels down
    b1 = base;
    while (l > 0) {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}
