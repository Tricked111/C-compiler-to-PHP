#ifndef COMP_ERRORS_H
#define COMP_ERROR_H
    enum Errors {
        LEXICAL_ERR     = 1,
        SYNTAX_ERR      = 2,
        FUNC_DEF_ERR    = 3,
        FUNC_TYPE_ERR   = 4,
        UNDEF_ERR       = 5,
        RET_EXPR_ERR    = 6,
        TYPE_EXPR_ERR   = 7,
        SEMANTIC_ERR    = 8,
        INTERN_ERR      = 99
    };

#endif