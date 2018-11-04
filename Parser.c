#include "token.h"
#include "data.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>
/**
 * This pointer is set when by parser() func and used by printParsedToken() func.
 * 
 *You are not required to use it anywhere. The implemented part of the skeleton
 * handles the printing. Instead, You could use the supplied helper functions to
 * manipulate the output file.
 * */
FILE* _out;
/**
 * Token list iterator used by the parser. It will be set once entered to parser()
 * and reset before exiting parser().
 * 
 * It is better to use the given helper functions to make use of token list iterator.
 * */
TokenListIterator _token_list_it;
/**
 * Current level.
 * */
unsigned int currentLevel;
/**
 * Symbol table.
 * */
SymbolTable symbolTable;
Symbol current_symbol[100];
int array_index = 0;
char previous[100][12];
int pindex = 0;

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
 * Prints the given token on _out by applying required formatting.
 * */
void printCurrentToken();
/**
 * Advances the position of TokenListIterator by incrementing the current token
 * index by one.
 * */
void nextToken();
/**
 * Given an entry from non-terminal enumaration, prints it.
 * */
void printNonTerminal(NonTerminal nonTerminal);
/**
 * Functions used for non-terminals of the grammar
 * */
int program();
int block();
int const_declaration();
int var_declaration();
int proc_declaration();
int statement();
int condition();
int relop();
int expression();
int term();
int factor();

Token getCurrentToken()
{
    return getCurrentTokenFromIterator(_token_list_it);
}

int getCurrentTokenType()
{
    return getCurrentToken().id;
}

void printCurrentToken()
{
    fprintf(_out, "%8s <%s, '%s'>\n", "TOKEN  :", tokenNames[getCurrentToken().id], getCurrentToken().lexeme);
}

void nextToken()
{
    _token_list_it.currentTokenInd++;
}

void printNonTerminal(NonTerminal nonTerminal)
{
    fprintf(_out, "%8s %s\n", "NONTERM:", nonTerminalNames[nonTerminal]);
}
/**
 * Given the parser error code, prints error message on file by applying
 * required formatting.
 * */
void printParserErr(int errCode, FILE* fp)
{
    if(!fp) return;
    if(!errCode)
        fprintf(fp, "\nPARSING WAS SUCCESSFUL.\n");
    else
        fprintf(fp, "\nPARSING ERROR[%d]: %s.\n", errCode, parserErrorMsg[errCode]);
}
/**
 * Advertised parser function. Given token list, which is possibly the output of 
 * the lexer, parses the tokens. If encountered, return the error code.
 * 
 * Returning 0 signals successful parsing.
 * Otherwise, returns a non-zero parser error code.
 * */
