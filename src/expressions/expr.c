/******************************************************************************
 *                                  IFJ22
 *                                 expr.c
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
                 Erik Hrubý        (xhruby30)   
 *      Purpose: Definitions of functions used for expressions transformation to postfix notation.
 * 
 *                        Last change: 29. 11. 2022
 *****************************************************************************/

#include "expr.h"
#include "../data/data.h"

void stackInit(stack_t * stack) {
    stack->data = malloc(0);
    stack->size = 0;
}
void stackPuch(stack_t * stack, token_t tok) {
    stack->size++;
    stack->data = realloc(stack->data, sizeof(token_t) * stack->size);
    stack->data[stack->size - 1] = tok;
}
token_t stackPop(stack_t * stack) {
    token_t tok = stack->data[stack->size - 1];
    stack->size--;
    stack->data = realloc(stack->data, sizeof(token_t) * stack->size);
    return tok;
}
token_t stackTop(stack_t * stack) {
    token_t tok = stack->data[stack->size - 1];
    return tok;
}
bool stackEmpty(stack_t * stack) {
    if (stack->size == 0)
        return true;
    return false;
}
void stackFree(stack_t * stack) {
    free(stack->data);
}

void exprAdd(pfExpr_t * expr, token_t tok);
bool lowerOper(token_t oper1, token_t oper2);

pfExpr_t makeExpression(program_t * prog, int startIndex) {
    pfExpr_t expr;
    expr.expr = malloc(0);
    expr.size = 0;
    stack_t stack;
    stackInit(&stack);
    int brokes = 0;

    while (1) {
        TokenType tt = prog->tokens[startIndex].type;
        if (tt == INT || tt == FLOAT || tt == STRING || tt == VAR || (tt == TYPE && prog->tokens[startIndex].numericData.ivalue == (long long)NULL_IND))
            exprAdd(&expr, prog->tokens[startIndex]);
        else if (tt == BR_O) {
            brokes++;
            stackPuch(&stack, prog->tokens[startIndex]);
        }
        else if (tt == OPER) {
            if (stackEmpty(&stack) || stackTop(&stack).type == BR_O || (stackTop(&stack).type == OPER && lowerOper(stackTop(&stack), prog->tokens[startIndex])))
                stackPuch(&stack, prog->tokens[startIndex]);
            else {
                exprAdd(&expr, stackPop(&stack));
                continue;
            }
        }
        else if (tt == BR_C && brokes) {
            token_t tok;
            brokes--;
            while ((tok = stackPop(&stack)).type != BR_O)
                exprAdd(&expr, tok);
        }
        else {
            while (!stackEmpty(&stack))
                exprAdd(&expr, stackPop(&stack));
            break;
        }

        startIndex++;
    }

    stackFree(&stack);
    return expr;
}

void exprAdd(pfExpr_t * expr, token_t tok) {
    expr->size++;
    expr->expr = realloc(expr->expr, sizeof(token_t) * expr->size);
    expr->expr[expr->size - 1] = tok;
}

int findOperInd(OperatorsInd * operSet, OperatorsInd oper) {
    int index = 0;
    while (1) {
        if (operSet[index] == oper)
            return index;
        index++;
    }
}
bool lowerOper(token_t oper1, token_t oper2) {
    OperatorsInd oper1Ind = (OperatorsInd)oper1.numericData.ivalue;
    OperatorsInd oper2Ind = (OperatorsInd)oper2.numericData.ivalue;
    OperatorsInd sortedOpers[] = {  MUL_IND,    DIV_IND,    ADD_IND,    SUB_IND,
                                    CON_IND,    LT_IND,     GT_IND,     LE_IND,
                                    GE_IND,     EQ_IND,     NE_IND  };
    if (findOperInd(sortedOpers, oper1Ind) > findOperInd(sortedOpers, oper2Ind))
        return true;
    return false;   
}

int evaluateExpression(Symtable * table, pfExpr_t * expr, TypesInd * type) {
    typeStack_t stack;
    typeStackInit(&stack); 
    // printf("Eval start-->size %d: \n", expr->size);   

    for (int i = 0; i < expr->size; i++) {
        token_t token =  expr->expr[i]; 

        if (token.type == OPER) { 
            // printf("OPERATOR:\n");
            TypesInd operand2 = typeStackPop(&stack);
            TypesInd operand1 = typeStackPop(&stack);

            long long operator = token.numericData.ivalue;            
            if (operator == ADD_IND || operator == SUB_IND || operator == MUL_IND) {

                if (operand1 == STRING_IND || operand2 == STRING_IND) {
                    return 7;
                } else if (operand1 == FLOAT_IND || operand2 == FLOAT_IND) {
                    typeStackPush(&stack, FLOAT_IND);
                } else {
                    typeStackPush(&stack, INT_IND);
                }

            } else if (operator == DIV_IND) {

                if (operand1 == STRING_IND || operand2 == STRING_IND) {
                    return 7;
                } else {
                    typeStackPush(&stack, FLOAT_IND);
                }

            } else if(operator == CON_IND) {

                if (operand1 == INT_IND || operand2 == INT_IND || operand1 == FLOAT_IND || operand2 == FLOAT_IND) {
                    return 7;
                } else {
                    typeStackPush(&stack, STRING_IND);
                }
            } else {
                typeStackPush(&stack, BOOL_IND);
            }                        
        } else {
            
            if (token.type == VAR) {                
                if(symtableSearch(table, getKey(token.textData.str))) {
                    typeStackPush(&stack, (symtableGet(table, getKey(token.textData.str)))->dtype.var_type);
                } else {                    
                    return 5;
                }
            } else {
                typeStackPush(&stack, typeIndChoice(token));               
            }                                    
        }
    }

    *type = typeStackPop(&stack);
    typeStackFree(&stack);
    return 0;
}


void typeStackInit(typeStack_t * stack) {
    stack->data = malloc(0);
    stack->size = 0;
}
void typeStackPush(typeStack_t * stack, TypesInd type) {
    stack->size++;
    stack->data = realloc(stack->data, sizeof(TypesInd) * stack->size);    
    stack->data[stack->size - 1] = type;
    // printf("pushed %i\n", type); 
}

TypesInd typeStackPop(typeStack_t * stack) {
    TypesInd type = stack->data[stack->size - 1];
    stack->size--;
    stack->data = realloc(stack->data, sizeof(TypesInd) * stack->size);
    return type;
}

void typeStackFree(typeStack_t * stack) {
    free(stack->data);
}

TypesInd typeIndChoice(token_t token) {
    switch(token.type) {
        case INT:
            return INT_IND;
        case FLOAT:
            return FLOAT_IND;
        case STRING:
            return STRING_IND;        
        default:
            return NULL_IND; 
    }
}

uint32_t getKey(const char *str) { //Source: https://en.wikipedia.org/wiki/Jenkins_hash_function   
    uint32_t hash = 0;
    size_t i = 0;
    while (str[i]) {
        hash += str[i];
        hash += hash << 10;
        hash ^= hash >> 6;
        i++;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
} 
