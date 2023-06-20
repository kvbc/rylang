#ifndef RYL_LEXER_H
#define RYL_LEXER_H

#include "token.h"

#include "util/str/alloc_str.h"
#include "util/str/str_view.h"
#include "util/array.h"

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
    struct ryUSTR_DynStr id;
    struct ryUSTR_DynStr src;

    usize src_idx;
    struct ryL_Lexer__Pos pos;
    
    struct ryU_Array tokens; // ry_LexerToken[]
    struct ryU_Array infos; // ry_LexerInfo[]
};

void ryL_lex( struct ryL_Lexer * lex );

// 
// 
// 

#endif // RYL_LEXER_H