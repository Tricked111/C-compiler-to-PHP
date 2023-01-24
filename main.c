/******************************************************************************
 *                                  IFJ22
 *                                  main.c
 * 
 *      Authors: Kniazkin Daniil   (xkniaz00)
                 Erik Hrubý        (xhruby30)
                 Alžbeta Hricovová (xhrico00)
                 Nikita Kotvitskiy (xkotvi01)
 *                    Purpose: Main function of compiler
 * 
 *                        Last change: 2. 10. 2022
 *****************************************************************************/

#include <stdio.h>
#include "src/scanner/scanner.h"
#include "src/parser/parser.h"
#include "src/bst/bst.h"
#include "src/data/data.h"
#include "src/symtable/symtable.h"
#include "src/semantic/semantic.h"
#include "src/generator/generator.h"


void printProgramTokens(program_t * program) {
    for (int i = 0; i < program->tokenCount; i++) {
        token_t tok = program->tokens[i];
        switch (tok.type) {
            case FLOAT:
                printf("[float: %f]\n", tok.numericData.fvalue);
                break;
            case INT:
                printf("[int: %lld]\n", tok.numericData.ivalue);
                break;
            case PHP_END:
                printf("[php_end: ]\n");
                break;
            case PHP_START:
                printf("[php_start: ]\n");
                break;
            case OPER:
                printf("[oper: %lld]\n", tok.numericData.ivalue);
                break;
            case ASSIG:
                printf("[assig: ]\n");
                break;
            case VAR:
                printf("[var: %s]\n", stringRead(&(tok.textData)));
                break;
            case STRING:
                printf("[string: %s]\n", stringRead(&(tok.textData)));
                break;
            case END:
                printf("[end: ]\n");
                break;
            case COLON:
                printf("[colon: ]\n");
                break;
            case SEMICOLON:
                printf("[semicolon: ]\n");
                break;
            case COMMA:
                printf("[comma: ]\n");
                break;
            case BR_O:
                printf("[open_br: ]\n");
                break;
            case BR_C:
                printf("[close_br: ]\n");
                break;
            case CB_O:
                printf("[open_cb: ]\n");
                break;
            case CB_C:
                printf("[close_cb: ]\n");
                break;
            case IDEN:
                printf("[id: %s]\n", stringRead(&(tok.textData)));
                break;
            case KW:
                printf("[key_word: %lld]\n", tok.numericData.ivalue);
                break;
            case TYPE:
                printf("[type: %lld]\n", tok.numericData.ivalue);
                break;
            case FUN:
                printf("[funID: %s]\n", stringRead(&(tok.textData)));
                break;
            case QUEST:
                printf("[question: ]\n");
                break;
            default:
                printf("error\n");
                break;
        }
    }
}

int main()
{
    if (grammarInit())
        return INTERN_ERR;
    program_t program;
    int retValue = readPogram(&program);
    if (retValue) {        
        //printf("%d\n", retValue);
        return retValue;
    }

    //printProgramTokens(&program);
    retValue = parseProgram(&program);
    if (retValue) {        
        printf("%d\n", retValue);
        return retValue;
    }    

    retValue = semanticControl(&program);
    if (retValue) {        
        printf("%d\n", retValue);
        return retValue;
    }

    retValue = generateProgram(program);
    if (retValue) {
        printf("%d\n", retValue);
        return retValue;
    }
    
    return 0;
}