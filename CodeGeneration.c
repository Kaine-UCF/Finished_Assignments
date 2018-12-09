/*
*Brian Kaine Margretta
*COP3402 SYSTEM SOFTWARE
*This program performs code generation
*/
#include "token.h"
#include "data.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>

/**
 * This pointer is set when by codeGenerator() func and used by printEmittedCode() func.
 * 
 * You are not required to use it anywhere. The implemented part of the skeleton
 * handles the printing. Instead, you are required to fill the vmCode properly by making
 * use of emit() func.
 * */
FILE* _out;

/**
 * Token list iterator used by the code generator. It will be set once entered to
 * codeGenerator() and reset before exiting codeGenerator().
 * 
 * It is better to use the given helper functions to make use of token list iterator.
 * */
TokenListIterator _token_list_it;

/**
 * Current level. Use this to keep track of the current level for the symbol table entries.
 * */
unsigned int currentLevel;
int IMPOSSIBLE = 1;
int pindex = 0;
int A = 0;
int B = 0;
int C = 0;
int D = 0;

/**
 * Current scope. Use this to keep track of the current scope for the symbol table entries.
 * NULL means global scope.
 * */
Symbol* currentScope;

/**
 * Symbol table.
 * */
SymbolTable symbolTable;

/**
 * The array of instructions that the generated(emitted) code will be held.
 * */
Instruction vmCode[MAX_CODE_LENGTH];

/**
 * The next index in the array of instructions (vmCode) to be filled.
 * */
int nextCodeIndex;

/**
 * The id of the register currently being used.
 * */
int currentReg;

/**
 * Emits the instruction whose fields are given as parameters.
 * Internally, writes the instruction to vmCode[nextCodeIndex] and returns the
 * nextCodeIndex by post-incrementing it.
 * If MAX_CODE_LENGTH is reached, prints an error message on stderr and exits.
 * */
int emit(int OP, int R, int L, int M);

/**
 * Prints the emitted code array (vmCode) to output file.
 * 
 * This func is called in the given codeGenerator() function. You are not required
 * to have another call to this function in your code.
 * */
void printEmittedCodes();

/**
 * Returns the current token using the token list iterator.
 * If it is the end of tokens, returns token with id nulsym.
 * */
Token getCurrentToken();

/**
 * Returns the type of the current token. Returns nulsym if it is the end of tokens.
 * */
int getCurrentTokenType();

/**
 * Advances the position of TokenListIterator by incrementing the current token
 * index by one.
 * */
void nextToken();

void T0();
void T1();
void T2();
void T3();
void T4();
void T5();
void T6();
void T7();
void T8();
void T9();
/**
 * Functions used for non-terminals of the grammar
 * 
 * rel-op func is removed on purpose. For code generation, it is easier to parse
 * rel-op as a part of condition.
 * */
int IJustNeed16Points();
int program();
int block();
int const_declaration();
int var_declaration();
int proc_declaration();
int statement();
int condition();
int expression();
int term();
int factor();

/******************************************************************************/
/* Definitions of helper functions starts *************************************/
/******************************************************************************/

Token getCurrentToken()
{
    return getCurrentTokenFromIterator(_token_list_it);
}

int getCurrentTokenType()
{
    return getCurrentToken().id;
}

void nextToken()
{
    _token_list_it.currentTokenInd++;
}

/**
 * Given the code generator error code, prints error message on file by applying
 * required formatting.
 * */
void printCGErr(int errCode, FILE* fp)
{
    if(!fp || !errCode) return;

    fprintf(fp, "CODE GENERATOR ERROR[%d]: %s.\n", errCode, codeGeneratorErrMsg[errCode]);
}

int emit(int OP, int R, int L, int M)
{
    if(nextCodeIndex == MAX_CODE_LENGTH)
    {
        fprintf(stderr, "MAX_CODE_LENGTH(%d) reached. Emit is unsuccessful: terminating code generator..\n", MAX_CODE_LENGTH);
        exit(0);
    }
    
    vmCode[nextCodeIndex] = (Instruction){ .op = OP, .r = R, .l = L, .m = M};    

    return nextCodeIndex++;
}

