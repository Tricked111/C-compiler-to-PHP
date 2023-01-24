#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "../data/data.h"

bool ruleProcess(rule_t * rule, int * cursor, const program_t * program);
bool checkRuleVariant(ruleJoint_t * rule, int * cursor, const program_t * program);
bool checkRulePart(ruleJoint_t * rule, int * cursor, const program_t * program);

#define DEBUGGING false //TEMPORARY


int parseProgram(const program_t * program) {
    rule_t * firstRule = (rule_t *)grammar.syntaxRules->data;
    int cursor = 0;
    if (!ruleProcess(firstRule, &cursor, program))
        return SYNTAX_ERR;
    return 0;
}

bool ruleProcess(rule_t * rule, int * cursor, const program_t * program) {
    if (DEBUGGING) printf("=============\nStart rule processing...\n");
    if (DEBUGGING) printf("Rule has %d variants\n", rule->variantsCount);


    for (int i = 0; i < rule->variantsCount; i++) {
        if (DEBUGGING) printf("Trying %d variant...\n  ", i);
        if (checkRuleVariant(rule->ruleVariants[i], cursor, program))
            return true;
    }
    if (DEBUGGING) printf("All variants are incorrect\n");
    return false;
}

bool checkRuleVariant(ruleJoint_t * rule, int * cursor, const program_t * program) {
    int localCursor = * cursor;
    do {
        if (!checkRulePart(rule, &localCursor, program))
            return false;
        localCursor++;
        rule = rule->next;
    } while (rule != NULL);
    *cursor = localCursor - 1;
    if (DEBUGGING) printf("Rule is processed seccessfully\n");
    return true;
}

bool checkRulePart(ruleJoint_t * rule, int * cursor, const program_t * program) {
    if (DEBUGGING) printf("\tRule part processing...\n\tType of part index is %d\n", rule->type);
    if (*cursor >= program->tokenCount)
        return false;
    switch (rule->type) {
        case KEY_WORDS_J:
            if (DEBUGGING) printf("\tKey word with %d index is required\n", rule->RuleJointData.kwIndex);
            if (DEBUGGING) printf("\tToken type is %d\n\tToken data is %lld\n", program->tokens[*cursor].type, program->tokens[*cursor].numericData.ivalue);
            if (program->tokens[*cursor].type == KW && program->tokens[*cursor].numericData.ivalue == rule->RuleJointData.kwIndex)
                return true;
            return false;
        case TYPE_J:
            if (DEBUGGING) printf("\tToken of type is required\n");
            if (DEBUGGING) printf("\tToken type is %d\n", program->tokens[*cursor].type);
            if (program->tokens[*cursor].type == TYPE) {
                if (rule->RuleJointData.TokenData.tokenAtribute != -1 && program->tokens[*cursor].numericData.ivalue != rule->RuleJointData.TokenData.tokenAtribute)
                    return false;
                return true;
            }
            return false;
        case TOK_J:
            if (DEBUGGING) printf("\tToken of special type %d is required\n\tToken type is %d\n", rule->RuleJointData.TokenData.tokenType, program->tokens[*cursor].type);
            if (program->tokens[*cursor].type == (TokenType)rule->RuleJointData.TokenData.tokenType) {
                if (rule->RuleJointData.TokenData.tokenAtribute != -1 && program->tokens[*cursor].numericData.ivalue != rule->RuleJointData.TokenData.tokenAtribute)
                    return false;
                return true;
            }
            return false;
        case RULE_J:
            if (DEBUGGING) printf("\tRule is required...\n");
            rule_t * innerRule = bstGet(grammar.syntaxRules, rule->RuleJointData.ruleKey);
            return ruleProcess(innerRule, cursor, program);
        case CONST_J:
            if (DEBUGGING) printf("\tSome constant value is required\n");
            if (program->tokens[*cursor].type == INT || program->tokens[*cursor].type == FLOAT || program->tokens[*cursor].type == STRING)
                return true;
            return false;
    }
    return false;
}