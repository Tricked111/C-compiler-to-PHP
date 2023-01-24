/******************************************************************************
 *                                  IFJ22
 *                              generator.h
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: IFJcode22 code generation header
 * 
 *                        Last change: 7. 12. 2022
 *****************************************************************************/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "../scanner/scanner.h"
#include "../semantic/semantic.h"
#include "../symtable/symtable.h"
#include "../bst/bst.h"
#include <stdbool.h>

#define PROG_START 8
#define OUTPUT_FILE_NAME "out.ifjcode"

#define IFJ_PROG_START      ".IFJcode22"
#define TMP1                "GF@__TMP1"
#define TMP2                "GF@__TMP2"
#define TMP3                "GF@__TMP3"
#define ARG_CNT             "GF@__ARG_CNT"

#define CREATEFRAME         "CREATEFRAME"
#define PUSHFRAME           "PUSHFRAME"
#define POPFRAME            "POPFRAME"
#define LABEL               "LABEL"
#define JUMP                "JUMP"
#define IFJ_TYPE            "TYPE"
#define RETURN              "RETURN"
#define EXIT                "EXIT int@0"
#define CALL                "CALL"

#define DEFVAR          "DEFVAR"
#define LOCAL_VAR       "LF@"
#define TMP_VAR         "TF@"
#define POPS            "POPS"
#define PUSHS           "PUSHS"
#define IFJ_INTEGER     "int@"
#define IFJ_FLOAT       "float@"
#define IFJ_STRING      "string@"
#define ADDS            "ADDS"
#define SUBS            "SUBS"
#define MULS            "MULS"
#define DIVS            "DIVS"
#define EQS             "EQS"
#define NOTS            "NOTS"

#define NUMERIC_CONVERTER "\
#NUMERNUMERIC_CONVERTER_START\n\
POPS GF@__TMP2\n\
POPS GF@__TMP1\n\
TYPE GF@__TMP3 GF@__TMP2\n\
PUSHS GF@__TMP3\n\
TYPE GF@__TMP3 GF@__TMP1\n\
PUSHS GF@__TMP3\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_1\n\
PUSHS GF@__TMP1\n\
PUSHS GF@__TMP2\n\
JUMP __CONV_%d_2\n\
LABEL __CONV_%d_1\n\
PUSHS GF@__TMP3\n\
PUSHS string@float\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_3\n\
PUSHS GF@__TMP1\n\
PUSHS GF@__TMP2\n\
INT2FLOATS\n\
JUMP __CONV_%d_2\n\
LABEL __CONV_%d_3\n\
PUSHS GF@__TMP1\n\
INT2FLOATS\n\
PUSHS GF@__TMP2\n\
LABEL __CONV_%d_2\n\
#NUMERNUMERIC_CONVERTER_END\n"

#define FLOAT_CONVERTER "\
#FLOAT_CONVERTER\n\
POPS GF@__TMP2\n\
POPS GF@__TMP1\n\
TYPE GF@__TMP3 GF@__TMP1\n\
PUSHS GF@__TMP3\n\
PUSHS string@float\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_1\n\
PUSHS GF@__TMP1\n\
LABEL __CONV_%d_2\n\
TYPE GF@__TMP3 GF@__TMP2\n\
PUSHS GF@__TMP3\n\
PUSHS string@float\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_3\n\
PUSHS GF@__TMP2\n\
JUMP __CONV_%d_4\n\
LABEL __CONV_%d_1\n\
PUSHS GF@__TMP1\n\
INT2FLOATS\n\
JUMP __CONV_%d_2\n\
LABEL __CONV_%d_3\n\
PUSHS GF@__TMP2\n\
INT2FLOATS\n\
JUMP __CONV_%d_4\n\
LABEL __CONV_%d_4\n\
#FLOAT_CONVERT_END"

#define CONCATENATION "\
#CONCATENATION_START\n\
POPS GF@__TMP2\n\
POPS GF@__TMP1\n\
TYPE GF@__TMP3 GF@__TMP1\n\
PUSHS GF@__TMP3\n\
PUSHS string@string\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFNEQS __CONV_%d_1\n\
MOVE GF@__TMP1 string@\n\
LABEL __CONV_%d_1\n\
TYPE GF@__TMP3 GF@__TMP2\n\
PUSHS GF@__TMP3\n\
PUSHS string@string\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFNEQS __CONV_%d_2\n\
MOVE GF@__TMP2 string@\n\
LABEL __CONV_%d_2\n\
CONCAT GF@__TMP1 GF@__TMP1 GF@__TMP2\n\
PUSHS GF@__TMP1\n\
#CONCATENATION_END\n"

