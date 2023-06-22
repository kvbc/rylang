#include "token.h"
#include "token_code.h"
#include "../util/arr.h"
#include "../util/str.h"

#include <stdint.h>
#include <stdio.h>
#include <limits.h>

// 
// Static
// 
#pragma region

static const u8 * kwcode_to_str( enum ryL_TokenCode code ) {
    RY_ASSERT(ryL_TokenCode_is_keyword(code));

    const char * str = NULL;

    switch( code ) {
        #define CASE(tkcode, strlit) case tkcode: str = strlit; break

        CASE(TK_KW_LOOP,     "loop");
        CASE(TK_KW_BREAK,    "break");
        CASE(TK_KW_CONTINUE, "continue");
        CASE(TK_KW_ELSE,     "else");
        CASE(TK_KW_ELIF,     "elif");
        CASE(TK_KW_IF,       "if");
        CASE(TK_KW_FALSE,    "false");
        CASE(TK_KW_TRUE,     "true");
        CASE(TK_KW_AS,       "as");

        CASE(TK_KW_BOOL,  "bool");
        CASE(TK_KW_I8,    "i8");
        CASE(TK_KW_I16,   "i16");
        CASE(TK_KW_I32,   "i32");
        CASE(TK_KW_I64,   "i64");
        CASE(TK_KW_I128,  "i128");
        CASE(TK_KW_U8,    "u8");
        CASE(TK_KW_U16,   "u16");
        CASE(TK_KW_U32,   "u32");
        CASE(TK_KW_U64,   "u64");
        CASE(TK_KW_U128,  "u128");
        CASE(TK_KW_F32,   "f32");
        CASE(TK_KW_F64,   "f64");

        default:
            RY_ASSERT(false);
            return NULL;

        #undef CASE
    }

    return (const u8 *)str;
}

static usize kwcode_to_strhash (enum ryL_TokenCode code) {
    RY_ASSERT(ryL_TokenCode_is_keyword(code));
    static usize code_hashes[RYL_TOKEN_CODE_KW_COUNT] = {0};
    const size_t idx = code - (TK__KW_FIRST + 1);
    if( code_hashes[idx] == 0 ) {
        const u8 * kwstr = kwcode_to_str(code);
        code_hashes[idx] = ryU_cstr_hash(kwstr, ryU_cstr_len(kwstr));
    }
    return code_hashes[idx];
}

static struct ryU_DynArr * Token_get_string( struct ryL_Token * tk ) {
    return (struct ryU_DynArr *)ryL_Token_get_string((const struct ryL_Token *)tk);
}

#pragma endregion
// 
// Public
// 
#pragma region public

void ryL_Token_init( struct ryL_Token * tk ) {
    tk->_code = TK_NONE;
    tk->_value_type = TKVAL_NONE;
}

void ryL_Token_free( struct ryL_Token * tk ) {
    if( tk->_value_type == TKVAL_STRING )
        ryU_DynArr_free(Token_get_string(tk));
}

// set
#pragma region set

void ryL_Token_set( struct ryL_Token * tk, enum ryL_TokenCode code ) {
    ryL_Token_free(tk);
    tk->_code = code;
    tk->_value_type = TKVAL_NONE;
}
void ryL_Token_set_string( struct ryL_Token * tk, enum ryL_TokenCode code, struct ryU_DynArr * str ) {
    ryL_Token_free(tk);
    tk->_code = code;
    tk->_value_type = TKVAL_STRING;
    tk->_value.str = str;
}
void ryL_Token_set_int( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_int_t intv ) {
    ryL_Token_free(tk);
    tk->_code = code;
    tk->_value_type = TKVAL_INT;
    tk->_value.intv = intv;
}
void ryL_Token_set_float( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_float_t floatv ) {
    ryL_Token_free(tk);
    tk->_code = code;
    tk->_value_type = TKVAL_FLOAT;
    tk->_value.floatv = floatv;
}
void ryL_Token_set_char( struct ryL_Token * tk, enum ryL_TokenCode code, ryL_char_t charv ) {
    ryL_Token_free(tk);
    tk->_code = code;
    tk->_value_type = TKVAL_CHAR;
    tk->_value.charv = charv;
}

#pragma endregion set
// 

enum ryL_TokenCode ryL_Token_string_to_keyword (const u8 * str, usize len) {
    usize strhash = ryU_cstr_hash(str, len);
    for(
        enum ryL_TokenCode kwcode = (enum ryL_TokenCode)(TK__KW_FIRST + 1);
        kwcode < TK__KW_LAST;
        kwcode = (enum ryL_TokenCode)((int)kwcode + 1)
    ) {
        usize kwhash = kwcode_to_strhash(kwcode);
        if( strhash == kwhash )
            return kwcode;
    }
    return TK_NONE;
}

#pragma endregion
// 
// Debug
// 
#ifdef RY_DEBUG