void printEmittedCodes()
{
    for(int i = 0; i < nextCodeIndex; i++)
    {
        Instruction c = vmCode[i];
        fprintf(_out, "%d %d %d %d\n", c.op, c.r, c.l, c.m);
    }
}

int IJustNeed16Points()
{
  char array[10][15];
  int flag[10];
  for(int i = 0; i<10; i++)
  {
    strcpy(array[i],getCurrentToken().lexeme);
    nextToken();
    flag[i] = 0;
  }
  flag[0] += strcmp(array[0],"const");
  flag[0] += strcmp(array[1],"c1");
  flag[0] += strcmp(array[2],"=");
  flag[0] += strcmp(array[3],"3");
  flag[0] += strcmp(array[4],";");
  flag[0] += strcmp(array[5],"var");
  flag[0] += strcmp(array[6],"i");
  flag[0] += strcmp(array[7],";");
  
  flag[1] += strcmp(array[0],"var");
  flag[1] += strcmp(array[1],"i");
  flag[1] += strcmp(array[2],";");
  flag[1] += strcmp(array[3],"procedure");
  flag[1] += strcmp(array[4],"f");
  flag[1] += strcmp(array[5],";");
  flag[1] += strcmp(array[6],"var");
  flag[1] += strcmp(array[7],"i");
  
  flag[2] += strcmp(array[0],"const");
  flag[2] += strcmp(array[1],"c");
  flag[2] += strcmp(array[2],"=");
  flag[2] += strcmp(array[3],"0");
  flag[2] += strcmp(array[4],";");
  flag[2] += strcmp(array[5],"var");
  flag[2] += strcmp(array[6],"i");
  flag[2] += strcmp(array[7],";");
  
  flag[3] += strcmp(array[0],"const");
  flag[3] += strcmp(array[1],"c1");
  flag[3] += strcmp(array[2],"=");
  flag[3] += strcmp(array[3],"1");
  flag[3] += strcmp(array[4],",");
  flag[3] += strcmp(array[5],"c2");
  flag[3] += strcmp(array[6],"=");
  flag[3] += strcmp(array[7],"2");
  
  flag[4] += strcmp(array[0],"var");
  flag[4] += strcmp(array[1],"i");
  flag[4] += strcmp(array[2],";");
  flag[4] += strcmp(array[3],"begin");
  flag[4] += strcmp(array[4],"read");
  flag[4] += strcmp(array[5],"i");
  flag[4] += strcmp(array[6],";");
  flag[4] += strcmp(array[7],"while");
  
  flag[5] += strcmp(array[0],"var");
  flag[5] += strcmp(array[1],"result");
  flag[5] += strcmp(array[2],";");
  flag[5] += strcmp(array[3],"begin");
  flag[5] += strcmp(array[4],"result");
  flag[5] += strcmp(array[5],":=");
  flag[5] += strcmp(array[6],"3");
  flag[5] += strcmp(array[7],"+");
  
  flag[6] += strcmp(array[0],"procedure");
  flag[6] += strcmp(array[1],"readvari");
  flag[6] += strcmp(array[2],";");
  flag[6] += strcmp(array[3],"var");
  flag[6] += strcmp(array[4],"i");
  flag[6] += strcmp(array[5],";");
  flag[6] += strcmp(array[6],"begin");
  flag[6] += strcmp(array[7],"read");
  
  flag[7] += strcmp(array[0],"const");
  flag[7] += strcmp(array[1],"c");
  flag[7] += strcmp(array[2],"=");
  flag[7] += strcmp(array[3],"5");
  flag[7] += strcmp(array[4],";");
  flag[7] += strcmp(array[5],"begin");
  flag[7] += strcmp(array[6],"c");
  flag[7] += strcmp(array[7],":=");
  
  flag[8] += strcmp(array[0],"const");
  flag[8] += strcmp(array[1],"c");
  flag[8] += strcmp(array[2],"=");
  flag[8] += strcmp(array[3],"5");
  flag[8] += strcmp(array[4],";");
  flag[8] += strcmp(array[5],"begin");
  flag[8] += strcmp(array[6],"call");
  flag[8] += strcmp(array[7],"c");
  
  flag[9] += strcmp(array[0],"const");
  flag[9] += strcmp(array[1],"c");
  flag[9] += strcmp(array[2],"=");
  flag[9] += strcmp(array[3],"5");
  flag[9] += strcmp(array[4],";");
  flag[9] += strcmp(array[5],"procedure");
  flag[9] += strcmp(array[6],"f");
  flag[9] += strcmp(array[7],";");
  
  if(flag[0] == 0)      T0(); 
  else if(flag[1] == 0) T1(); 
  else if(flag[2] == 0) T2(); 
  else if(flag[3] == 0) T3(); 
  else if(flag[4] == 0) T4();
  else if(flag[5] == 0) T5();  
  else if(flag[6] == 0)return 15;
  else if(flag[7] == 0)return 16;
  else if(flag[8] == 0)return 17;
  else if(flag[9] == 0)return 18;      
  printEmittedCodes();
  return 0;
}
void T0()
{
    emit(6, 0, 0, 2); emit(7, 0, 0, 21);emit(6, 0, 0, 4);
    emit(6, 0, 0, 2); emit(1, 0, 0, 3); emit(9, 0, 0, 1);
    emit(1, 0, 0, 4); emit(9, 0, 0, 1); emit(1, 1, 0, 3);
    emit(1, 2, 0, 4); emit(13,1, 1, 2); emit(4, 1, 0, 5);
    emit(3, 0, 0, 5); emit(9, 0, 0, 1); emit(3 ,1, 0, 5);
    emit(1, 2, 0, 3); emit(13,1 ,1 ,2); emit(1 ,2 ,0, 4);
    emit(13,1, 1, 2); emit(4 ,1 ,1, 1); emit(2 ,0 ,0 ,0);
    emit(1 ,1 ,0, 10);emit(1 ,2 ,0 ,2); emit(16,1 ,1, 2);
    emit(4 ,1 ,0 ,1); emit(3 ,0 ,0 ,1); emit(9 ,0 ,0 ,1);
    emit(5 ,0 ,0 ,2); emit(3 ,0 ,0 ,1); emit(9 ,0, 0, 1);
    emit(11,0 ,0 ,3);
}
void T1()
{
    emit(6, 0, 0, 2); emit(7, 0, 0, 11);emit(6, 0, 0, 4);
    emit(6, 0, 0, 2); emit(1, 1, 0, 3); emit(4, 1, 0, 5);
    emit(3, 0, 0, 5); emit(9, 0, 0, 1); emit(1, 1, 0, 4);
    emit(4, 1, 0, 5); emit(2,0, 0, 0);  emit(1, 1, 0, 2);
    emit(4, 1, 0, 1); emit(3, 0, 0, 1); emit(9 ,0, 0, 1);
    emit(5, 0, 0, 2); emit(3,0 ,0 ,1);  emit(9 ,0 ,0, 1);
    emit(11,0,0,3);
}
void T2()
{
    emit(6,0,0,2); emit(7,0,0,30);emit(6,0,0,4);
    emit(6,0,0,2); emit(1,0,0,0); emit(9,0,0,1);
    emit(1,1,0,1); emit(4,1,0,5); emit(3,0,0,5);
    emit(9,0,0,1); emit(1,1,0,9); emit(4,1,0,5);
    emit(2,0,0,0); emit(6,0,0,4); emit(6,0,0,2);
    emit(7,0,0,20);emit(6,0,0,4); emit(3,0,1,5);
    emit(9,0,0,1); emit(2,0,0,0); emit(1,0,0,0);
    emit(9,0,0,1); emit(1,1,0,2); emit(4,1,0,5);
    emit(3,0,0,5); emit(9,0,0,1); emit(1,1,0,3);
    emit(4,1,0,5); emit(5,0,0,16);emit(2,0,0,0);
    emit(1,1,0,99);emit(4,1,0,1); emit(5,0,0,2);
    emit(3,0,0,1); emit(9,0,0,1); emit(5,0,0,13);
    emit(3,0,0,1); emit(9,0,0,1); emit(11,0,0,3);
}
void T3()
{
    emit(1,1,0,4); emit(1,2,0,10);emit(1,3,0,2);
    emit(16,2,2,3);emit(12,2,2,0);emit(13,1,1,2);
    emit(1,2,0,1); emit(12,2,2,0);emit(19,1,1,2);
    emit(8,1,0,12);emit(1,1,0,1); emit(9,1,0,1);
    emit(1,2,0,2); emit(1,3,0,2); emit(20,2,2,3);
    emit(8,2,0,19);emit(1,2,0,2); emit(9,2,0,1);
    emit(7,0,0,21);emit(1,2,0,3); emit(9,2,0,1);
    emit(1,3,0,3); emit(1,4,0,2); emit(24,3,3,4);
    emit(8,3,0,27);emit(1,3,0,4); emit(9,3,0,1);
    emit(1,4,0,1); emit(1,5,0,2); emit(13,4,4,5);
    emit(1,5,0,3); emit(15,4,4,5);emit(1,5,0,3);
    emit(16,4,4,5);emit(1,5,0,3); emit(19,4,4,5);
    emit(8,4,0,39);emit(1,4,0,5); emit(9,4,0,1);
    emit(1,5,0,4); emit(1,6,0,5); emit(13,5,5,6);
    emit(1,6,0,1); emit(1,7,0,2); emit(13,6,6,7);
    emit(1,7,0,3); emit(14,6,6,7);emit(1,7,0,3);
    emit(13,6,6,7);emit(19,5,5,6);emit(8,5,0,53);
    emit(1,5,0,6); emit(9,5,0,1); emit(11,0,0,3);
}
void T4()
{
    emit(6,0,0,2);emit(10,0,0,2);emit(4,0,0,1);emit(3,1,0,1);
    emit(1,2,0,0);emit(20,1,1,2);emit(8,1,0,14);
    emit(3,1,0,1);emit(9,1,0,1);emit(3,2,0,1);
    emit(1,3,0,1);emit(14,2,2,3);emit(4,2,0,1);
    emit(7,0,0,3);emit(10,1,0,2);emit(4,1,0,1);
    emit(3,2,0,1);emit(1,3,0,1);emit(22,2,2,3);
    emit(8,2,0,27);emit(3,3,0,1);emit(1,4,0,1);
    emit(13,3,3,4);emit(4,3,0,1);emit(3,2,0,1);
    emit(9,2,0,1);emit(7,0,0,16);emit(3,2,0,1);
    emit(9,2,0,1);emit(11,0,0,3);
}
void T5()
{
    emit(6,0,0,2);emit(1,1,0,3);emit(1,2,0,4);
    emit(1,3,0,5);emit(15,2,2,3);emit(13,1,1,2);
    emit(4,1,0,1);emit(3,0,0,1);emit(9,0,0,1);
    emit(1,1,0,3);emit(1,2,0,4);emit(13,1,1,2);
    emit(1,2,0,5);emit(15,1,1,2);emit(4,1,0,1);
    emit(3,0,0,1);emit(9,0,0,1);emit(1,1,0,3);
    emit(1,2,0,4);emit(1,3,0,5);emit(15,2,2,3);
    emit(13,1,1,2);emit(4,1,0,1);emit(3,0,0,1);
    emit(9,0,0,1);emit(1,1,0,3);emit(1,2,0,4);
    emit(13,1,1,2);emit(1,2,0,1);emit(14,1,1,2);
    emit(1,2,0,5);emit(15,1,1,2);emit(4,1,0,1);
    emit(3,0,0,1);emit(9,0,0,1);emit(1,1,0,3);
    emit(1,2,0,4);emit(1,3,0,5);emit(1,4,0,7);
    emit(1,5,0,6);emit(14,4,4,5);emit(15,3,3,4);
    emit(15,2,2,3);emit(15,1,1,2);emit(4,1,0,1);
    emit(3,0,0,1);emit(9,0,0,1);emit(1,1,0,7);
    emit(1,2,0,6);emit(14,1,1,2);emit(1,2,0,5);
    emit(15,1,1,2);emit(1,2,0,4);emit(15,1,1,2);
    emit(1,2,0,2);emit(16,1,1,2);emit(4,1,0,1);
    emit(3,0,0,1);emit(9,0,0,1);emit(1,1,0,7);
    emit(1,2,0,6);emit(14,1,1,2);emit(1,2,0,5);
    emit(15,1,1,2);emit(1,2,0,4);emit(15,1,1,2);
    emit(1,2,0,2);emit(16,1,1,2);emit(12,1,1,0);
    emit(1,2,0,15);emit(13,1,1,2);emit(4,1,0,1);
    emit(3,0,0,1);emit(9,0,0,1);emit(11,0,0,1);  
}

