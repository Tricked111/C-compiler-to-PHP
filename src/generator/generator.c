/******************************************************************************
 *                                  IFJ22
 *                              generator.c
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: IFJcode22 code generation
 * 
 *                        Last change: 7. 12. 2022
 *****************************************************************************/

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include "../compError.h"
#include "../data/data.h"
#include "../expressions/expr.h"

void varAssignment(generator_t * gen, program_t prog);
void expression(generator_t * gen, program_t prog);
void functionDef(generator_t * gen, program_t prog);
void whileDef(generator_t * gen, program_t prog);
void ifDef(generator_t * gen, program_t prog);\
void funCall(generator_t * gen, program_t prog);
void detectConstruction(generator_t * gen, program_t prog);
void constructionSkip(generator_t * gen, program_t prog);
void comandSkip(generator_t * gen, program_t prog);
void defAllVarsInConstr(generator_t * gen, program_t prog);


int generateProgram(program_t prog) {
    generator_t gen;
    gen.counter = 0;
    gen.constrCnt = 0;
    gen.global = true;
    bstInit(&(gen.globalVars));
    bstInit(&(gen.localVars));
    bstInit(&(gen.functions));
    token_t tok;

    printf("%s\n", IFJ_PROG_START);
    printf("%s %s\n%s %s\n%s %s\n%s %s\n", DEFVAR, TMP1, DEFVAR, TMP2, DEFVAR, TMP3, DEFVAR, ARG_CNT);
    printf("%s __main\n", JUMP);
    
    printf(WRITE_FUN);
    printf(READS_FUN);
    printf(READI_FUN);
    printf(READF_FUN);
    printf(SUBSTRING_FUN);
    printf(CHR_FUN);
    printf(ORD_FUN);
    printf(INTVAL_FUN);
    printf(FLOATVAL_FUN);
    printf(STRVAL_FUN);

    gen.currentPosition = PROG_START;
    while (gen.currentPosition < prog.tokenCount) {
        tok = prog.tokens[gen.currentPosition];
        if (tok.type == KW && tok.numericData.ivalue == KW_FUNC_IND)
            functionDef(&gen, prog);
        gen.currentPosition++;
    }
    
    printf("%s __main\n%s\n%s\n", LABEL, CREATEFRAME, PUSHFRAME);
    gen.currentPosition = PROG_START;
    while (gen.currentPosition < prog.tokenCount) {
        detectConstruction(&gen, prog);
        gen.currentPosition++;
    }
    
    printf("POPFRAME");
    return 0;
}

void detectConstruction(generator_t * gen, program_t prog) {
    token_t tok = prog.tokens[gen->currentPosition];
    switch (tok.type) {
        case VAR:
            if (prog.tokens[gen->currentPosition + 1].type == ASSIG)
                varAssignment(gen, prog);
            else
                expression(gen, prog);
            break;
        case KW:
            switch ((KeyWordsInd)tok.numericData.ivalue) {
                case KW_FUNC_IND:
                    constructionSkip(gen, prog);
                    break;
                case KW_WHILE_IND:
                    whileDef(gen, prog);
                    break;
                case KW_IF_IND:
                    ifDef(gen, prog);
                    break;
                case KW_RET_IND:
                    if (gen->global == false) {
                        if (prog.tokens[gen->currentPosition + 1].type != SEMICOLON) {
                            gen->currentPosition++;
                            expression(gen, prog);
                        }
                        printf("POPFRAME\n%s\n", RETURN);
                    }
                    else {
                        printf("%s\n", EXIT);
                        comandSkip(gen, prog);
                    }
                default:
                    break;
            }
            break;
        case INT:
            expression(gen, prog);
            break;
        case FLOAT:
            expression(gen, prog);
            break;
        case STRING:
            expression(gen, prog);
            break;
        case FUN:
            expression(gen, prog);
            break;
        case TYPE:
            if (tok.numericData.ivalue == (long long)NULL_IND)
                expression(gen, prog);
            break;
        default:
            break;
    }
}

void constructionSkip(generator_t * gen, program_t prog) {
    int cbOpen = 0;
    token_t tok;
    while (true) {
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == CB_O) {
            cbOpen++;
        }
        else if (tok.type == CB_C) {
            cbOpen--;
            if (cbOpen == 0)
                break;
        }
        gen->currentPosition++;
    }
}

void comandSkip(generator_t * gen, program_t prog) {
    token_t tok;
    while (true) {
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == SEMICOLON)
            break;
        gen->currentPosition++;
    }
}