void ryL_Token_to_string( struct ryL_Token * tk, struct ryU_ArrView * out_strview, struct ryU_Arr * out_str, bool * out_is_view ) {
    const u8 * cstr = NULL;

    switch( tk->_code ) {
        case TK_CHAR: {
            ryU_str_format(out_str,
                ryU_cstr("char '%c'"),
                (char)tk->_code
            );
        }

        #define CASE_LIT(tkcode, strlit) case tkcode: cstr = ryU_cstr(strlit); break
        CASE_LIT(TK_NONE, "NONE");
        CASE_LIT(TK_OP_POWER,        "**");
        CASE_LIT(TK_OP_FLOORDIV,     "//");
        CASE_LIT(TK_OP_SET_ADD,      "+=");
        CASE_LIT(TK_OP_SET_SUB,      "-=");
        CASE_LIT(TK_OP_SET_MUL,      "*=");
        CASE_LIT(TK_OP_SET_POWER,    "**=");
        CASE_LIT(TK_OP_SET_DIV,      "/=");
        CASE_LIT(TK_OP_SET_FLOORDIV, "//=");
        CASE_LIT(TK_OP_SET_MOD,      "%=");
        CASE_LIT(TK_OP_BLSHIFT,      "<<");
        CASE_LIT(TK_OP_BRSHIFT,      ">>");
        CASE_LIT(TK_OP_SET_BOR,      "|=");
        CASE_LIT(TK_OP_SET_BXOR,     "^=");
        CASE_LIT(TK_OP_SET_BAND,     "&=");
        CASE_LIT(TK_OP_SET_BLSHIFT,  "<<=");
        CASE_LIT(TK_OP_SET_BRSHIFT,  ">>=");
        CASE_LIT(TK_OP_EQ,           "==");
        CASE_LIT(TK_OP_NE,           "!=");
        CASE_LIT(TK_OP_LE,           "<=");
        CASE_LIT(TK_OP_GE,           ">=");
        CASE_LIT(TK_OP_OR,           "||");
        CASE_LIT(TK_OP_AND,          "&&");
        CASE_LIT(TK_OP_IDX,          "..");
        #undef CASE_LIT

        default:
            if( tk->_code == TK_NAME || tk->_code == TK_STRING ) {
                const char * fmt = "name \"%.*s\"";
                if( tk->_code == TK_STRING )
                    fmt = "string \"%.*s\"";

                const struct ryU_DynArr * tk_str = ryL_Token_get_string(tk);
                ryU_str_format(out_str, ryU_cstr(fmt),
                    ryU_DynArr_get_len(tk_str),
                    ryU_DynArr_get_buf(tk_str)
                );
            }
            else if( tk->_code == TK_INT || tk->_code == TK_FLOAT ) {
                const char * fmt = "int %d";
                if( tk->_code == TK_FLOAT )
                    fmt = "float %f";

                ryU_str_format(out_str, ryU_cstr(fmt),
                    (tk->_code == TK_INT)
                        ? ryL_Token_get_int(tk)
                        : ryL_Token_get_float(tk)
                );
            }
            else if( ryL_TokenCode_is_keyword(tk->_code) ) {
                const u8 * kwstr = kwcode_to_str(tk->_code);
                usize kwhash = kwcode_to_strhash(tk->_code);
                ryU_str_format(out_str, ryU_cstr("keyword \"%s\" (hash: %ul)"), (const char *)kwstr, kwhash);
            }
            else if( ryL_TokenCode_is_char(tk->_code) ) {
                ryU_str_format(out_str, ryU_cstr("'%c'"), (char)tk->_code);
            }
            else RY_ASSERT(false);
    }

    if( cstr != NULL ) {
        ryU_ArrView_set(out_strview, cstr, ryU_cstr_len(cstr));
    }
}

#endif
// 
// Getters
// 
#pragma region

enum ryL_TokenCode ryL_Token_get_code( const struct ryL_Token * tk ) {
    return tk->_code;
}

enum ryL_TokenValueType ryL_Token_get_value_type( const struct ryL_Token * tk ) {
    return tk->_value_type;
}

// value

const struct ryU_DynArr * ryL_Token_get_string( const struct ryL_Token * tk ) {
    RY_ASSERT(tk->_value_type == TKVAL_STRING);
    return tk->_value.str;
}
ryL_int_t ryL_Token_get_int( const struct ryL_Token * tk ) {
    RY_ASSERT(tk->_value_type == TKVAL_INT);
    return tk->_value.intv;
}
ryL_float_t ryL_Token_get_float( const struct ryL_Token * tk ) {
    RY_ASSERT(tk->_value_type == TKVAL_FLOAT);
    return tk->_value.floatv;
}
ryL_char_t ryL_Token_get_char( const struct ryL_Token * tk ) {
    RY_ASSERT(tk->_value_type == TKVAL_CHAR);
    return tk->_value.charv;
}

#pragma endregion
// 
// 
// 