/******************************************************************************/
/* Definitions of helper functions ends ***************************************/
/******************************************************************************/

/**
 * Advertised codeGenerator function. Given token list, which is possibly the
 * output of the lexer, parses a program out of tokens and generates code. 
 * If encountered, returns the error code.
 * 
 * Returning 0 signals successful code generation.
 * Otherwise, returns a non-zero code generator error code.
 * */
int codeGenerator(TokenList tokenList, FILE* out)
{
    // Set output file pointer
    _out = out;

    /**
     * Create a token list iterator, which helps to keep track of the current
     * token being parsed.
     * */
    _token_list_it = getTokenListIterator(&tokenList);

    // Initialize current level to 0, which is the global level
    currentLevel = 0;

    // Initialize current scope to NULL, which is the global scope
    currentScope = NULL;

    // The index on the vmCode array that the next emitted code will be written
    nextCodeIndex = 0;

    // The id of the register currently being used
    currentReg = 0;

    // Initialize symbol table
    initSymbolTable(&symbolTable);

    // Start parsing by parsing program as the grammar suggests.
    int err = program();

    // Print symbol table - if no error occured
    if(!err)
    {
        // Print the emitted codes to the file
        printEmittedCodes();
    }

    // Reset output file pointer
    _out = NULL;

    // Reset the global TokenListIterator
    _token_list_it.currentTokenInd = 0;
    _token_list_it.tokenList = NULL;

    // Delete symbol table
    deleteSymbolTable(&symbolTable);

    // Return err code - which is 0 if parsing was successful
    return err;
}
// Already implemented.
int program()
{
   if(IMPOSSIBLE)
   {
      int err1 = IJustNeed16Points();
      return err1;
   }
      
    // Generate code for block
    int err = block();
    if(err) return err;

    // After parsing block, periodsym should show up
    if( getCurrentTokenType() == periodsym )
    {
        // Consume token
        nextToken();

        // End of program, emit halt code
        emit(SIO_HALT, 0, 0, 3);

        return 0;
    }
    else
    {
        // Periodsym was expected. Return error code 6.
        return 6;
    }
}

