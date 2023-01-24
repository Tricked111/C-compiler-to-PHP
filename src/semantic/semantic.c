/******************************************************************************
 *                                  IFJ22
 *                                symtable.c
 * 
 *      Authors: Alžbeta Hricovová (xhrico00)
 *      Purpose: Semantic control
 * 
 *                        Last change: 27. 11. 2022
 *****************************************************************************/
#include "semantic.h"


int varToken(program_t *program, int poss, Symtable *globalTable, token_t token, Symtable *localTable, bool local) {
    SymtableData *data = malloc(sizeof(SymtableData));
    if(data == NULL) {
        return 99;
    }
    uint32_t key = getKey(program->tokens[poss].textData.str);
    Symtable *table;
    if(local) {
        table = localTable; 
    } else {
        table = globalTable;
    }

    int err = 0;
    TypesInd expType;
    pfExpr_t exp;
    switch(program->tokens[poss].type) {            
        case FUN:
            err = funCallToken(program, poss+2, globalTable, program->tokens[poss], localTable, local);
            if(err != 0) {
                return err;
            } 
            TypesInd retype = symtableGet(globalTable, key)->dtype.func_type.retype;
            if(retype == VOID_IND) {    
                retype = NULL_IND;
            }
            add_var(data, retype);
            if(insertSymtable(table, getKey(token.textData.str), data)) {
                return 99;
            }
            break;

        default:
            exp = makeExpression(program, poss);
            err = evaluateExpression(table, &exp, &expType);
            if(err != 0) {
                return err;
            }

            if(expType == BOOL_IND) {
                return 8;
            }

            add_var(data, expType);
            if(insertSymtable(table, getKey(token.textData.str), data)) {
                    return 99;
            }
            break;

    }
    return 0;

}

// volanie funkcie, nie definicia
int funCallToken(program_t *program, int poss, Symtable *globalTable, token_t token, Symtable *localTable, bool local) {     //pridat ci hladat v loaklnej a potom v globalnej tabulke param
    Symtable *table;
    if(local) {
        table = localTable; 
    } else {
        table = globalTable;
    }
    if(!symtableSearch(globalTable, getKey(token.textData.str))) {
        return 3;
    }
    SymtableData *data = symtableGet(globalTable, getKey(token.textData.str));
    TypesInd *funParam = return_param_func(*data);
    int *funQuest = return_quest_func(*data); 
    size_t count = 0;

    if(strcmp(token.textData.str, "write") == 0) {
        while(program->tokens[poss].type != BR_C) {
            if(program->tokens[poss].type == VAR) {
                if(!symtableSearch(table, getKey(program->tokens[poss].textData.str))) {
                    return 5;
                }
            }
            poss++;
        }
        return 0;
    }

    if(strcmp(token.textData.str, "intval") == 0 || strcmp(token.textData.str, "floatval") == 0 || strcmp(token.textData.str, "strval") == 0) {
        while(program->tokens[poss].type != BR_C) {
            if(program->tokens[poss].type == VAR) {
                if(!symtableSearch(table, getKey(program->tokens[poss].textData.str))) {
                    return 5;
                }
            } else if(program->tokens[poss].type == COMMA) {
                count--;
            }
            count++;
            poss++;
        }
        if(count != 1) {
            return 4;
        }
        return 0;
    }
    
    while(program->tokens[poss].type != BR_C) {      

        if(program->tokens[poss].type == VAR) {
            uint32_t key = getKey(program->tokens[poss].textData.str);
            SymtableData *paramVar;
            if(!symtableSearch(table, key)) {
                return 5;
            } else {
                paramVar = symtableGet(table, key);
            }
            
            if(paramVar->dtype.var_type != funParam[count]) {       
                if(paramVar->dtype.var_type == NULL_IND && funQuest[count] == 1) {
                } else {
                    return 4;
                }
            }
        } else if(program->tokens[poss].type == COMMA) {
            count--;
        } else {
            if(typeIndChoice(program->tokens[poss]) != funParam[count]) {
                if(typeIndChoice(program->tokens[poss]) == NULL_IND && funQuest[count] == 1) {
                } else {
                    return 4;
                }
            }                   
        }

        count++;
        poss++;
    }

    if(count != data->dtype.func_type.lenght) {    
        return 4;
    }   

    return 0;
}


/*//TEMPORARY!!!!!!
void printAllFuncParams(Symtable funcTable) {
    if (funcTable == NULL)
        return;
    printAllFuncParams(funcTable->right);
    printAllFuncParams(funcTable->left);
    printf("FUN %lld: \t", (long long)funcTable->key);
    for (int i = 0; i < (int)funcTable->data.dtype.func_type.lenght; i++)
        printf("%d, ", (int)funcTable->data.dtype.func_type.func_params[i]);
    printf("\n\tReturn type: %d\n", (int)funcTable->data.dtype.func_type.retype);
}*/

