#ifndef RYL_TOKEN_H
#define RYL_TOKEN_H

#include "util/str/cstr.h"
#include "core.h"
#include "util/str/dyn_str.h"
#include "lexer.h"
#include "lex.h"

// 
// [Public]
// 

enum ryL_TokenValueType {
    TKVAL_STRING,
    TKVAL_INT,
    TKVAL_FLOAT,
    TKVAL_CHAR
};

struct ryL_Token {
    enum ryL_TokenCode code;
    union {
        struct ryUSTR_DynStr str;
        ryL_int_t intv;
        ryL_float_t floatv;
        ryL_char_t charv;
    } value;
    enum ryL_TokenValueType value_type;
};

void ryL_Token_free( struct ryL_Token * tk );

enum ryL_TokenCode ryL_Token_string_to_keyword( const char * str, size_t len );

void ryL_Token_to_string( struct ryL_Token * tk, struct ryUSTR_DynStr * out_str );

// 
// 
// 

#endif // RYL_TOKEN_H