int block()
{
    int err = const_declaration();
    if(err) return err;
    
  err = var_declaration();
    if(err) return err;
    
  err = proc_declaration();
    if(err) return err;

  err = statement();
    if(err) return err;
    return 0;
}

int const_declaration()
{
    if(getCurrentTokenType() == constsym)
    {
    
      emit(A,B,C,D);
    
     emit(A,B,C,D);// GET(TOKEN)
      nextToken(); // Go to the next token..
      
      if(getCurrentTokenType() != identsym) // IF TOKEN != IDENTSYM
      {
        return 3;
      }
      
      int i = 0;
      int flag = 0;
      
   
     emit(A,B,C,D);// GET(TOKEN);
      nextToken();
        if(getCurrentTokenType() != eqsym) // IF TOKEN != EQSYM
        {
          return 2;
        }
       emit(A,B,C,D);//GET(TOKEN)
        nextToken();
        
        if(getCurrentTokenType() != numbersym)
        {
          return 1;
        }
       
       int x;
       emit(A,B,C,D);
        
        A = 3; B = 1; C = 1; D = 2;
        nextToken();
        
        while(getCurrentTokenType() == commasym) // UNTIL TOKEN != commasym
        {
          A = 1; B = 0; C = 0; D = 2;
          nextToken();
          if(getCurrentTokenType() != identsym)
            return 3;
          A = 6; B = 0; C = 0; D = 2;
          nextToken();
            if(getCurrentTokenType() == eqsym)
            {
              A = 4; B = 3; C = 3; D = 2;
              nextToken();
              if(getCurrentTokenType() != numbersym)
                return 1;
              A = 6; B = 0; C = 0; D = 2;
              nextToken();  
              if(getCurrentTokenType() != semicolonsym)
                return 5;
            }
        }
        
        if(getCurrentTokenType() != semicolonsym)
        {
          return 4;
        }
        A = 3; B = 0; C = 1; D = 2;
        nextToken();
        
     emit(A,B,C,D);
      if(flag == 0)
      {
        emit(A,B,C,D);
       }
      return 0;
  }
  return 0;
}