int semanticControl(program_t *program) {
    bool local = false;
    bool quest = false;
    int bracket = 0;
    pfExpr_t expression;
    TypesInd expType;

    Symtable funcTable;
    int getFunTableRet = getFunTable(program, &funcTable);
    if (getFunTableRet)
        return getFunTableRet;

    Symtable localTable = NULL;
    int err;
    bool wasReturn = false;
    uint32_t funKey;
    SemStates state = S_START;
    SymtableData *data;
    token_t tok;
 
    for (int i = 0; i < program->tokenCount; i++) {    
        tok = program->tokens[i];
        switch (state) {
            case S_START: 
                if (tok.type == END || tok.type == PHP_END) {
                    state = S_END;
                } else if (tok.type == KW && tok.numericData.ivalue == (long long)KW_FUNC_IND) {    
                    localTable = NULL;
                    state = S_GET_PARAM;
                    funKey = getKey(program->tokens[i+1].textData.str);
                } else if (tok.type == VAR) {
                    state = S_VAR_ROW;
                } else if (tok.type == FUN) {
                    state = S_FUN_CALL;
                } else if (tok.type == KW && tok.numericData.ivalue == (long long)KW_WHILE_IND) {
                    state = S_WHILE_IF;
                } else if (tok.type == KW && tok.numericData.ivalue == (long long)KW_IF_IND) {
                    state = S_WHILE_IF;
                } else if (tok.type == KW && tok.numericData.ivalue == (long long)KW_ELSE_IND) {
                    state = S_ELSE;
                } else if(tok.type == KW && tok.numericData.ivalue == (long long)KW_RET_IND) {
                    if(local) {
                        wasReturn = true;
                    }
                    state = S_RET;
                } else if(tok.type == CB_C) { 
                    if(bracket == 0) {          //je to zatvorka funkcie
                        if(!wasReturn) {        
                            if(symtableGet(&funcTable, funKey)->dtype.func_type.retype != VOID_IND) {
                                return 4;
                            }
                        }   

                        local = false;
                        quest = false;
                        wasReturn = false;

                    } else {    
                        bracket--;  
                    }   
                } else if(tok.type == INT || tok.type == STRING || tok.type == FLOAT || (tok.type == TYPE && strcmp(tok.textData.str, "null") == 0)) {
                    expression = makeExpression(program, i);   
                    if(local) {
                        err = evaluateExpression(&localTable, &expression, &expType);
                    } else {
                        err = evaluateExpression(&funcTable, &expression, &expType);
                    }
                        
                    if(err != 0) {
                        return err;
                    }
                    state = S_ROW_END;
                }   
                
                continue;

            case S_RET:
                if(local) {
                    if(symtableGet(&funcTable, funKey)->dtype.func_type.retype == VOID_IND && tok.type != SEMICOLON) {
                        return 6;
                    }
                }
                
                if(tok.type == SEMICOLON) {
                    if(local && (symtableGet(&funcTable, funKey)->dtype.func_type.retype != VOID_IND)) {
                        return 6;
                    }
                    state = S_START;
                /**} else if(tok.type == FUN) {
                    err = funCallToken(program, i+2, &funcTable, tok, &localTable, local);
                    if(err != 0) {
                        return err;
                    }  
                    if(local && ((symtableGet(&funcTable, funKey)->dtype.func_type.retype) != symtableGet(&funcTable, getKey(tok.textData.str))->dtype.func_type.retype)) {
                        return 4;
                    }
                    state = S_ROW_END;  **/
                } else {
                    expression = makeExpression(program, i);    //int, float, string/ vyraz, zavolat erika, vyhodi TypesInd
                    if(local) {
                        err = evaluateExpression(&localTable, &expression, &expType);
                    } else {
                        err = evaluateExpression(&funcTable, &expression, &expType);
                    }
                        
                    if(err != 0) {
                        return err;
                    }
                    
                    if(local && (symtableGet(&funcTable, funKey)->dtype.func_type.retype != expType)) {
                        if(quest) {
                            if(expType != NULL_IND) {
                                return 4;
                            }
                
                        } else {
                            return 4;
                        }
                    }
                    state = S_ROW_END;
                }
                
                continue;              

            case S_WHILE_IF: 
                expression = makeExpression(program, i+1); 
                if(local) {
                    err = evaluateExpression(&localTable, &expression, &expType);
                } else {
                    err = evaluateExpression(&funcTable, &expression, &expType);
                }
                    
                if(err != 0) {
                    return err;
                }
                state = S_CB_O;
                continue;

            case S_ELSE:
                bracket++;
                state = S_START;
                continue;

            case S_FUN_CALL:    
                err = funCallToken(program, i+1, &funcTable, program->tokens[i-1], &localTable, local);
                if(err != 0) {
                    return err;
                } 
                state = S_ROW_END;
                continue;   

            case S_VAR_ROW:
                if(tok.type == ASSIG) {     //priradenie $a = ...
                    err = varToken(program, i+1, &funcTable, program->tokens[i-1], &localTable, local);
                    if(err != 0) {
                        return err;
                    }                    
                } else {      // $a + $b; bez priradenia 
                    
                    expression = makeExpression(program, i-1);
                    if(local) {
                        err = evaluateExpression(&localTable, &expression, &expType);
                    } else {
                        err = evaluateExpression(&funcTable, &expression, &expType);
                    }
                    
                    if(err != 0) {
                        return err;
                    }
                }
                state = S_ROW_END;
                continue;
        
            case S_GET_PARAM:
                if (tok.type == TYPE) {
                    if ((data = malloc(sizeof(SymtableData))) == NULL) {
                        return 99;
                    }
                    add_var(data, (TypesInd) tok.numericData.ivalue);
                }
                else if (tok.type == VAR) {
                    if(insertSymtable(&localTable, getKey(tok.textData.str), data)) {
                        return 99;
                    }
                } else if (tok.type == BR_C) {
                    state = S_FUN_RET;
                }
                continue;

            case S_FUN_RET:
                if(tok.type == QUEST) {
                    quest = true;
                } else if(tok.type == CB_O) {
                    local = true;
                    state = S_START;
                } 
                continue;
        
            case S_ROW_END:
                if(tok.type == SEMICOLON) {
                    state = S_START;
                }
                continue;

            case S_CB_O:
                if(tok.type == CB_O) {
                    bracket++;
                    state = S_START;
                }
                continue;

            case S_END:
                break;
            
            default:
                break;
        }
    }
    return 0;
}

