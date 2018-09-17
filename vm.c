/*
* Brian Kaine Margretta
* Cop3402 Systems Software
* This program implements a virtual machine in C
*/

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "data.h"

void initVM(VirtualMachine*);

int readInstructions(FILE*, Instruction*);

void dumpInstructions(FILE*, Instruction*, int numOfIns);

int getBasePointer(int *stack, int currentBP, int L);

void dumpStack(FILE*, int* stack, int sp, int bp);

int executeInstruction(VirtualMachine* vm, Instruction insi, FILE* vmIn, FILE* vmOut);

// Allows conversion from opcode to opcode string
const char *opcodes[] = 
{
    "illegal", // opcode 0 is illegal
    "lit", "rtn", "lod", "sto", "cal", // 1, 2, 3 ..
    "inc", "jmp", "jpc", "sio", "sio",
    "sio", "neg", "add", "sub", "mul",
    "div", "odd", "mod", "eql", "neq",
    "lss", "leq", "gtr", "geq"
};
// Conditions
enum { CONT, HALT };

// Initialize Virtual Machine
// Since vm was allocated using calloc, just initilize BP to 1
void initVM(VirtualMachine* vm)
{
    if(vm)
    {
        vm->BP = 1;
    }
}

 // Fill the (ins)tructions array by reading instructions from (in)put file
 // Return the number of instructions read
int readInstructions(FILE* in, Instruction* ins)
{
    int i = 0;
    while(fscanf(in, "%d %d %d %d", &ins[i].op, &ins[i].r, &ins[i].l, &ins[i].m) != EOF)
    {
        i++;
    }
    return i;
}

 // Dump instructions to the output file with formatting
void dumpInstructions(FILE* out, Instruction* ins, int numOfIns)
{
    fprintf(out,"***Code Memory***\n%3s %3s %3s %3s %3s \n","#", "OP", "R", "L", "M" );

    // Instructions
    int i;
    for(i = 0; i < numOfIns; i++)
    {
        fprintf(out,"%3d %3s %3d %3d %3d \n",i, opcodes[ins[i].op], ins[i].r, ins[i].l, ins[i].m);
    }
}

 // Returns the base pointer for the lexiographic level L
int getBasePointer(int *stack, int currentBP, int L)
{
    return 0;
}

// Function that dumps the whole stack into output file
// Do not forget to use '|' character between stack frames
void dumpStack(FILE* out, int* stack, int sp, int bp)
{
    if(bp == 0)
        return;

    // bottom-most level, where a single zero value lies
    if(bp == 1)
        fprintf(out, "%3d ", 0);

    // former levels - if exists
    if(bp != 1)
        dumpStack(out, stack, bp - 1, stack[bp + 2]);            

    // top level: current activation record
    if(bp <= sp)
    {
        // indicate a new activation record
        fprintf(out, "| ");
        
        // print the activation record
        int i;
        for(i = bp; i <= sp; i++)
            fprintf(out, "%3d ", stack[i]);
    }
}

 // Executes the (ins)truction on the (v)irtual (m)achine.
 // This changes the state of the virtual machine.
 // Returns HALT if the executed instruction was meant to halt the VM.
 // .. Otherwise, returns CONT
 // ins has op,r,l,m
 // vm has BP,SP,PC,IR,RF,stack