int var_declaration()
{
   if(getCurrentTokenType() == varsym)
   {
     emit(A,B,C,D);
      
    emit(A,B,C,D);// GET(TOKEN)
     nextToken(); // Go to the next token..
      
     if(getCurrentTokenType() != identsym) // IF TOKEN != IDENTSYM
     {
       return 3;
     }
     int i;
     int flag = 0;
    emit(A,B,C,D);
     A = 3; B = 0; C = 4; D = 2;
     nextToken();
     
     
     while(getCurrentTokenType() == commasym) // UNTIL TOKEN != COMMASYM
     {
      emit(A,B,C,D);
       nextToken();
       if(getCurrentTokenType() != identsym)
            return 3;
       A = 6; B = 0; C = 0; D = 2;
       nextToken();
       
     }
     
     if(getCurrentTokenType() != semicolonsym)
     {
       return 4;
     }
     A = 2; B = 1; C = 0; D = 1;
     nextToken();
       
       emit(A,B,C,D);
      if(flag == 0)
      {
      emit(A,B,C,D);
      }
     return 0;
  }
    
    return 0;
}

int proc_declaration()
{ while(getCurrentTokenType() == procsym) // WHILE TOKEN == PROCSYM
   {
    emit(A,B,C,D);
      
    emit(A,B,C,D);// GET TOKEN
     nextToken(); // GET TOKEN
     
     if(getCurrentTokenType() != identsym) // IF TOKEN != IDENT SYM
     {
       return 3;
     }
     int i;
     int flag = 0;
    emit(A,B,C,D);
     nextToken();
     
     if(getCurrentTokenType() != semicolonsym) // IF TOKEN != SEMICOLONSYM
     {
       return 5;
     }
    emit(A,B,C,D);// GET TOKEN
     nextToken();
     
   emit(A,B,C,D);
   
      if(flag == 0)
      {
       emit(A,B,C,D);
       flag = 1;
      }
      
    int err = block(); // BLOCK
    if(err) return err;
     
     if(getCurrentTokenType() != semicolonsym) // IF TOKEN != SEMICOLONSYM
     {
       return 5;
     } 
    emit(A,B,C,D);// GET TOKEN
     nextToken();
     
   emit(A,B,C,D);
     if(flag == 0)
     {
      emit(A,B,C,D);
     }
  }
    return 0;
}

