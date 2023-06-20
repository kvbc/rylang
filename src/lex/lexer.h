#ifndef RY_LEXER_H
#define RY_LEXER_H

#include "token.h"

#include "util/str/alloc_str.h"
#include "util/str/str_view.h"
#include "util/array.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// 
// Private
// 

struct ry__LexerPos {
    usize ln;
    usize col;
};

//
// Public
// 

struct ry_Lexer {
    struct ryUSTR_StringView id;
    const struct ryUSTR_StringView * src;

    usize src_idx;
    struct ry__LexerPos pos;
    
    struct ryU_Array tokens; // ry_LexerToken[]
    struct ryU_Array infos; // ry_LexerInfo[]
};

void ry_lex (struct ry_Lexer * lex);

// 
// 
// 

#endif // RY_LEXER_H