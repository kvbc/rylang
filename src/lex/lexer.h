#ifndef RYL_LEXER_H
#define RYL_LEXER_H

#include "token.h"

#include "../util/arr.h"
#include "../util/arr_view.h"
#include "../util/dyn_arr.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// 
// [Private]
// 

struct ryL_Lexer__Pos {
    usize ln;
    usize col;
};

//
// [Public]
// 

struct ryL_Lexer {
    struct ryU_DynArr * _id;
    struct ryU_DynArr * _src;

    usize _src_idx;
    struct ryL_Lexer__Pos _pos;

    struct ryU_Arr _str_views; // arr of ryU_ArrView<u8> (str view)
    struct ryU_Arr _dyn_strs; // arr of ryU_DynArr<u8> (dyn str)
    struct ryU_Arr _strs; // arr of ryU_Arr<u8> (str)

    struct ryU_Arr _tokens; // ryL_Token[]
    struct ryU_Arr _msgs; // LexerInfo[]
};

void ryL_Lexer_init( struct ryL_Lexer * out_lex, struct ryU_DynArr * id, struct ryU_DynArr * src );
void ryL_Lexer_lex( struct ryL_Lexer * lex );
void ryL_Lexer_free( struct ryL_Lexer * lex );

// 
// Debug
// 

#ifdef RY_DEBUG
void ryL_Lexer_msgs_to_string( struct ryL_Lexer * lex, struct ryU_Arr * out_str );
void ryL_Lexer_tokens_to_string( struct ryL_Lexer * lex, struct ryU_Arr * out_str );
#endif

// 
// 
// 

#endif // RYL_LEXER_H