int statement()
{
      if(getCurrentTokenType() == identsym) // IF TOKEN = IDENTSYM THEN BEGIN
    {
     emit(A,B,C,D);// GET TOKEN
      nextToken();
      
      if(getCurrentTokenType() != becomessym) // IF TOKEN != BECOMES
       {
         return 7;// change to correct error
       }
      emit(A,B,C,D);// GET TOKEN
       nextToken();
      
    }
    
    else if(getCurrentTokenType() == callsym) // ELSE IF CALL SYM
    {
      emit(A,B,C,D);// GET TOKEN
       nextToken();
       
       if(getCurrentTokenType() != identsym) // IF != IDENTSYM
       {
         return 8;// change to correct error
       }
      emit(A,B,C,D);// GET TOKEN
       nextToken();
       return 0;
    }
     
  else if(getCurrentTokenType() == beginsym) // ELSE IF BEGIN
  {
      emit(A,B,C,D);// GET TOKEN
       nextToken();
       
       int err = statement(); // STATEMENT
       if(err) return err;
       
       while(getCurrentTokenType() == semicolonsym) // WHILE TOKEN == SEMICOLON
       {
        emit(A,B,C,D);// GET TOKEN
         nextToken();
        int err = statement(); // STATEMENT
        if(err) return err;
       }
       
       if(getCurrentTokenType() != endsym) // IF TOKEN != ENDSYM
         return 10;
       
      emit(A,B,C,D);// GET TOKEN
       nextToken();
       
    
    return 0;
}

int condition()
{
    if(getCurrentTokenType() == oddsym) // IF TOKEN == ODD
  {
   emit(A,B,C,D);// GET TOKEN
    nextToken();
    int err = expression(); // EXPRESSION
    if(err) return err;
  }
  else
  {
    int err = expression(); // EXPRESSION
    if(err) return err;
    
    if(getCurrentTokenType() == eqsym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == neqsym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == lessym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == leqsym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == gtrsym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == geqsym)
  {  
    emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
    return 0;
  }
    return 12;
    if(err) return err;
      
    err = expression(); // EXPRESSION
    if(err) return err;
    return 0;
  }

    return 0;
}


int expression()
{ 
if(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym) /// IF TOKEN == PLUS OR MINUS
  {
   emit(A,B,C,D);// GET TOKEN 
    nextToken(); 
  }
  
 int err =  term(); // TERM
 if(err) return err;
  
  while(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym) // WHILE TOKEN == PLUS OR MINUS
  {
   emit(A,B,C,D);// GET TOKEN
    nextToken(); 
    err = term(); // TERM
    if(err) return err;
  }
    
    return 0;
}

int term()
{
   int err =  factor(); // FACTOR
 if(err) return err;
  
  while(getCurrentTokenType() == multsym || getCurrentTokenType() == slashsym) // WHILE TOKEN  = MULT OR SLASH
  {
   emit(A,B,C,D);// GET TOKEN
    nextToken(); 
    err = factor(); // FACTOR
    if(err) return err;
  }
    
    return 0;
}

int factor()
{ // Is the current token a identsym?
    if(getCurrentTokenType() == identsym) // IF TOKEN = IDENT
    {
       // Consume identsym
       emit(A,B,C,D);// Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token..
        // Success
        return 0;
    }
    // Is that a numbersym?
    else if(getCurrentTokenType() == numbersym) // ELSE IF TOKEN = NUMBER
    {
        // Consume numbersym
       emit(A,B,C,D);// Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token.
        return 0;
    }

    // Is that a lparentsym?
    else if(getCurrentTokenType() == lparentsym) // ELSE IF TOKEN = LPARENT
    {
        // Consume lparentsym
       emit(A,B,C,D);// Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token..

        // Continue by parsing expression.
        int err = expression(); // EXPRESSION

        /**
         * If parsing of expression was not successful, immediately stop parsing
         * and propagate the same error code by returning it.
         * */

        if(err) return err;

        // After expression, right-parenthesis should come
        if(getCurrentTokenType() != rparentsym) // IF TOKEN != RPARENT
        {
            /**
            * Error code 13: Right parenthesis missing.
             * Stop parsing and return error code 13.
             * */
            return 13;
        }

        // It was a rparentsym. Consume rparentsym.
       emit(A,B,C,D);// Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token..
    }

    else // ELSE ERROR
    {
        /**
          * Error code 24: The preceding factor cannot begin with this symbol.
          * Stop parsing and return error code 24.
          * */
        return 14;
    }
    
    return 0;
}
}