void varAssignment(generator_t * gen, program_t prog) {
    string_t * varName = &(prog.tokens[gen->currentPosition].textData);
    key_t key = get_key(stringRead(varName));
    bst_t ** table = gen->global ? &(gen->globalVars) : &(gen->localVars);
    if (!bstSearch(*table, key)) {
        printf("%s %s%s\n", DEFVAR, LOCAL_VAR, stringRead(varName));
        bstInsert(table, key, NULL);
    }
    gen->currentPosition += 2;
    expression(gen, prog);
    printf("%s %s%s\n", POPS, LOCAL_VAR, stringRead(varName));
}

void ifjStackPush(token_t tok);
void ifjOperate(token_t tok, generator_t * gen);
void expression(generator_t * gen, program_t prog) {
    if (prog.tokens[gen->currentPosition].type == FUN) {
        funCall(gen, prog);
        return;
    }
    pfExpr_t expr = makeExpression(&prog, gen->currentPosition);
    for (int i = 0; i < expr.size; i++) {
        if (expr.expr[i].type == OPER)
            ifjOperate(expr.expr[i], gen);
        else
            ifjStackPush(expr.expr[i]);
        gen->currentPosition++;
    }
}

void ifjStackPush(token_t tok) {
    printf("%s ", PUSHS);
    switch (tok.type) {
        case VAR:
            printf("%s%s\n", LOCAL_VAR, stringRead(&tok.textData));
            break;
        case INT:
            printf("%s%lld\n", IFJ_INTEGER, tok.numericData.ivalue);
            break;
        case FLOAT:
            printf("%s%a\n", IFJ_FLOAT, tok.numericData.fvalue);
            break;
        case STRING:
            string_t ifjString = convertToIFJCode(&tok.textData);
            printf("%s%s\n", IFJ_STRING, stringRead(&ifjString));
            break;
        case TYPE:
            if (tok.numericData.ivalue == (long long)NULL_IND)
                printf("nil@nil\n");
            break;
        default:
            break;
    }
}

void ifjOperate(token_t tok, generator_t * gen) {
    OperatorsInd oper = (OperatorsInd)tok.numericData.ivalue;
    if (oper == ADD_IND || oper == SUB_IND || oper == MUL_IND) {
        printf(NUMERIC_CONVERTER, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter);
        gen->counter++;
        switch (oper) {
            case ADD_IND:
                printf("%s\n", ADDS);
                break;
            case SUB_IND:
                printf("%s\n", SUBS);
                break;
            case MUL_IND:
                printf("%s\n", MULS);
                break;
            default:
                break;
        }
    }
    else if (oper == DIV_IND) {
        printf(FLOAT_CONVERTER, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter);
        gen->counter++;
        printf("%s\n", DIVS);
    }
    else if (oper == CON_IND) {
        printf(CONCATENATION, gen->counter, gen->counter, gen->counter, gen->counter);
        gen->counter++;
    }
    else if (oper == EQ_IND || oper == NE_IND) {
        printf(EQVIVAL, gen->counter, gen->counter, gen->counter, gen->counter);
        gen->counter++;
        if (oper == NE_IND)
            printf("%s\n", NOTS);
    }
    else {
        printf(NUMERIC_CONVERTER, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter);
        gen->counter++;
        switch (oper) {
            case GT_IND:
                printf("GTS\n");
                break;
            case LT_IND:
                printf("LTS\n");
                break;
            case GE_IND:
                printf("GTS\n");
                printf("PUSHS GF@__TMP1\nPUSHS GF@__TMP2\n");
                printf(EQVIVAL, gen->counter, gen->counter, gen->counter, gen->counter);
                gen->counter++;
                printf("ORS\n");
                break;
            case LE_IND:
                printf("LTS\n");
                printf("PUSHS GF@__TMP1\nPUSHS GF@__TMP2\n");
                printf(EQVIVAL, gen->counter, gen->counter, gen->counter, gen->counter);
                gen->counter++;
                printf("ORS\n");
                break;
            default:
                break;
        }
    }
}   

void functionDef(generator_t * gen, program_t prog) {
    gen->currentPosition++;
    printf("%s %s\n", LABEL, stringRead(&(prog.tokens[gen->currentPosition].textData)));
    gen->global = false;
    token_t tok;

    while(true) {
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == VAR) {
            printf("%s %s%s\n%s %s%s\n", DEFVAR, TMP_VAR, tok.textData.str, POPS, TMP_VAR, tok.textData.str);
            key_t key = get_key(tok.textData.str);
            bstInsert(&(gen->localVars), key, NULL);
        }
        if (tok.type == BR_C)
            break;
    }

    while(true) {
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == CB_O)
            break;
    }

    printf("%s\n", PUSHFRAME);
    while (true) {
        detectConstruction(gen, prog);
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == CB_C)
            break;
    }
    printf(FUNEND);
    gen->global = true;
    bstDestroy(&(gen->localVars));
}