int executeInstruction(VirtualMachine* VM, Instruction insi, FILE* vmIn, FILE* vmOut)
{
    switch(insi.op)
    {
      case 1: //LIT
      {
        VM->RF[insi.r] = insi.m;
        break;
      }
      case 2: // RTN
      {
        VM->SP = VM->BP - 1;
        VM->BP = VM->stack[VM->SP+3];
        VM->PC = VM->stack[VM->SP+4];
        break;
      }
      case 3: // LOD
      {
        VM->RF[insi.r] = VM->stack[getBasePointer(VM->stack,insi.l,VM->BP) + insi.m];
        break;
      }
      case 4: // STO
      {
        VM->stack[getBasePointer(VM->stack,insi.l,VM->BP) + insi.m] = insi.r;
        break;
      }
      case 5: // CAL
      {
        VM->stack[VM->SP+1] = 0;
        VM->stack[VM->SP+2] = getBasePointer(VM->stack,insi.l,VM->BP);
        VM->stack[VM->SP+3] = VM->BP;
        VM->stack[VM->SP+4] = VM->PC;
        VM->BP = VM->SP+1;
        VM->PC = insi.m;
        break;
      }
      case 6: // INC
      {
        VM->SP = VM->SP + insi.m;
        break;
      }
      case 7:// JMP
      {
        VM->PC = insi.m;
        break;
      }
      case 8: // JPC
      {
        if(VM->RF[insi.r] == 0)
          VM->PC = insi.m;
        break;
      }
      case 9: // SIO
      {
        fprintf(vmOut,"%d ",VM->RF[insi.r]);
        break;
      }
      case 10: // SIO
      {
        int buffer[1];
        fscanf(vmIn,"%d",buffer);
        VM->RF[insi.r] = buffer[0];
        break; //read??
      }
      case 11: // SIO
      {
        return HALT;
        break;
      }
      case 12: // NEG
      {
        VM->RF[insi.r] = -VM->RF[insi.l];
        break;
      }
      case 13: // ADD
      {
        VM->RF[insi.r] = VM->RF[insi.l] + VM->RF[insi.m];
        break;
      }
      case 14: // SUB
      {
        VM->RF[insi.r] = VM->RF[insi.l] - VM->RF[insi.m];
        break;
      }
      case 15: // MUL
      {
        VM->RF[insi.r] = VM->RF[insi.l] * VM->RF[insi.m];
        break;
      }
      case 16: // DIV
      {
        VM->RF[insi.r] = VM->RF[insi.l] / VM->RF[insi.m];
        break;
      }
      case 17: // ODD
      {
        VM->RF[insi.r] = VM->RF[insi.r] % 2;
        break;
      }
      case 18: // MOD
      {
        VM->RF[insi.r] = VM->RF[insi.l] % VM->RF[insi.m];
        break;
      }
      case 19: // EQL
      {
        if(VM->RF[insi.l] == VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
      case 20: // NEQ
      {
        if(VM->RF[insi.l] != VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
      case 21: // LSS
      {
        if(VM->RF[insi.l] < VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
      case 22: // LEQ
      {
        if(VM->RF[insi.l] <= VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
      case 23: // GTR
      {
        if(VM->RF[insi.l] > VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
      case 24: // GEQ
      {
        if(VM->RF[insi.l] >= VM->RF[insi.m])
          VM->RF[insi.r] = 1;
        else
          VM->RF[insi.r] = 0;
        break;
      }
        default:
            fprintf(stderr, "Illegal instruction?");
            return HALT;
    }

    return CONT;
}

/**
 * inp: The FILE pointer containing the list of instructions to
 *         be loaded to code memory of the virtual machine.
 * 
 * outp: The FILE pointer to write the simulation output, which
 *       contains both code memory and execution history.
 * 
 * vm_inp: The FILE pointer that is going to be attached as the input
 *         stream to the virtual machine. Useful to feed input for SIO
 *         instructions.
 * 
 * vm_outp: The FILE pointer that is going to be attached as the output
 *          stream to the virtual machine. Useful to save the output printed
 *          by SIO instructions.
 * */
void simulateVM(
    FILE* inp,
    FILE* outp,
    FILE* vm_inp,
    FILE* vm_outp
    )
{
    
    // Allocate array of instructions, initiate all values to 0
    Instruction* insArray = calloc(MAX_CODE_LENGTH,sizeof(Instruction));
    // Get number of instructions
    int nInstructions = readInstructions(inp,insArray);

    // Dump instructions to the output file
    dumpInstructions(outp,insArray,nInstructions);

    // Before starting the code execution on the virtual machine,
    // .. write the header for the simulation part (***Execution***)
    fprintf(outp, "\n***Execution***\n");
    fprintf(outp,"%3s %3s %3s %3s %3s %3s %3s %3s %3s \n","#","OP","R","L","M","PC","BP","SP","STK");

    // Create a virtual machine, initilize values to 0(BP to 1)
    VirtualMachine* vm = calloc(1,sizeof(VirtualMachine));
    initVM(vm);

    int flag = CONT;
    // Fetch&Execute the instructions on the virtual machine until halting
    while( flag == CONT )
    {
        // Fetch
        Instruction insi = insArray[vm->PC];
        vm->IR = vm->PC;
        vm->PC++; // Advance PC

        // Execute the instruction
        flag = executeInstruction(vm,insi,vm_inp,vm_outp);

        // Print current state 
         fprintf(outp,"%3d %3s %3d %3d %3d %3d %3d %3d ",vm->IR,opcodes[insi.op],insi.r,insi.l,insi.m,vm->PC,vm->BP,vm->SP);

        // Print 0 for empty stack
          fprintf(outp,"%3d",0);
          
        // Print out rest of the stack  
        if(vm->SP > vm->BP)
        {
          fprintf(outp," | ");
          int j;
           fprintf(outp,"%3d",0);
          for(j = 1;j<vm->SP; j++)
          {
            fprintf(outp,"%3d ",vm->stack[vm->SP]);
          }
        }
        fprintf(outp, "\n");
  }
  fprintf(outp,"HLT\n");
  return;
}
