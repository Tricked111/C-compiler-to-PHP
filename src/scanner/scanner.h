/******************************************************************************
 *                                  IFJ22
 *                                scanner.h
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: Declaration of scanner and token structures and operations on them
 * 
 *                        Last change: 16. 11. 2022
 *****************************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

#include "../str/ifj_string.h"
#include "../data/data.h"
#include <ctype.h>
#include <stdbool.h>

//States of scanner FSM.
typedef enum {
    Start,
    Num,
    FloatInter1,
    FloatInter2,
    FloatInter3,
    Float1,
    Float2,
    Question,
    PhpEnd,
    Less,
    LQ,
    P,
    PH,
    PhpStart,
    More,
    Slash,
    Comment,
    BlockCom,
    BCInter,
    Oper,
    OperInter1,
    OperInter2,
    ID,
    Assig,
    CBracket,
    Bracket,
    Comma,
    Semicolon,
    Colon,
    End,
    String,
    EsqSeq,
    StringEnd,
    VarStart,
    Var,
    Error
} ScannerStates;

//Types of tokens.
typedef enum {
    FLOAT,      
    INT,        
    PHP_END,    
    PHP_START,  
    OPER,
    ASSIG,      
    IDEN,           KW,     FUN,    TYPE,
    CBRACK,         CB_O,   CB_C,
    BRACK,          BR_O,   BR_C,
    COMMA,      
    SEMICOLON,  
    COLON,      
    END,        
    STRING,     
    VAR,        
    QUEST,
    ERROR,
    tok_init   //Default value of new inicialized token, does not denote any real type of token.
} TokenType;

//Types of action on processed symbol.
typedef enum {
    WRITE,  //Symbol will be added to the end of token string
    SKIP,   //Symbol will be skipped
    NEXT,   //Symbol is a part of next token, so it will be processed again
    CLEAN   //All read symbols are not parts token, so the token string will be cleaned, symbol will be skipped
} ScannerActions;

//Lexical unit of program.
typedef struct Token {
    TokenType type;
    union TokenData {
        long long ivalue;
        float fvalue; 
    } numericData;
    string_t textData;
} token_t;

typedef struct Scanner {
    int symbol;                 //Processed symbol.
    ScannerActions action;      //Denote an action on symbol.
    ScannerStates state;        //Actual state of scanner.
    bool endOfToken;            //Indicates whether we have reached the end of token.
} scanner_t;

//Structure including all program tokens.
typedef struct Program {
    int tokenCount;
    token_t * tokens;
} program_t;

//Reads program and load it's tokens into global program structure
int readPogram(program_t * program);

#endif