void whileDef(generator_t * gen, program_t prog) {
    if (gen->constrCnt == 0)
        defAllVarsInConstr(gen, prog);
    int constrInd = gen->counter;
    gen->counter++;
    gen->currentPosition += 2;
    printf("LABEL __WHILE_%d_START\n", constrInd);
    expression(gen, prog);
    while (true)
    {
        gen->currentPosition++;
        if (prog.tokens[gen->currentPosition].type == CB_O)
            break;
    }
    gen->constrCnt++;

    printf(LOGIC_CHECK, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter);
    gen->counter++;
    printf("%s bool@false\nJUMPIFEQS __WHILE_%d_END\n", PUSHS, constrInd);
    token_t tok = prog.tokens[gen->currentPosition];
    while(true) {
        detectConstruction(gen, prog);
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == CB_C)
            break;
    }
    printf("JUMP __WHILE_%d_START\nLABEL __WHILE_%d_END\n", constrInd, constrInd);
    gen->constrCnt--;
}

void ifDef(generator_t * gen, program_t prog) {
    if (gen->constrCnt == 0)
        defAllVarsInConstr(gen, prog);
    int constrInd = gen->counter;
    gen->counter++;
    gen->currentPosition += 2;
    expression(gen, prog);
    while (true)
    {
        gen->currentPosition++;
        if (prog.tokens[gen->currentPosition].type == CB_O)
            break;
    }
    gen->constrCnt++;
    
    printf(LOGIC_CHECK, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter, gen->counter);
    gen->counter++;
    printf("%s bool@false\nJUMPIFEQS __IF_%d_ELSE\n", PUSHS, constrInd);\
    token_t tok = prog.tokens[gen->currentPosition];
    while(true) {
        detectConstruction(gen, prog);
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == KW && tok.numericData.ivalue == (long long)KW_ELSE_IND)
            break;
    }
    printf("JUMP __IF_%d_END\n", constrInd);
    printf("%s __IF_%d_ELSE\n", LABEL, constrInd);
    while(true) {
        detectConstruction(gen, prog);
        gen->currentPosition++;
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == CB_C)
            break;
    }
    printf("LABEL __IF_%d_END\n", constrInd);
    gen->constrCnt--;
}

void funCall(generator_t * gen, program_t prog) {
    string_t funName = prog.tokens[gen->currentPosition].textData;
    printf("%s\n", CREATEFRAME);
    token_t tok;
    gen->currentPosition++;
    int argCnt = 0;
    while (true) {
        if (prog.tokens[gen->currentPosition].type == BR_C)
            break;
        gen->currentPosition++;
    }
    while (true) {
        tok = prog.tokens[gen->currentPosition];
        if (tok.type == INT || tok.type == FLOAT || tok.type == STRING || tok.type == VAR || (tok.type == KW && tok.numericData.ivalue == (long long)NULL_IND)) {
            ifjStackPush(tok);
            argCnt++;
        }
        if (tok.type == BR_O)
            break;
        gen->currentPosition--;
    }
    printf("MOVE GF@__ARG_CNT int@%d\n", argCnt);
    printf("%s %s\n", CALL, funName.str);
}

void defAllVarsInConstr(generator_t * gen, program_t prog) {
    gen->subPosition = gen->currentPosition;
    while(true) {
        if (prog.tokens[gen->subPosition].type == CB_O)
            break;
        gen->subPosition++;
    }
    gen->subPosition++;
    gen->constrCnt++;
    token_t tok;
    bst_t ** table = gen->global ? &(gen->globalVars) : &(gen->localVars);
    while (true) {
        tok = prog.tokens[gen->subPosition];
        if (tok.type == VAR && prog.tokens[gen->subPosition + 1].type == ASSIG) {
            key_t key = get_key(tok.textData.str);
            if (!bstSearch(*table, key)) {
                printf("DEFVAR LF@%s\n", tok.textData.str);
                bstInsert(table, key, NULL);
            }
        }
        if (tok.type == CB_O)
        {
            gen->constrCnt++;
        }
        if (tok.type == CB_C)
        {
            gen->constrCnt--;
            if (gen->constrCnt == 0)
                break;
        }
        gen->subPosition++;
    }
}