#define EQVIVAL "\
#EQVIVAL\n\
POPS GF@__TMP2\n\
POPS GF@__TMP1\n\
TYPE GF@__TMP3 GF@__TMP1\n\
PUSHS GF@__TMP3\n\
TYPE GF@__TMP3 GF@__TMP2\n\
PUSHS GF@__TMP3\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_1\n\
PUSHS GF@__TMP1\n\
PUSHS GF@__TMP2\n\
EQS\n\
JUMP __CONV_%d_2\n\
LABEL __CONV_%d_1\n\
PUSHS bool@false\n\
LABEL __CONV_%d_2\n\
#EQVIVAL_END\n"

#define FUNEND "\
POPFRAME\n\
RETURN\n"

#define LOGIC_CHECK "\
#LOGIC_CHECK_START\n\
POPS GF@__TMP1\n\
PUSHS GF@__TMP1\n\
TYPE GF@__TMP2 GF@__TMP1\n\
PUSHS GF@__TMP2\n\
PUSHS string@bool\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_1\n\
PUSHS bool@false\n\
EQS\n\
NOTS\n\
JUMP __CONV_%d_END\n\
LABEL __CONV_%d_1\n\
PUSHS GF@__TMP2\n\
PUSHS string@int\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_2\n\
PUSHS int@0\n\
EQS\n\
NOTS\n\
JUMP __CONV_%d_END\n\
LABEL __CONV_%d_2\n\
PUSHS GF@__TMP2\n\
PUSHS string@float\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_3\n\
PUSHS float@0x0.0p+0\n\
EQS\n\
NOTS\n\
JUMP __CONV_%d_END\n\
LABEL __CONV_%d_3\n\
PUSHS GF@__TMP2\n\
PUSHS string@string\n\
EQS\n\
PUSHS bool@false\n\
JUMPIFEQS __CONV_%d_4\n\
PUSHS string@\n\
EQS\n\
NOTS\n\
JUMP __CONV_%d_END\n\
LABEL __CONV_%d_4\n\
POPS GF@__TMP2\n\
PUSHS bool@false\n\
LABEL __CONV_%d_END\n\
#LOGIC_CHECK_END\n"

#define WRITE_FUN "\
LABEL write\n\
PUSHFRAME\n\
LABEL __WRITE_WHILE\n\
PUSHS GF@__ARG_CNT\n\
PUSHS int@0\n\
JUMPIFEQS __WRITE_WHILE_END\n\
POPS GF@__TMP1\n\
WRITE GF@__TMP1\n\
SUB GF@__ARG_CNT GF@__ARG_CNT int@1\n\
JUMP __WRITE_WHILE\n\
LABEL __WRITE_WHILE_END\n\
POPFRAME\n\
RETURN\n"

#define READS_FUN "\
LABEL reads\n\
PUSHFRAME\n\
READ GF@__TMP1 string\n\
PUSHS GF@__TMP1\n\
POPFRAME\n\
RETURN\n"

#define READI_FUN "\
LABEL readi\n\
PUSHFRAME\n\
READ GF@__TMP1 int\n\
PUSHS GF@__TMP1\n\
POPFRAME\n\
RETURN\n"

#define READF_FUN "\
LABEL readf\n\
PUSHFRAME\n\
READ GF@__TMP1 float\n\
PUSHS GF@__TMP1\n\
POPFRAME\n\
RETURN\n"

#define SUBSTRING_FUN "\
LABEL substring\n\
PUSHFRAME\n\
DEFVAR LF@s\n\
DEFVAR LF@i\n\
DEFVAR LF@j\n\
POPS LF@s\n\
POPS LF@i\n\
POPS LF@j\n\
DEFVAR LF@nil_check\n\
EQ LF@nil_check LF@s nil@nil\n\
JUMPIFEQ substring_err_nil LF@nil_check bool@true\n\
EQ LF@nil_check LF@i nil@nil\n\
JUMPIFEQ substring_err_nil LF@nil_check bool@true\n\
EQ LF@nil_check LF@j nil@nil\n\
JUMPIFEQ substring_err_nil LF@nil_check bool@true\n\
DEFVAR LF@jlti\n\
LT LF@jlti LF@j LF@i\n\
JUMPIFEQ substring_empty LF@jlti bool@true\n\
SUB LF@i LF@i int@1\n\
DEFVAR LF@ret\n\
MOVE LF@ret string@\n\
DEFVAR LF@aux\n\
DEFVAR LF@jumpvar\n\
MOVE LF@jumpvar LF@i\n\
label cykleStart\n\
LT LF@jumpvar LF@i LF@j\n\
JUMPIFEQ cykleEnd LF@jumpvar bool@false\n\
GETCHAR LF@aux LF@s LF@i\n\
CONCAT LF@ret LF@ret LF@aux\n\
ADD LF@i LF@i int@1\n\
JUMP cykleStart\n\
label cykleEnd\n\
PUSHS LF@ret\n\
POPFRAME\n\
RETURN\n\
LABEL substring_empty\n\
DEFVAR LF@empty_str\n\
MOVE LF@empty_str string@\n\
PUSHS LF@empty_str\n\
POPFRAME\n\
RETURN\n\
LABEL substring_err_nil\n\
EXIT int@8\n"

