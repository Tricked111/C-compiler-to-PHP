/******************************************************************************
 *                                  IFJ22
 *                                 expr.h
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
                 Erik Hrubý        (xhruby30)
 *      Purpose: Declaration of structure and functions for expressions transformation to postfix notation.
 * 
 *                        Last change: 29. 11. 2022
 *****************************************************************************/

#ifndef EXPR_H
#define EXPR_H

#include "../scanner/scanner.h"
#include <stdlib.h>
#include <stdbool.h>
#include "../symtable/symtable.h"

typedef token_t * expr_t;

typedef struct PostfixExpression {
    int size;
    expr_t expr;
} pfExpr_t;

typedef void * stackData_t;

typedef struct Stack {
    int size;
    token_t * data; 
} stack_t;

typedef struct TypeStack {
    int size;
    TypesInd * data; 
} typeStack_t;

pfExpr_t makeExpression(program_t * prog, int startIndex);
void stackInit(stack_t * stack);
void stackPuch(stack_t * stack, token_t tok);
token_t stackPop(stack_t * stack);
token_t stackTop(stack_t * stack);
bool stackEmpty(stack_t * stack);
void stackFree(stack_t * stack);

int evaluateExpression(Symtable * table, pfExpr_t * expr, TypesInd * type);
void typeStackInit(typeStack_t * stack);
void typeStackPush(typeStack_t * stack, TypesInd type);
TypesInd typeStackPop(typeStack_t * stack);
void typeStackFree(typeStack_t * stack);

TypesInd typeIndChoice(token_t token);
uint32_t getKey(const char *str);
#endif