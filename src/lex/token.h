#ifndef RYL_TOKEN_H
#define RYL_TOKEN_H

#include "../util/str/cstr.h"
#include "../ry.h"
#include "../util/str/dyn_str.h"
#include "lexer.h"
#include "lex.h"
#include "token_code.h"

// 
// [Public]
// 

enum ryL_TokenValueType {
    TKVAL_NONE,
    TKVAL_STRING,
    TKVAL_INT,
    TKVAL_FLOAT,
    TKVAL_CHAR
};

struct ryL_Token {
    enum ryL_TokenCode _code;
    union {
        struct ryUSTR_DynStr str;
        ryL_int_t intv;
        ryL_float_t floatv;
        ryL_char_t charv;
    } _value;
    enum ryL_TokenValueType _value_type;
};

void ryL_Token_init( struct ryL_Token * tk );
void ryL_Token_free( struct ryL_Token * tk );

void ryL_Token_set      ( struct ryL_Token * tk, enum ryL_TokenCode code );
struct ryUSTR_DynStr * ryL_Token_set_string( struct ryL_Token * tk, enum ryL_TokenCode code );
void ryL_Token_set_int  ( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_int_t intv );
void ryL_Token_set_float( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_float_t floatv );
void ryL_Token_set_char ( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_char_t charv );

enum ryL_TokenCode ryL_Token_string_to_keyword( const u8 * str, usize len );
void ryL_Token_to_string( struct ryL_Token * tk, struct ryUSTR_DynStr * out_str );

// 
// Getters
// 

enum ryL_TokenCode ryL_Token_get_code( const struct ryL_Token * tk );
enum ryL_TokenValueType ryL_Token_get_value_type( const struct ryL_Token * tk );
// value
const struct ryUSTR_DynStr * ryL_Token_get_string( const struct ryL_Token * tk );
ryL_int_t   ryL_Token_get_int  ( const struct ryL_Token * tk );
ryL_float_t ryL_Token_get_float( const struct ryL_Token * tk );
ryL_char_t  ryL_Token_get_char ( const struct ryL_Token * tk );

// 
// 
// 

#endif // RYL_TOKEN_H