#define ORD_FUN "\
LABEL ord\n\
PUSHFRAME\n\
DEFVAR LF@returned\n\
DEFVAR LF@param1\n\
POPS LF@param1\n\
PUSHS LF@param1\n\
PUSHS string@\n\
JUMPIFEQS ord-empty\n\
DEFVAR LF@param1-type\n\
TYPE LF@param1-type LF@param1\n\
JUMPIFNEQ ord-exit LF@param1-type string@string\n\
STRI2INT LF@returned LF@param1 int@0\n\
PUSHS LF@returned\n\
JUMP ord-return\n\
LABEL ord-exit\n\
PUSHS int@0\n\
POPFRAME\n\
RETURN\n\
LABEL ord-return\n\
POPFRAME\n\
RETURN\n\
LABEL ord-empty\n\
PUSHS int@0\n\
POPFRAME\n\
RETURN\n"

#define INTVAL_FUN "\
LABEL intval\n\
DEFVAR TF@ret\n\
DEFVAR TF@param1\n\
PUSHFRAME\n\
POPS LF@param1\n\
DEFVAR LF@param1-type\n\
TYPE LF@param1-type LF@param1\n\
JUMPIFEQ intToint LF@param1-type string@int\n\
JUMPIFEQ floatToint LF@param1-type string@float\n\
JUMPIFEQ nilToint LF@param1-type string@nil\n\
LABEL intToint\n\
PUSHS LF@param1\n\
POPFRAME\n\
RETURN\n\
LABEL floatToint\n\
FLOAT2INT LF@ret LF@param1\n\
PUSHS LF@ret\n\
POPFRAME\n\
RETURN\n\
LABEL nilToint\n\
PUSHS int@0\n\
POPFRAME\n\
RETURN\n\
POPFRAME\n\
RETURN\n"

#define FLOATVAL_FUN "\
LABEL floatval\n\
DEFVAR TF@ret\n\
DEFVAR TF@param1\n\
PUSHFRAME\n\
POPS LF@param1\n\
DEFVAR LF@param1-type\n\
TYPE LF@param1-type LF@param1\n\
JUMPIFEQ intTofloat LF@param1-type string@int\n\
JUMPIFEQ floatTofloat LF@param1-type string@float\n\
JUMPIFEQ nilTofloat LF@param1-type string@nil\n\
LABEL intTofloat\n\
INT2FLOAT LF@ret LF@param1\n\
PUSHS LF@ret\n\
POPFRAME\n\
RETURN\n\
LABEL floatTofloat\n\
PUSHS LF@param1\n\
POPFRAME\n\
RETURN\n\
LABEL nilTofloat\n\
PUSHS float@0x0.0p+0\n\
POPFRAME\n\
RETURN\n\
POPFRAME\n\
RETURN\n"

#define STRVAL_FUN "\
LABEL strval\n\
PUSHFRAME\n\
DEFVAR LF@ret\n\
DEFVAR LF@param1\n\
POPS LF@param1\n\
DEFVAR LF@param1-type\n\
TYPE LF@param1-type LF@param1\n\
JUMPIFEQ stringTostr LF@param1-type string@string\n\
JUMPIFEQ nilTostr LF@param1-type string@nil\n\
LABEL nilTostr\n\
PUSHS string@\n\
POPFRAME\n\
RETURN\n\
LABEL stringTostr\n\
PUSHS LF@param1\n\
POPFRAME\n\
RETURN\n"

#define CHR_FUN "\
LABEL chr\n\
DEFVAR TF@returned\n\
DEFVAR TF@param1\n\
PUSHFRAME\n\
POPS LF@param1\n\
DEFVAR LF@param1-type\n\
TYPE LF@param1-type LF@param1\n\
JUMPIFNEQ chr-exit LF@param1-type string@int\n\
INT2CHAR LF@returned LF@param1\n\
PUSHS LF@returned\n\
JUMP chr-return\n\
LABEL chr-exit\n\
EXIT int@4\n\
LABEL chr-return\n\
POPFRAME\n\
RETURN\n"

typedef struct Generator {
    int counter;
    int currentPosition;
    int subPosition;
    int constrCnt;
    bool global;
    bst_t * globalVars;
    bst_t * localVars;
    bst_t * functions;
} generator_t;

int generateProgram(program_t prog);

#endif