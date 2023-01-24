/******************************************************************************
 *                                  IFJ22
 *                                 data.h
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: Declaration of grammar and syntax rules structures
 * 
 *                        Last change: 16. 11. 2022
 *****************************************************************************/

#ifndef DATA_IFJ
#define DATA_IFJ

#include "../bst/bst.h"

//Indexed of key words.W
typedef enum {
    KW_ELSE_IND,
    KW_FUNC_IND,
    KW_IF_IND,
    KW_RET_IND,
    KW_WHILE_IND,
    KW_DEC_IND,
    KW_ST_IND
} KeyWordsInd;

//Indexes if operators.
typedef enum {
    ADD_IND,
    SUB_IND,
    MUL_IND,
    DIV_IND,
    GT_IND,
    LT_IND,
    GE_IND,
    LE_IND,
    EQ_IND,
    NE_IND,
    CON_IND
} OperatorsInd;

//Indexes of types.
typedef enum {
    INT_IND,
    FLOAT_IND,
    STRING_IND,
    NULL_IND,
    VOID_IND,
    BOOL_IND
} TypesInd;

//Indexes of tokens, which can be used in rules.
typedef enum {
    TOK_END,
    TOK_START,
    TOK_OPER,
    TOK_OPER_MIN,
    TOK_ASSIG,
    TOK_FUN,
    TOK_TYPE,
    TOK_BRO,
    TOK_BRC,
    TOK_CBO,
    TOK_CBC,
    TOK_COL,
    TOK_SEMICOL,
    TOK_EOF,
    TOK_CONST,
    TOK_VAR,
    TOK_QUEST,
    TOK_ONE,
    TOK_RET,
    TOK_COMMA
} TokenInd;

//Types of rules joint.
typedef enum {
    KEY_WORDS_J,        //if, else, while...
    TYPE_J,             //int, float...
    TOK_J,              //Specific type of token
    RULE_J,             //Other rule
    CONST_J             //Constant val
} RuleJointType;

//Structure describing singla part of syntax rule.
typedef struct RuleJoint {
    RuleJointType type;
    union
    {
        KeyWordsInd kwIndex;            //Index of key word
        TypesInd typeIndex;             //Index of type
        struct
        {
            TokenInd tokenType;         //Index of required token
            long long tokenAtribute;    //Required atribute of token
        } TokenData;
        key_t ruleKey;                  //Key of rule
    } RuleJointData;
    struct RuleJoint * next;            //Pointer to next part of rule
} ruleJoint_t;

typedef struct Rule {
    int variantsCount;
    ruleJoint_t ** ruleVariants;
} rule_t;

//Type for pointer to token index in token tree.
typedef int * lexData_t;

//Structure including BSTs of grammar definitions.
typedef struct Grammar {
    bst_t * keyWords;
    bst_t * operators;
    bst_t * types;
    bst_t * syntaxRules;
} grammar_t;

extern grammar_t grammar;

//Generates grammar trees. Must be called before first use of grammar structure.
int grammarInit();
//Free grammar.
void freeGrammar();

#endif