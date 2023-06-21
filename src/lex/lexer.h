#ifndef RYL_LEXER_H
#define RYL_LEXER_H

#include "token.h"

#include "../util/str/alloc_str.h"
#include "../util/str/str_view.h"
#include "../util/array.h"

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

enum ryL_LexerFlags {
    RYL_LEXFLAG_NONE          = 0,
    RYL_LEXFLAG_COLLECT_INFOS = 1
};

struct ryL_Lexer {
    struct ryUSTR_DynStr * id;
    struct ryUSTR_DynStr * src;

    enum ryL_LexerFlags flags;

    usize src_idx;
    struct ryL_Lexer__Pos pos;
    
    struct ryU_Array tokens; // ry_LexerToken[]
    struct ryU_Array infos; // ry_LexerInfo[]
};

void ryL_Lexer_init( struct ryL_Lexer * out_lex, enum ryL_LexerFlags flags, struct ryUSTR_DynStr * id, struct ryUSTR_DynStr * src );
void ryL_Lexer_lex( struct ryL_Lexer * lex );
void ryL_Lexer_free( struct ryL_Lexer * lex );

void ryL_Lexer_print_infos( struct ryL_Lexer * lex );
void ryL_Lexer_print_tokens( struct ryL_Lexer * lex );

// 
// 
// 

#endif // RYL_LEXER_H