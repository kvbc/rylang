#ifndef RYL_TOKEN_CODE_H
#define RYL_TOKEN_CODE_H

#include "lex.h"

enum ryL_TokenCode {
    TK__FIRST = RYL_CHAR_MAX + 1,

    TK_NONE,

    TK_NAME, // name
    TK_STRING, // "string"
    TK_INT, // 123
    TK_FLOAT, // 123.123
    TK_CHAR,

    // 
    // Keywords
    // 

    TK__KW_FIRST,

    TK_KW_IF,
    TK_KW_ELIF,
    TK_KW_ELSE,
    TK_KW_LOOP,
    TK_KW_CONTINUE,
    TK_KW_BREAK,
    TK_KW_FALSE,
    TK_KW_TRUE,
    TK_KW_AS,

    TK_KW_BOOL,
    TK_KW_I8,
    TK_KW_I16,
    TK_KW_I32,
    TK_KW_I64,
    TK_KW_I128,
    TK_KW_ISIZE,
    TK_KW_U8,
    TK_KW_U16,
    TK_KW_U32,
    TK_KW_U64,
    TK_KW_U128,
    TK_KW_USIZE,
    TK_KW_CHAR,
    TK_KW_F32,
    TK_KW_F64,

    TK__KW_LAST,

    // 
    // Special Operators
    // 

    // Arithmetic
    TK_OP_POWER,
    TK_OP_FLOORDIV,
    // set
    TK_OP_SET_ADD,
    TK_OP_SET_SUB,
    TK_OP_SET_MUL,
    TK_OP_SET_POWER,
    TK_OP_SET_DIV,
    TK_OP_SET_FLOORDIV,
    TK_OP_SET_MOD,

    // Bitwise
    TK_OP_BLSHIFT,
    TK_OP_BRSHIFT,
    // set
    TK_OP_SET_BOR,
    TK_OP_SET_BXOR,
    TK_OP_SET_BAND,
    TK_OP_SET_BLSHIFT,
    TK_OP_SET_BRSHIFT,

    // Comparison
    TK_OP_EQ,
    TK_OP_NE,
    TK_OP_LE,
    TK_OP_GE,

    // Logical
    TK_OP_OR,
    TK_OP_AND,

    // Other
    TK_OP_IDX // ..
};

#define RYL_TOKEN_CODE_KW_COUNT (TK__KW_LAST - TK__KW_FIRST - 1)

bool ryL_TokenCode_is_keyword( enum ryL_TokenCode code );
enum ryL_TokenCode ryL_TokenCode_from_char( u8 c );

#endif // RYL_TOKEN_CODE_H