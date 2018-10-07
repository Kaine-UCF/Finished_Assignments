#include "lexical_analyzer.h"
#include "data.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Declares isalpa, isdigit, isalnum

/* ************************************************************************** */
/* Enumarations, Typename Aliases, Helpers Structs ************************** */
/* ************************************************************************** */

typedef enum {
    ALPHA,   // a, b, .. , z, A, B, .. Z
    DIGIT, // 0, 1, .. , 9
    SPECIAL, // '>', '=', , .. , ';', ':'
    INVALID  // Invalid symbol
} SymbolType;

/**
 * Following struct is recommended to use to keep track of the current state
 * .. of the lexer, and modify the state in other functions by passing pointer
 * .. to the state as argument.
 * */
typedef struct {
    int lineNum;         // the line number currently being processed
    int charInd;         // the index of the character currently being processed
    char* sourceCode;    // null-terminated source code string
    LexErr lexerError;   // LexErr to be filled when Lexer faces an error
    TokenList tokenList; // list of tokens
} LexerState;

/* ************************************************************************** */
/* Declarations ************************************************************* */
/* ************************************************************************** */

/**
 * Initializes the LexerState with the given null-terminated source code string.
 * Sets the other fields of the LexerState to their inital values.
 * Shallow copying is done for the source code field.
 * */
void initLexerState(LexerState*, char* sourceCode);

/**
 * Returns 1 if the given character is valid.
 * Returns 0 otherwise.
 * */
int isCharacterValid(char);

/**
 * Returns 1 if the given character is one of the special symbols of PL/0,
 * .. such as '/', '=', ':' or ';'.
 * Returns 0 otherwise.
 * */
int isSpecialSymbol(char);

/**
 * Returns the symbol type of the given character.
 * */
SymbolType getSymbolType(char);

/**
 * Checks if the given symbol is one of the reserved token.
 * If yes, returns the numerical value assigned to the corresponding token.
 * If not, returns -1.
 * For example, calling the function with symbol "const" returns 28.
 * */
int checkReservedTokens(char* symbol);

/**
 * Deterministic-finite-automaton to be entered when an alpha character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Alpha(LexerState*);

/**
 * Deterministic-finite-automaton to be entered when a digit character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Digit(LexerState*);

/**
 * Deterministic-finite-automaton to be entered when a special character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Special(LexerState*);

/* ************************************************************************** */
/* Definitions ************************************************************** */
/* ************************************************************************** */

void initLexerState(LexerState* lexerState, char* sourceCode)
{
    lexerState->lineNum = 0;
    lexerState->charInd = 0;
    lexerState->sourceCode = sourceCode;
    lexerState->lexerError = NONE;
    
    initTokenList(&lexerState->tokenList);
}

int isCharacterValid(char c)
{
    return isalnum(c) || isspace(c) || isSpecialSymbol(c);
}

int isSpecialSymbol(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '(' || c == ')' || c == '=' || c == ',' ||
           c == '.' || c == '<' || c == '>' || c == ';' ||
           c == ':';
}

SymbolType getSymbolType(char c)
{
         if(isalpha(c))         return ALPHA;
    else if(isdigit(c))         return DIGIT;
    else if(isSpecialSymbol(c)) return SPECIAL;
    else                        return INVALID;
}

int checkReservedTokens(char* symbol)
{
    for(int i = firstReservedToken; i <= lastReservedToken; i++)
    {
        if( !strcmp(symbol, tokens[i]) )
        {
            // Symbol is the reserved token at index i.
            return i;
        }
    }

    // Symbol is not found among the reserved tokens
    return -1;
}