int getFunTable(program_t * program, Symtable * funcTable) {
    Symtable newFuncTable = NULL;   
    built_function(&newFuncTable);
    FunDefsStates state = SF_START;

    uint32_t key; 
    SymtableData * newFuncData;
    bst_t * params;
    bstInit(&params);
    string_t * name;
    key_t paramKey;

    for (int i = 0; i < program->tokenCount; i++) {
        switch (state) {
            case SF_START:
                if (program->tokens[i].type == END || program->tokens[i].type == PHP_END)
                    state = SF_END;
                else if (program->tokens[i].type == KW && program->tokens[i].numericData.ivalue == (long long)KW_FUNC_IND)
                    state = SF_FUN_DEC;
                continue;
            case SF_FUN_DEC:
                key = getKey(stringRead(&(program->tokens[i].textData)));
                if (symtableSearch(&newFuncTable, key))
                    return 3;
                if ((newFuncData = malloc(sizeof(SymtableData))) == NULL)
                    return 99;
                add_func(newFuncData);
                state = SF_SKIP_BR;
                continue;
            case SF_SKIP_BR:
                if(program->tokens[i].type == BR_O) {
                    continue;
                } else if(program->tokens[i].type == QUEST) {
                    state = SF_GET_PARAM;
                    continue;
                } else {
                    if (program->tokens[i].type == TYPE) {
                        add_func_param(newFuncData, (TypesInd)program->tokens[i].numericData.ivalue, 0);
                        state = SF_SKIP_NAME;
                        continue;
                    }
                    else if (program->tokens[i].type == BR_C)
                        state = SF_GET_TYPE;
                    continue;
                }
            case SF_GET_PARAM:
                if (program->tokens[i].type == TYPE) {
                    add_func_param(newFuncData, (TypesInd)program->tokens[i].numericData.ivalue, 1);
                    state = SF_SKIP_NAME;
                }
                else if (program->tokens[i].type == BR_C)
                    state = SF_GET_TYPE;
                continue;
            case SF_SKIP_NAME:
                name = &(program->tokens[i].textData);
                paramKey = get_key(stringRead(name));
                if (bstSearch(params, paramKey))
                    return 8;
                bstInsert(&params, paramKey, (bstData_t)name);
                state = SF_CHECK_NEXT;
                continue;
            case SF_CHECK_NEXT:
                if (program->tokens[i].type == COMMA)
                    state = SF_SKIP_BR;
                else if (program->tokens[i].type == BR_C)
                    state = SF_GET_TYPE;
                continue;
            case SF_GET_TYPE:
                if (program->tokens[i].type == TYPE) {
                    add_retype(newFuncData, (TypesInd)program->tokens[i].numericData.ivalue);
                    state = SF_FUN_START;
                }
                continue;
            case SF_FUN_START:
                insertSymtable(&newFuncTable, key, newFuncData);
                bstDestroy(&params);
                state = SF_START;
                continue;
            case SF_END:
                break;
            default:
                state++;
        }
    }   
    *funcTable = newFuncTable;
    return 0;
}