int parser(TokenList tokenList, FILE* out)
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
    // Initialize symbol table
    initSymbolTable(&symbolTable);

    // Write parsing history header
    fprintf(_out, "Parsing History\n===============\n");

    // Start parsing by parsing program as the grammar suggests.
    int err = program();

    // Print symbol table - if no error occured
    if(!err)
    {
        fprintf(_out, "\n\n");
        printSymbolTable(&symbolTable, _out);
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

int program()
{
int i = 0;
for(i = 0; i<100; i++)
{
  strcpy(previous[i],"@");
}
  printNonTerminal(PROGRAM);  //BEGIN
  
  getCurrentTokenType(); // get the first token
 
  int err = block(); // BLOCK
  if(err) return err;
  
  if(getCurrentTokenType() != periodsym) //if the last token is not a period, error
  {
    return 6;
  }
  
  printCurrentToken(); // Printing the period
  
  return 0; // end
}



int block()
{
  printNonTerminal(BLOCK);
  
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
    printNonTerminal(CONST_DECLARATION); // BEGIN
    if(getCurrentTokenType() == constsym)
    {
    
      Symbol current_sym;
      current_symbol[array_index].type = CONST;
    
      printCurrentToken(); // GET(TOKEN)
      nextToken(); // Go to the next token..
      
      if(getCurrentTokenType() != identsym) // IF TOKEN != IDENTSYM
      {
        return 3;
      }
      
      int i = 0;
      int flag = 0;
      
     strcpy(current_symbol[array_index].name,getCurrentToken().lexeme);
     strcpy(previous[pindex],getCurrentToken().lexeme);
      
      printCurrentToken(); // GET(TOKEN);
      nextToken();
        if(getCurrentTokenType() != eqsym) // IF TOKEN != EQSYM
        {
          return 2;
        }
        printCurrentToken(); //GET(TOKEN)
        nextToken();
        
        if(getCurrentTokenType() != numbersym)
        {
          return 1;
        }
       
       int x;
       sscanf(getCurrentToken().lexeme,"%d",&x);
       current_symbol[array_index].value = x;
        
        printCurrentToken();
        nextToken();
        
        while(getCurrentTokenType() == commasym) // UNTIL TOKEN != commasym
        {
          printCurrentToken();
          nextToken();
          if(getCurrentTokenType() != identsym)
            return 3;
          printCurrentToken();
          nextToken();
            if(getCurrentTokenType() == eqsym)
            {
              printCurrentToken();
              nextToken();
              if(getCurrentTokenType() != numbersym)
                return 1;
              printCurrentToken();
              nextToken();  
              if(getCurrentTokenType() != semicolonsym)
                return 5;
            }
        }
        
        if(getCurrentTokenType() != semicolonsym)
        {
          return 4;
        }
        printCurrentToken();
        nextToken();
        
        for(i = 0; i< pindex; i++)
      {
        if(strcmp(previous[i],previous[pindex]) == 0)
          flag = 1;
      }
      if(flag == 0)
      {
        pindex++;
       current_symbol[array_index].level = currentLevel;
       addSymbol(&symbolTable,current_symbol[array_index]);
       array_index++;
       }
       pindex++;
      return 0;
  }
  return 0;
}



int var_declaration() // TOKEN == INTSYM
{
   printNonTerminal(VAR_DECLARATION);
   if(getCurrentTokenType() == varsym)
   {
      current_symbol[array_index].type = VAR;
      
     printCurrentToken(); // GET(TOKEN)
     nextToken(); // Go to the next token..
      
     if(getCurrentTokenType() != identsym) // IF TOKEN != IDENTSYM
     {
       return 3;
     }
     int i;
     int flag = 0;
     strcpy(current_symbol[array_index].name,getCurrentToken().lexeme);
     strcpy(previous[pindex],getCurrentToken().lexeme);
     printCurrentToken();
     nextToken();
     
     
     while(getCurrentTokenType() == commasym) // UNTIL TOKEN != COMMASYM
     {
       printCurrentToken(); 
       nextToken();
       if(getCurrentTokenType() != identsym)
            return 3;
       printCurrentToken();
       nextToken();
       
     }
     
     if(getCurrentTokenType() != semicolonsym)
     {
       return 4;
     }
     printCurrentToken();
     nextToken();
       
        for(i = 0; i< pindex; i++)
      {
        if(strcmp(previous[i],previous[pindex]) == 0)
          flag = 1;
      }
      if(flag == 0)
      {
       pindex++;
      current_symbol[array_index].level = currentLevel;
      addSymbol(&symbolTable,current_symbol[array_index]);
      array_index++;
      }
      pindex++;
     return 0;
  }
  return 0;
}



int proc_declaration()
{
   printNonTerminal(PROC_DECLARATION); // BEGIN
   while(getCurrentTokenType() == procsym) // WHILE TOKEN == PROCSYM
   {
      current_symbol[array_index].type = PROC;
      
     printCurrentToken(); // GET TOKEN
     nextToken(); // GET TOKEN
     
     if(getCurrentTokenType() != identsym) // IF TOKEN != IDENT SYM
     {
       return 3;
     }
     int i;
     int flag = 0;
     strcpy(current_symbol[array_index].name,getCurrentToken().lexeme);
     strcpy(previous[pindex],getCurrentToken().lexeme);
     printCurrentToken(); // GET TOKEN
     nextToken();
     
     if(getCurrentTokenType() != semicolonsym) // IF TOKEN != SEMICOLONSYM
     {
       return 5;
     }
     printCurrentToken(); // GET TOKEN
     nextToken();
     
      for(i = 0; i< pindex; i++)
      {
        if(strcmp(previous[i],previous[pindex]) == 0)
          flag = 1;
      }
      if(flag == 0)
      {
        pindex++;
       current_symbol[array_index].level = currentLevel;
       addSymbol(&symbolTable,current_symbol[array_index]);
       array_index++;
       currentLevel++;
      }
      pindex++;
      flag = 0;
    int err = block(); // BLOCK
    if(err) return err;
     
     if(getCurrentTokenType() != semicolonsym) // IF TOKEN != SEMICOLONSYM
     {
       return 5;
     } 
     printCurrentToken(); // GET TOKEN
     nextToken();
     
       
     for(i = 0; i< pindex; i++)
      {
        if(strcmp(previous[i],previous[pindex]) == 0)
          flag = 1;
      }
     if(flag == 0)
     {
       pindex++;
       current_symbol[array_index].level = currentLevel;
       addSymbol(&symbolTable,current_symbol[array_index]);
       array_index++;
     }
     pindex++;
  }
  return 0;
}



int statement()
{
    printNonTerminal(STATEMENT); // BEGIN
    
    if(getCurrentTokenType() == identsym) // IF TOKEN = IDENTSYM THEN BEGIN
    {
      printCurrentToken(); // GET TOKEN
      nextToken();
      
      if(getCurrentTokenType() != becomessym) // IF TOKEN != BECOMES
       {
         return 7;// change to correct error
       }
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       int err = expression(); // EXPRESSION
       if(err) return err;
       return 0;
    }
    
    else if(getCurrentTokenType() == callsym) // ELSE IF CALL SYM
    {
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       if(getCurrentTokenType() != identsym) // IF != IDENTSYM
       {
         return 8;// change to correct error
       }
       printCurrentToken(); // GET TOKEN
       nextToken();
       return 0;
    }
     
  else if(getCurrentTokenType() == beginsym) // ELSE IF BEGIN
  {
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       int err = statement(); // STATEMENT
       if(err) return err;
       
       while(getCurrentTokenType() == semicolonsym) // WHILE TOKEN == SEMICOLON
       {
         printCurrentToken(); // GET TOKEN
         nextToken();
        int err = statement(); // STATEMENT
        if(err) return err;
       }
       
       if(getCurrentTokenType() != endsym) // IF TOKEN != ENDSYM
         return 10;
       
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       return 0;
  }
  
  else if(getCurrentTokenType() == ifsym) // ELSE IF TOKEN = IF
  {
     printCurrentToken(); // GET TOKEN
     nextToken();
     int err = condition(); // CONDITION
     if(err) return err;
     
     if(getCurrentTokenType() != thensym) // IF TOKEN != THEN
       return 9;
     
     printCurrentToken(); // GET TOKEN
     nextToken();
     err = statement(); // STATEMENT
     if(err) return err;
     
     if(getCurrentTokenType() == elsesym)
     {
       printCurrentToken(); // GET TOKEN
       nextToken();
       err = statement();
       if(err) return err;
     }
     return 0; 
  }
  
  else if(getCurrentTokenType() == whilesym) // ELSE IF TOKEN =  WHILE
  {
     printCurrentToken(); // GET TOKEN
     nextToken();
     int err = condition(); // CONDITION
     if(err) return err;
     
     if(getCurrentTokenType() != dosym) // IF TOKEN != DO
       return 11; // these are all still wrong
     
     printCurrentToken(); // GET TOKEN
     nextToken();
     err = statement(); // STATEMENT
     if(err) return err;
  }
  
    else if(getCurrentTokenType() == readsym) // ELSE IF read SYM
    {
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       if(getCurrentTokenType() != identsym) // IF != IDENTSYM
       {
         return 3;// change to correct error
       }
       printCurrentToken(); // GET TOKEN
       nextToken();
       return 0;
    }
  
    else if(getCurrentTokenType() == writesym) // ELSE IF write SYM
    {
       printCurrentToken(); // GET TOKEN
       nextToken();
       
       if(getCurrentTokenType() != identsym) // IF != IDENTSYM
       {
         return 3;// change to correct error
       }
       printCurrentToken(); // GET TOKEN
       nextToken();
       return 0;
    }
    return 0;
}



int condition() 
{
  printNonTerminal(CONDITION); //  BEGIN
  if(getCurrentTokenType() == oddsym) // IF TOKEN == ODD
  {
    printCurrentToken(); // GET TOKEN
    nextToken();
    int err = expression(); // EXPRESSION
    if(err) return err;
  }
  else
  {
    int err = expression(); // EXPRESSION
    if(err) return err;
    
    err = relop();
    if(err) return err;
      
    err = expression(); // EXPRESSION
    if(err) return err;
    return 0;
  }
  return 0;
}

int relop() // NO PSUEDO CODE???
{
  printNonTerminal(REL_OP); // check operaor, either reutrn error or 0
  
  if(getCurrentTokenType() == eqsym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == neqsym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == lessym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == leqsym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == gtrsym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
  
   if(getCurrentTokenType() == geqsym)
  {  
     printCurrentToken(); // GET TOKEN 
    nextToken(); 
    return 0;
  }
    return 12;
}

int expression() 
{
  printNonTerminal(EXPRESSION); // BEGIN

  if(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym) /// IF TOKEN == PLUS OR MINUS
  {
    printCurrentToken(); // GET TOKEN 
    nextToken(); 
  }
  
 int err =  term(); // TERM
 if(err) return err;
  
  while(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym) // WHILE TOKEN == PLUS OR MINUS
  {
    printCurrentToken(); // GET TOKEN
    nextToken(); 
    err = term(); // TERM
    if(err) return err;
  }
  return 0;
}



int term()
{
  printNonTerminal(TERM); // BEGIN
 int err =  factor(); // FACTOR
 if(err) return err;
  
  while(getCurrentTokenType() == multsym || getCurrentTokenType() == slashsym) // WHILE TOKEN  = MULT OR SLASH
  {
    printCurrentToken(); // GET TOKEN
    nextToken(); 
    err = factor(); // FACTOR
    if(err) return err;
  }
    return 0;
}



/**
 * The below function is left fully-implemented as a hint.
 * */
int factor()
{
  printNonTerminal(FACTOR); //BEGIN
    /**
     * There are three possibilities for factor:
     * 1) ident
     * 2) number
     * 3) '(' expression ')'
     * */
     
    // Is the current token a identsym?
    if(getCurrentTokenType() == identsym) // IF TOKEN = IDENT
    {
       // Consume identsym
        printCurrentToken(); // Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token..
        // Success
        return 0;
    }
    // Is that a numbersym?
    else if(getCurrentTokenType() == numbersym) // ELSE IF TOKEN = NUMBER
    {
        // Consume numbersym
        printCurrentToken(); // Printing the token is essential! // GET TOKEN
        nextToken(); // Go to the next token.
        return 0;
    }

    // Is that a lparentsym?
    else if(getCurrentTokenType() == lparentsym) // ELSE IF TOKEN = LPARENT
    {
        // Consume lparentsym
        printCurrentToken(); // Printing the token is essential! // GET TOKEN
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
        printCurrentToken(); // Printing the token is essential! // GET TOKEN
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
