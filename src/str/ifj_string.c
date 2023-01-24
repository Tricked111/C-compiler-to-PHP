/******************************************************************************
 *                                  IFJ22
 *                              ifj_string.c
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *               Erik Hrubý        (xhruby30)
 *      Purpose: Declaration of operations on strings
 * 
 *                        Last change: 07. 12. 2022
 *****************************************************************************/

#include "ifj_string.h"
#include <stdio.h>
#include <stdlib.h>

int stringInit(string_t * str) {
    if ((str->str = malloc(sizeof(char) * (DEFAULT_STRING_LENGTH + 1))) == NULL)
        return 1;
    str->len = 0;
    str->allocated = DEFAULT_STRING_LENGTH;
    str->str[0] = '\0';
    return 0;
}

void stringFree(string_t * str) {
    free(str->str);
}

int stringAppend(string_t * str, char newChar) {
    if (str->len == str->allocated) {
        if ((str->str = realloc(str->str, sizeof(char) * (str->allocated + DEFAULT_STRING_LENGTH + 1))) == NULL)
            return 1;
        str->allocated += DEFAULT_STRING_LENGTH;
    }
    str->str[str->len] = newChar;
    str->len++;
    str->str[str->len] = '\0';
    return 0;
}

int stringClear(string_t * str)
{
    stringFree(str);
    if (stringInit(str))
        return 1;
    return 0;
}

void stringCopy(string_t * strDest, string_t * strSource)
{
    if ((strDest->str = malloc(sizeof(char) * strSource->allocated + 1)) == NULL)
        fprintf(stderr, "String allocation failure\n");
    strDest->len = strSource->len;
    strDest->allocated = strSource->allocated;
    strcpy(strDest->str, strSource->str);
}

char * stringRead(string_t * str)
{
    return str->str;
}

int stringLength(string_t * str)
{
    return str->len;
}

string_t convertToIFJCode(string_t * str) {
    char c;
    string_t converted;
    stringInit(&converted);
    for (size_t i = 0; i < str->len; i++) {
        c = str->str[i];
        if (c <= 32 || c == 35 || c == 34 || c == 10 || c == 11) {
            stringAppendCode(&converted, c);
        }
        else if (c == 92) {
            i++;
            c = str->str[i];
            switch (c) {
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\\':
                    c = '\\';
                    break;
                default:
                    break;
            }
            stringAppendCode(&converted, c);
        }    
        else {
            stringAppend(&converted, c);
        }
    }
    return converted;
}

void stringAppendCode(string_t * str, int code) {
    stringAppend(str, '\\');
    stringAppend(str, (code / 100) + '0');
    stringAppend(str, (code / 10) + '0');
    stringAppend(str, (code % 10) + '0');
}