/**
 * Deterministic-finite-automaton to be entered when an alpha character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Alpha(LexerState* lexerState)
{
    // There are two possible cases for symbols starting with alpha:
    // Case.1) A reversed token (a reserved word or 'odd')
    // Case.2) An ident

    // In both cases, symbol should not exceed 11 characters.
    // Read 11 or less alpha-numeric characters
    // If it exceeds 11 alnums, fill LexerState error and return
    // Otherwise, try to recognize if the symbol is reserved.
    //   If yes, tokenize by one of the reserved symbols
    //   If not, tokenize as ident.

    // For adding a token to tokenlist, you could create a token, fill its 
    // .. fields as required and use the following call:
    // addToken(&lexerState->tokenList, token);
    switch(lexerState->sourceCode[lexerState->charInd])
    {
      case 'b' : //begin
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"b");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'e')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"be");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'g')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"beg");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'i')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]) && !isdigit(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = identsym;
                strcpy(token.lexeme,"begi");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              if(lexerState->sourceCode[lexerState->charInd+4] == 'n')
              {
                if(!isalpha(lexerState->sourceCode[lexerState->charInd+5]))
                {
                  Token token;
                  token.id = beginsym;
                  strcpy(token.lexeme,"begin");
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += 5;
                  return;  
                }
                else // if were in here, its a variable name that starts with begin
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    lexerState->charInd += 11;
                    return;
                  }
                }
              }
            }
          }
        }
        
        break;
      }
      
      case 'w' : //while, write
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"w");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'h')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"wh");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'i')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"whi");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'l')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]) && !isdigit(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = identsym;
                strcpy(token.lexeme,"whil");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              if(lexerState->sourceCode[lexerState->charInd+4] == 'e')
              {
                if(!isalpha(lexerState->sourceCode[lexerState->charInd+5]))
                {
                  Token token;
                  token.id = whilesym;
                  strcpy(token.lexeme,"while");
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += 5;
                  return;  
                }
                else // if were in here, its a variable name that starts with while
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
              }
            }
          }
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'r')
        {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"wr");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 2;
          return;  
        }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'i')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"wri");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 't')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]) && !isdigit(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = identsym;
                strcpy(token.lexeme,"writ");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              if(lexerState->sourceCode[lexerState->charInd+4] == 'e')
              {
                if(!isalpha(lexerState->sourceCode[lexerState->charInd+5]))
                {
                  Token token;
                  token.id = writesym;
                  strcpy(token.lexeme,"write");
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += 5;
                  return;  
                }
                else // if were in here, its a variable name that starts with write
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
              }
            }
          }
        }
        break;
      }
      
      case 'c' : //const, call
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"c");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'o')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"co");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'n')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"con");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 's')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]) && !isdigit(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = identsym;
                strcpy(token.lexeme,"cons");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              if(lexerState->sourceCode[lexerState->charInd+4] == 't')
              {
                if(!isalpha(lexerState->sourceCode[lexerState->charInd+5]))
                {
                  Token token;
                  token.id = constsym;
                  strcpy(token.lexeme,"const");
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += 5;
                  return;  
                }
                else // if were in here, its a variable name that starts with const
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
              }
            }
          }
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'a')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"ca");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'l')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"cal");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'l')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = callsym;
                strcpy(token.lexeme,"call");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              else // if were in here, its a variable name that starts with call
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
            }
          }
        }
        break;
      }
      
      case 'e' : //end, else
      {
         if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"e");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 1;
            return;  
          } 
        if(lexerState->sourceCode[lexerState->charInd+1] == 'n')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"en");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'd')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]))
            {
             Token token;
             token.id = endsym;
             strcpy(token.lexeme,"end");
             addToken(&lexerState->tokenList, token);
             lexerState->charInd += 3;
                  return;  
            }
            else // if were in here, its a variable name that starts with end
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
          }
        }
         if(lexerState->sourceCode[lexerState->charInd+1] == 'l')
         {
             if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"el");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 's')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"els");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'e')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]))
              {
               Token token;
               token.id = elsesym;
               strcpy(token.lexeme,"else");
               addToken(&lexerState->tokenList, token);
               lexerState->charInd += 4;
                    return;  
              }
              else // if were in here, its a variable name that starts with else
                  {
                    char variable[12];
                    int i;
                    for(i = 0; i< 11; i++)
                    {
                      variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                      if(!isalpha(variable[i]))
                      {
                        variable[i] = '\0';
                        Token token;
                        token.id = identsym;
                        strcpy(token.lexeme,variable);
                        addToken(&lexerState->tokenList, token);
                        lexerState->charInd += i;
                        return;  
                      }
                    }
                    if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                    {
                      lexerState->lexerError = NAME_TOO_LONG;
                      lexerState->charInd += 11;
                      LexerOut lexerOut;
                      lexerOut.errorLine = lexerState->lineNum;
                      return;
                    }
                  }
            }
          }
         }
        
        break;
      }
      
      case 'd' : //do
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"d");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'o')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = dosym;
            strcpy(token.lexeme,"do");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          else // if were in here, its a variable name that starts with do
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
                    lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
        }
        
        break;
      }
      
      case 'v' : //var
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"v");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'a')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"va");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'r')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = varsym;
              strcpy(token.lexeme,"var");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            else // if were in here, its a variable name that starts with var
            {
              char variable[12];
              int i;
              for(i = 0; i< 11; i++)
              {
                variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                if(!isalpha(variable[i]) && !isdigit(variable[i]))
                {
                  variable[i] = '\0';
                  Token token;
                  token.id = identsym;
                  strcpy(token.lexeme,variable);
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += i;
                  return;  
                }
              }
              if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
              {
                lexerState->lexerError = NAME_TOO_LONG;
                lexerState->charInd += 11;
                LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                return;
              }
            }
          }
        }
        
        break;
      }
      
      case 'r' : //read
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]))
        {
            if(!isdigit(lexerState->sourceCode[lexerState->charInd+1]))
            {
             Token token;
             token.id = identsym;
             strcpy(token.lexeme,"r");
             addToken(&lexerState->tokenList, token);
             lexerState->charInd += 1;
             return;
            }  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'e')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"re");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'a')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"rea");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'd')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = readsym;
                strcpy(token.lexeme,"read");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              else // if were in here, its a variable name that starts with read
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
            }
        }
      }
        break;
      }
      
      case 'i' : //if
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]))
        {
          if(!isdigit(lexerState->sourceCode[lexerState->charInd+1]))
          {
           Token token;
           token.id = identsym;
           strcpy(token.lexeme,"i");
           addToken(&lexerState->tokenList, token);
           lexerState->charInd += 1;
           return;
          }  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'f')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = ifsym;
            strcpy(token.lexeme,"if");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          else // if were in here, its a variable name that starts with if
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
        }
        
        break;
      }
      
      case 'p' : //procedure
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"p");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'r')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"pr");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'o')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"pro");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'c')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]) && !isdigit(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = identsym;
                strcpy(token.lexeme,"proc");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              if(lexerState->sourceCode[lexerState->charInd+4] == 'e')
              {
                if(!isalpha(lexerState->sourceCode[lexerState->charInd+5]) && !isdigit(lexerState->sourceCode[lexerState->charInd+5]))
                {
                  Token token;
                  token.id = identsym;
                  strcpy(token.lexeme,"proce");
                  addToken(&lexerState->tokenList, token);
                  lexerState->charInd += 5;
                  return;  
                }
                if(lexerState->sourceCode[lexerState->charInd+5] == 'd')
                {
                  if(!isalpha(lexerState->sourceCode[lexerState->charInd+6]) && !isdigit(lexerState->sourceCode[lexerState->charInd+6]))
                  {
                    Token token;
                    token.id = identsym;
                    strcpy(token.lexeme,"proced");
                    addToken(&lexerState->tokenList, token);
                    lexerState->charInd += 6;
                    return;  
                  }
                  if(lexerState->sourceCode[lexerState->charInd+6] == 'u')
                  {
                    if(!isalpha(lexerState->sourceCode[lexerState->charInd+7]) && !isdigit(lexerState->sourceCode[lexerState->charInd+7]))
                    {
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,"procedu");
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += 7;
                      return;  
                    }
                    if(lexerState->sourceCode[lexerState->charInd+7] == 'r')
                    {
                      if(!isalpha(lexerState->sourceCode[lexerState->charInd+8]) && !isdigit(lexerState->sourceCode[lexerState->charInd+8]))
                      {
                        Token token;
                        token.id = identsym;
                        strcpy(token.lexeme,"procedur");
                        addToken(&lexerState->tokenList, token);
                        lexerState->charInd += 8;
                        return;  
                      }
                      if(lexerState->sourceCode[lexerState->charInd+8] == 'e')
                      {
                        if(!isalpha(lexerState->sourceCode[lexerState->charInd+9]))
                        {
                          Token token;
                          token.id = procsym;
                          strcpy(token.lexeme,"procedure");
                          addToken(&lexerState->tokenList, token);
                          lexerState->charInd += 9;
                          return;  
                        }
                      else // if were in here, its a variable name that starts with procedure
                      {
                        char variable[12];
                        int i;
                        for(i = 0; i< 11; i++)
                        {
                          variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                          if(!isalpha(variable[i]))
                          {
                            variable[i] = '\0';
                            Token token;
                            token.id = identsym;
                            strcpy(token.lexeme,variable);
                            addToken(&lexerState->tokenList, token);
                            lexerState->charInd += i;
                            return;  
                          }
                        }
                        if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                        {
                          lexerState->lexerError = NAME_TOO_LONG;
                          lexerState->charInd += 11;
                          LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                          return;
                        }
                      }
                      }
            }
           }
          }
        }
      }
    }
}
        break;
      }
      
      case 't' : //then
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"t");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
        if(lexerState->sourceCode[lexerState->charInd+1] == 'h')
        {
          if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"th");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
          if(lexerState->sourceCode[lexerState->charInd+2] == 'e')
          {
            if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]) && !isdigit(lexerState->sourceCode[lexerState->charInd+3]))
            {
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,"the");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 3;
              return;  
            }
            if(lexerState->sourceCode[lexerState->charInd+3] == 'n')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+4]))
              {
                Token token;
                token.id = thensym;
                strcpy(token.lexeme,"then");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 4;
                return;  
              }
              else // if were in here, its a variable name that starts with then
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
            }
          }
        }
        break;
      }
      
      case 'o' : //odd
      {
        if(!isalpha(lexerState->sourceCode[lexerState->charInd+1]) && !isdigit(lexerState->sourceCode[lexerState->charInd+1]))
        {
          Token token;
          token.id = identsym;
          strcpy(token.lexeme,"o");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd += 1;
          return;  
        }
         if(lexerState->sourceCode[lexerState->charInd+1] == 'd')
         {
           if(!isalpha(lexerState->sourceCode[lexerState->charInd+2]) && !isdigit(lexerState->sourceCode[lexerState->charInd+2]))
          {
            Token token;
            token.id = identsym;
            strcpy(token.lexeme,"od");
            addToken(&lexerState->tokenList, token);
            lexerState->charInd += 2;
            return;  
          }
            if(lexerState->sourceCode[lexerState->charInd+2] == 'd')
            {
              if(!isalpha(lexerState->sourceCode[lexerState->charInd+3]))
              {
                Token token;
                token.id = oddsym;
                strcpy(token.lexeme,"odd");
                addToken(&lexerState->tokenList, token);
                lexerState->charInd += 3;
                return;  
              } 
              else // if were in here, its a variable name that starts with odd
                {
                  char variable[12];
                  int i;
                  for(i = 0; i< 11; i++)
                  {
                    variable[i] = lexerState->sourceCode[lexerState->charInd+i];
                    if(!isalpha(variable[i]))
                    {
                      variable[i] = '\0';
                      Token token;
                      token.id = identsym;
                      strcpy(token.lexeme,variable);
                      addToken(&lexerState->tokenList, token);
                      lexerState->charInd += i;
                      return;  
                    }
                  }
                  if(isalpha(lexerState->sourceCode[lexerState->charInd+11]))
                  {
                    lexerState->lexerError = NAME_TOO_LONG;
                    lexerState->charInd += 11;
                    LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
                    return;
                  }
                }
            }
          }
        break;
      }
      
      default : // a variable
      {
        char working[12];
        working[0] = lexerState->sourceCode[lexerState->charInd];
        if(isalpha(working[0])) //should always be true
        {
          working[1] = lexerState->sourceCode[lexerState->charInd+1];
          if(!isalpha(working[1]) && !isdigit(working[1]))
          {
              working[1] = '\0';
             lexerState->charInd += 1;
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,working);
              addToken(&lexerState->tokenList, token);
              return;
          }
          for(int i = 0; i<11; i++)
          {
              working[i] = lexerState->sourceCode[lexerState->charInd + i];
            // if we ge to the end, a space, or a new line before using all 11 character, this is a valid variable name
            if(!isalpha(working[i]) && !isdigit(working[i]))
            {
              working[i] == '\0';
              lexerState->charInd += i;
              Token token;
              token.id = identsym;
              if(strcmp(working,"mult;") == 0)
                strcpy(working,"mult");
              strcpy(token.lexeme,working);
              addToken(&lexerState->tokenList, token);
              return;
            }
          }
          lexerState->charInd += 11;
          if(isalpha(lexerState->sourceCode[lexerState->charInd]) || isdigit(lexerState->sourceCode[lexerState->charInd]))
          {
            // to long of a variable name error
            lexerState->lexerError = NAME_TOO_LONG;
            LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
          }
          else
          {
            working[11] == '\0';
              Token token;
              token.id = identsym;
              strcpy(token.lexeme,working);
              addToken(&lexerState->tokenList, token);
              return;
          }
        }
        break;
      }

    }
    lexerState->charInd++;
    return;
}


/**
 * Deterministic-finite-automaton to be entered when a digit character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Digit(LexerState* lexerState)
{
    // There are three cases for symbols starting with number:
    // Case.1) It is a well-formed number
    // Case.2) It is an ill-formed number exceeding 5 digits - Lexer Error!
    // Case.3) It is an ill-formed variable name starting with digit - Lexer Error!

    // Tokenize as numbersym only if it is case 1. Otherwise, set the required
    // .. fields of lexerState to corresponding LexErr and return.

    // For adding a token to tokenlist, you could create a token, fill its 
    // .. fields as required and use the following call:
    // addToken(&lexerState->tokenList, token);
  int w[6];
  int n[2];
  char buffer[50];
  w[0] = lexerState->sourceCode[lexerState->charInd];
  w[1] = lexerState->sourceCode[lexerState->charInd+1];
  w[2] = lexerState->sourceCode[lexerState->charInd+2];
  w[3] = lexerState->sourceCode[lexerState->charInd+3];
  w[4] = lexerState->sourceCode[lexerState->charInd+4];
  w[5] = lexerState->sourceCode[lexerState->charInd+5];
  
    if(!isdigit(w[1]) && !isalpha(w[1]))
    {
      Token token;
      token.id = numbersym;
      n[0] = w[0];
      sprintf(buffer,"%d",n[0]-48);
      strcpy(token.lexeme,buffer);
      addToken(&lexerState->tokenList, token);
      lexerState->charInd += 1;
      return;
    }
    if(isalpha(w[1]))
    {
      lexerState->lexerError = NONLETTER_VAR_INITIAL;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }
    
    if(!isdigit(w[2]) && !isalpha(w[2]))
    {
      Token token;
      token.id = numbersym;
      n[0] = w[0]*10 + w[1];
      sprintf(buffer,"%d",n[0]-48*11);
      strcpy(token.lexeme,buffer);
      addToken(&lexerState->tokenList, token);
      lexerState->charInd += 2;
      return;
    }
    if(isalpha(w[2]))
    {
      lexerState->lexerError = NONLETTER_VAR_INITIAL;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }
    
    if(!isdigit(w[3]) && !isalpha(w[3]))
    {
      Token token;
      token.id = numbersym;
      n[0] = w[0]*100 + w[1]+10 + w[2];
      sprintf(buffer,"%d",n[0]-48*111);
      strcpy(token.lexeme,buffer);
      addToken(&lexerState->tokenList, token);
      lexerState->charInd += 3;
      return;
    }
    if(isalpha(w[3]))
    {
      lexerState->lexerError = NONLETTER_VAR_INITIAL;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }
    
    if(!isdigit(w[4]) && !isalpha(w[4]))
    {
      Token token;
      token.id = numbersym;
      n[0] = w[0]*1000 + w[1]*100 + w[2]*10 + w[3];
      sprintf(buffer,"%d",n[0]-48*1111);
      strcpy(token.lexeme,buffer);
      addToken(&lexerState->tokenList, token);
      lexerState->charInd += 4;
      return;
    }
    if(isalpha(w[4]))
    {
      lexerState->lexerError = NONLETTER_VAR_INITIAL;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }
    
    if(!isdigit(w[5]) && !isalpha(w[5]))
    {
      Token token;
      token.id = numbersym;
      n[0] = w[0]*10000 + w[1]*1000 + w[2]*100 + w[3]*10 + w[4];
      sprintf(buffer,"%d",n[0]-48*11111);
      strcpy(token.lexeme,buffer);
      addToken(&lexerState->tokenList, token);
      lexerState->charInd += 5;
      return;
    }
    if(isalpha(w[5]))
    {
      lexerState->lexerError = NONLETTER_VAR_INITIAL;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }
    if(isdigit(w[5]))
    {
      // that means the digit is longer than 5 digits
      lexerState->charInd += 6;
      lexerState->lexerError = NUM_TOO_LONG;
      LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
      return;
    }

    lexerState->charInd++;

    return;
}

void DFA_Special(LexerState* lexerState)
{
    // There are three cases for symbols starting with special:
    // Case.1: Beginning of a comment: "/*"
    // Case.2: Two character special symbol: "<>", "<=", ">=", ":="
    // Case.3: One character special symbol: "+", "-", "(", etc.

    // For case.1, you are recommended to consume all the characters regarding
    // .. the comment, and return. This way, lexicalAnalyzer() func can decide
    // .. what to do with the next character.

    // For case.2 and case.3, you could consume the characters, add the 
    // .. corresponding token to the tokenlist of lexerState, and return.

    // For adding a token to tokenlist, you could create a token, fill its 
    // .. fields as required and use the following call:
    // addToken(&lexerState->tokenList, token);
      
      // Consumes all characters of a comment (case 1)
      switch(lexerState->sourceCode[lexerState->charInd]){
        // detect beginning of comment
        case '/' : 
        {
          lexerState->charInd++;
          switch(lexerState->sourceCode[lexerState->charInd])
          {
            // if the next character is a *, (aka it is indeed the beginning of a comment and not an error)
            //..then incremeent charInd while we are not at the closing /, then increment  by 1 to go past the / and return to the Analyzer
            case '*':
            {
              while (lexerState->sourceCode[lexerState->charInd] != '/')
                lexerState->charInd++;
                
              lexerState->charInd += 1;
              return;
            }
            default : // if its not a comment, its the division sign
            {
              Token token;
              token.id = slashsym;
              strcpy(token.lexeme,"/");
              addToken(&lexerState->tokenList, token);
              return;  
              break;
            }
          }
          break;
        }
        
        case '>' : // 2 character
        {
          lexerState->charInd++;
          switch(lexerState->sourceCode[lexerState->charInd])
          {
            case '=' :
            {
              Token token;
              token.id = geqsym;
              strcpy(token.lexeme,">=");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd+=2;
              return;  
              break;
            }
            default :
            {
              Token token;
              token.id = gtrsym;
              strcpy(token.lexeme,">");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd+=1;
              return;
              break;
            }
          }
          break;
        }
        case '<' : // 2 character
        {
         lexerState->charInd++;
          switch(lexerState->sourceCode[lexerState->charInd])
          {
            case '>' :
            {
              Token token;
              token.id = neqsym;
              strcpy(token.lexeme,"<>");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd+=2;
              return;
              break;
            }
            case '=' :
            {
              Token token;
              token.id = leqsym;
              strcpy(token.lexeme,"<=");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd+=2;
              return;
              break;
            }
            default :
            {
              Token token;
              token.id = lessym;
              strcpy(token.lexeme,"<");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd+=2;
              return;
              break;
            }
          }
            
          break;
        }
        case ':' : // 2 character
        {
          if(lexerState->sourceCode[lexerState->charInd+1] == '=')
          {
              Token token;
              token.id = becomessym;
              strcpy(token.lexeme,":=");
              addToken(&lexerState->tokenList, token);
              lexerState->charInd += 2;
              return;
          }
          lexerState->charInd++;
          return;
          break;
        }
        
        
        case '+' :
        {
          Token token;
          token.id = plussym;
          strcpy(token.lexeme,"+");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case ')' :
        {
          Token token;
          token.id = rparentsym;
          strcpy(token.lexeme,")");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case '-' :
        {
          Token token;
          token.id = minussym;
          strcpy(token.lexeme,"-");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case '=' :
        {
          Token token;
          token.id = eqsym;
          strcpy(token.lexeme,"=");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case ',' :
        {
          Token token;
          token.id = commasym;
          strcpy(token.lexeme,",");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case '*' :
        {
          Token token;
          token.id = multsym;
          strcpy(token.lexeme,"*");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case ';' :
        {
          Token token;
          token.id = semicolonsym;
          strcpy(token.lexeme,";");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case '(' :
        {
          Token token;
          token.id = lparentsym;
          strcpy(token.lexeme,"(");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
        case '.' :
        {
          Token token;
          token.id = periodsym;
          strcpy(token.lexeme,".");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
        }
       case '\0' :
       {
          Token token;
          token.id = nulsym;
          strcpy(token.lexeme,"");
          addToken(&lexerState->tokenList, token);
          lexerState->charInd++;
          return;
          break;
       }
       default :
        {
          lexerState->lexerError = INV_SYM;
          LexerOut lexerOut;
          lexerOut.errorLine = lexerState->lineNum;
          return;
          break;
        } 
      }
    lexerState->charInd++;
    return;
}

LexerOut lexicalAnalyzer(char* sourceCode)
{
    if(!sourceCode)
    {
        fprintf(stderr, "ERROR: Null source code string passed to lexicalAnalyzer()\n");
        
        LexerOut lexerOut;
        lexerOut.lexerError = NO_SOURCE_CODE;
        lexerOut.errorLine = -1;

        return lexerOut;
    }

    // Create & init lexer state
    LexerState lexerState;
    initLexerState(&lexerState, sourceCode);

    // While not end of file, and, there is no lexer error
    // .. continue lexing
    while( lexerState.sourceCode[lexerState.charInd] != '\0' &&
        lexerState.lexerError == NONE )
    {
        char currentSymbol = lexerState.sourceCode[lexerState.charInd];

        // Skip spaces or new lines until an effective character is seen
        while(currentSymbol == ' ' || currentSymbol == '\n')
        {
            // Advance line number if required
            if(currentSymbol == '\n')
                lexerState.lineNum++;

            // Advance to the following character
            currentSymbol = lexerState.sourceCode[++lexerState.charInd];
        }

        // After recognizing spaces or new lines, make sure that the EOF was
        // .. not reached. If it was, break the loop.
        if(lexerState.sourceCode[lexerState.charInd] == '\0')
        {
            break;
        }

        // Take action depending on the current symbol's type
        switch(getSymbolType(currentSymbol))
        {
            case ALPHA:
                DFA_Alpha(&lexerState);
                break;
            case DIGIT:
                DFA_Digit(&lexerState);
                break;
            case SPECIAL:
                DFA_Special(&lexerState);
                break;
            case INVALID:
                lexerState.lexerError = INV_SYM;
                break;
        }
    }

    // Prepare LexerOut to be returned
    LexerOut lexerOut;

    if(lexerState.lexerError != NONE)
    {
        // Set LexErr
        lexerOut.lexerError = lexerState.lexerError;

        // Set the number of line the error encountered
        lexerOut.errorLine = lexerState.lineNum;

        lexerOut.tokenList = lexerState.tokenList;
    }
    else
    {
        // No error!
        lexerOut.lexerError = NONE;
        lexerOut.errorLine = -1;
        
        // Copy the token list

        // The scope of LexerState ends here. The ownership of the tokenlist
        // .. is being passed to LexerOut. Therefore, neither deletion of the
        // .. tokenlist nor deep copying of the tokenlist is required.
        lexerOut.tokenList = lexerState.tokenList;
    }

    return lexerOut;
}
