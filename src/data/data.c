/******************************************************************************
 *                                  IFJ22
 *                                 data.c
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: Generation of grammar trees and syntax rules reading
 * 
 *                        Last change: 16. 11. 2022
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "data.h"
#include "../str/ifj_string.h"
#include "../bst/bst.h"
#include "../scanner/scanner.h"
#include "../compError.h"

grammar_t grammar;
bst_t * getKeyWordTree();
bst_t * getOperatorsTree();
bst_t * getTypesTree();
bst_t * getRuleTree();

static const char * KEY_WORDS[] = {
    "else",     "function",     "if",           "return",
    "while",    "declare",      "strict_types"
};

static const char * OPERATORS[] = {
    "+",    "-",    "*",    "/",
    ">",    "<",    ">=",   "<=",
    "===",  "!==",  "."
};

static const char * TYPES[] = {
    "int",      "float",
    "string",   "null",
    "void"
};

static const char * TOKENS[] = {
    "?>",       "START",        "oper",     "-",     
    "=",        "funID",        "type",     "(",
    ")",        "{",            "}",        ":",
    ";",        "EOF",          "const",    "varID",
    "?",        "1",            "null",     ","
};

int grammarInit() {
    if (    (grammar.keyWords = getKeyWordTree())      == NULL ||
            (grammar.operators = getOperatorsTree())   == NULL ||
            (grammar.types = getTypesTree())           == NULL ||
            (grammar.syntaxRules = getRuleTree())      == NULL
        )
        return 1;
    return 0;
}

bst_t * getKeyWordTree() {
    bst_t * tree;
    bstInit(&tree);
    key_t key;
    lexData_t data;

    for (int i = KW_ELSE_IND; i <= KW_ST_IND; i++) {
        key = get_key(KEY_WORDS[i]);
        if ((data = malloc(sizeof(int))) == NULL)
            return NULL;
        *data = i;
        bstInsert(&tree, key, (bstData_t)data);
    }

    return tree;
}

bst_t * getOperatorsTree() {
    bst_t * tree;
    bstInit(&tree);
    key_t key;
    lexData_t data;

    for (int i = ADD_IND; i <= CON_IND; i++) {
        key = get_key(OPERATORS[i]);
        if ((data = malloc(sizeof(int))) == NULL)
            return NULL;
        *data = i;
        if (bstInsert(&tree, key, (bstData_t)data))
            return NULL;
    }

    return tree;
}

bst_t * getTypesTree() {
    bst_t * tree;
    bstInit(&tree);
    key_t key;
    lexData_t data;

    for (int i = INT_IND; i <= VOID_IND; i++) {
        key = get_key(TYPES[i]);
        if ((data = malloc(sizeof(int))) == NULL)
            return NULL;
        *data = i;
        if (bstInsert(&tree, key, (bstData_t)data))
            return NULL;
    }

    return tree;
}

bst_t * getTokTree();
int readRuleStr(FILE * f, string_t * str);
bool ruleIsInTree(bst_t * ruleTree, string_t str);
int addRuleVariant(bst_t * ruleTree, string_t str);
int addRule(bst_t ** ruleTree, string_t str);
rule_t * getPointerToCurrentRule(bst_t *ruleTree, string_t str);
int readRule(FILE * f, rule_t * rule, bst_t * tokTree);

//Reads syntax rules from rules.txt 
bst_t * getRuleTree() {
    bst_t * ruleTree;
    bstInit(&ruleTree);

    bst_t * tokTree = getTokTree();

    FILE *f;
    if ((f = fopen("./src/data/rules.txt", "r")) == NULL)
        return NULL;
    string_t str;
    if (stringInit(&str))
        return NULL;

    while (true) {
        //Read name of rule
        if (readRuleStr(f, &str))
            return NULL;
        
        //If rule has already been described in rule tree, new variant of rule will be created
        if (ruleIsInTree(ruleTree, str)) {
            if (addRuleVariant(ruleTree, str))
                return NULL;
        }
        //Else, new rule will be created
        else if (addRule(&ruleTree, str))
            return NULL;

        //Rule reading...
        rule_t * currentRule = getPointerToCurrentRule(ruleTree, str);
        if (readRule(f, currentRule, tokTree))
            return NULL;

        //Rule end flag reading...
        if (readRuleStr(f, &str))
            return NULL;
        if (*stringRead(&str) == '#')
            break;
    }
    if (fclose(f) == EOF)
        return NULL;
    
    stringFree(&str);
    return ruleTree;
}

bst_t * getTokTree() {
    bst_t * tree;
    bstInit(&tree);
    key_t key;
    lexData_t data;

    for (int i = TOK_END; i <= TOK_COMMA; i++) {
        key = get_key(TOKENS[i]);
        if ((data = malloc(sizeof(int))) == NULL)
            return NULL;
        *data = i;
        if (bstInsert(&tree, key, (bstData_t)data))
            return NULL;
    }

    return tree;
}

//Reads single part of rule
int readRuleStr(FILE * f, string_t * str) {
    stringClear(str);
    int c;
    while (isspace(c = fgetc(f)));
    do {
        if (c == EOF) {
            if (stringLength(str) == 0)
                return 1;
            else
                return 0;
        }
        stringAppend(str, c);
    } while (!isspace(c = fgetc(f)));
    return 0;
}

//Checks, if rule has already been described it rule tree
bool ruleIsInTree(bst_t * ruleTree, string_t str) {
    key_t key = get_key(stringRead(&str));
    if (bstSearch(ruleTree, key))
        return true;
    return false;
}

//Adds new variant of rule
int addRuleVariant(bst_t * ruleTree, string_t str) {
    key_t key = get_key(stringRead(&str));
    rule_t * rule = (rule_t *)bstGet(ruleTree, key);
    rule->variantsCount++;
    if ((rule->ruleVariants = realloc(rule->ruleVariants, sizeof(ruleJoint_t *) * rule->variantsCount)) == NULL)
        return 1;
    return 0;
}

//Adds new rule in rule tree
int addRule(bst_t ** ruleTree, string_t str) {
    key_t key = get_key(stringRead(&str));
    rule_t * newRule = malloc(sizeof(rule_t));
    if (newRule == NULL)
        return 1;
    newRule->ruleVariants = malloc(sizeof(ruleJoint_t *));
    if (newRule->ruleVariants == NULL)
        return 1;
    newRule->variantsCount = 1;
    
    bstInsert(ruleTree, key, (bstData_t)newRule);
    return 0;
}

//Returnes pointer to rule which is currently in process
rule_t * getPointerToCurrentRule(bst_t *ruleTree, string_t str) {
    key_t key = get_key(stringRead(&str));
    rule_t * rule = (rule_t *)bstGet(ruleTree, key);
    return rule;
}

void tokenDefine(ruleJoint_t * ruleJoint, TokenInd ind);
//Reading sequence of rule parts and save them in rule structure.
int readRule(FILE * f, rule_t * rule, bst_t * tokTree) {
    string_t word;
    if (stringInit(&word))
        return 1;

    //-> skip
    if (readRuleStr(f, &word))
        return 1;
    
    ruleJoint_t ** jointLocation = &(rule->ruleVariants[rule->variantsCount - 1]);

    while (true) {
        if (readRuleStr(f, &word))
            return 1;
        if (*stringRead(&word) == '|')
            break;
        
        ruleJoint_t * newJoint = malloc(sizeof(ruleJoint_t));
        if (newJoint == NULL)
            return 1;
        newJoint->next = NULL;

        *jointLocation = newJoint;
        jointLocation = &(newJoint->next);

        key_t key = get_key(stringRead(&word));
        if (*stringRead(&word) == '<') {
            newJoint->type = RULE_J;
            newJoint->RuleJointData.ruleKey = key;
        }
        else if (bstSearch(grammar.keyWords, key)) {
            newJoint->type = KEY_WORDS_J;
            KeyWordsInd kwInd = *(KeyWordsInd *)bstGet(grammar.keyWords, key);
            newJoint->RuleJointData.kwIndex = kwInd;
        }
        else if (bstSearch(tokTree, key)) {
            newJoint->type = TOK_J;
            newJoint->RuleJointData.TokenData.tokenAtribute = -1;
            tokenDefine(newJoint, *(TokenInd *)bstGet(tokTree, key));

        }
        else
        {
            fprintf(stderr, "Undefined type of rule joint: %s\n", stringRead(&word));
            return 1;
        }
        
    }
    stringFree(&word);
    return 0;
}

//Sets type of token rule joint based on index from token tree
void tokenDefine(ruleJoint_t * ruleJoint, TokenInd ind) {
    switch (ind) {
        case TOK_END:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)PHP_END;
            break;
        case TOK_START:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)PHP_START;
            break;
        case TOK_OPER:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)OPER;
            break;
        case TOK_OPER_MIN:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)OPER;
            ruleJoint->RuleJointData.TokenData.tokenAtribute = SUB_IND;
            break;
        case TOK_ASSIG:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)ASSIG;
            break;
        case TOK_FUN:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)FUN;
            break;
        case TOK_TYPE:
            ruleJoint->type = TYPE_J;
            break;
        case TOK_BRO:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)BR_O;
            break;
        case TOK_BRC:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)BR_C;
            break;
        case TOK_CBO:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)CB_O;
            break;
        case TOK_CBC:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)CB_C;
            break;
        case TOK_COL:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)COLON;
            break;
        case TOK_SEMICOL:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)SEMICOLON;
            break;
        case TOK_EOF:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)END;
            break;
        case TOK_CONST:
            ruleJoint->type = CONST_J;
            break;
        case TOK_VAR:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)VAR;
            break;
        case TOK_QUEST:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)QUEST;
            break;
        case TOK_ONE:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)INT;
            ruleJoint->RuleJointData.TokenData.tokenAtribute = 1;
            break;
        case TOK_RET:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)TYPE;
            ruleJoint->RuleJointData.TokenData.tokenAtribute = (long long)NULL_IND;
            break;
        case TOK_COMMA:
            ruleJoint->RuleJointData.TokenData.tokenType = (TokenInd)COMMA;
            break;
    }
}

void freeGrammar() {
    bstDestroy(&(grammar.keyWords));
    bstDestroy(&(grammar.operators));
    bstDestroy(&(grammar.syntaxRules));
    bstDestroy(&(grammar.types));
}