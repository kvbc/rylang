#ifndef RYL_TOKEN_H
#define RYL_TOKEN_H

#include "../util/str.h"
#include "../ry.h"
#include "../util/dyn_arr.h"
#include "lexer.h"
#include "lex.h"
#include "token_code.h"
#include "../util/strings.h"

// 
// [Public]
// 

enum ryL_TokenValueType {
    TKVAL_NONE,
    TKVAL_STRING,
    TKVAL_INT,
    TKVAL_FLOAT
};

struct ryL_Token {
    enum ryL_TokenCode _code;
    union {
        const struct ryU_DynArr * str;
        ryL_int_t intv;
        ryL_float_t floatv;
        ryL_char_t charv;
    } _value;
    enum ryL_TokenValueType _value_type;
};

void ryL_Token_init( struct ryL_Token * tk );

void ryL_Token_set       ( struct ryL_Token * tk, enum ryL_TokenCode code );
void ryL_Token_set_string( struct ryL_Token * tk, enum ryL_TokenCode code, const struct ryU_DynArr * str );
void ryL_Token_set_int   ( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_int_t intv );
void ryL_Token_set_float ( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_float_t floatv );

enum ryL_TokenCode ryL_Token_string_to_keyword( const struct ryU_DynArr * str, struct ryU_Strings * strings );

// 
// Debug
// 

#ifdef RY_DEBUG
void ryL_Token_to_string( struct ryL_Token * tk, struct ryU_ArrView * out_strview, struct ryU_Arr * out_str, bool * out_is_view );
#endif

// 
// Getters
// 

enum ryL_TokenCode      ryL_Token_get_code      ( const struct ryL_Token * tk );
enum ryL_TokenValueType ryL_Token_get_value_type( const struct ryL_Token * tk );
// value
const struct ryU_DynArr * ryL_Token_get_string( const struct ryL_Token * tk );
ryL_int_t                 ryL_Token_get_int   ( const struct ryL_Token * tk );
ryL_float_t               ryL_Token_get_float ( const struct ryL_Token * tk );

// 
// 
// 

#endif // RYL_TOKEN_H