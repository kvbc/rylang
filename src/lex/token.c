#include "token.h"
#include "token_code.h"

#include <stdint.h>
#include <stdio.h>
#include <limits.h>

// 
// [Static]
// 

static const char * kwcode_to_str (enum ryL_TokenCode code) {
    RY_ASSERT(IS_KWCODE(code));
    switch( code ) {
        case TK_KW_LOOP:     return "loop";
        case TK_KW_BREAK:    return "break";
        case TK_KW_CONTINUE: return "continue";
        case TK_KW_ELSE:     return "else";
        case TK_KW_ELIF:     return "elif";
        case TK_KW_IF:       return "if";
        case TK_KW_FALSE:    return "false";
        case TK_KW_TRUE:     return "true";
        case TK_KW_AS:       return "as";

        case TK_KW_BOOL:  return "bool";
        case TK_KW_I8:    return "i8";
        case TK_KW_I16:   return "i16";
        case TK_KW_I32:   return "i32";
        case TK_KW_I64:   return "i64";
        case TK_KW_I128:  return "i128";
        case TK_KW_ISIZE: return "isize";
        case TK_KW_U8:    return "u8";
        case TK_KW_U16:   return "u16";
        case TK_KW_U32:   return "u32";
        case TK_KW_U64:   return "u64";
        case TK_KW_U128:  return "u128";
        case TK_KW_USIZE: return "usize";
        case TK_KW_CHAR:  return "char";
        case TK_KW_F32:   return "f32";
        case TK_KW_F64:   return "f64";

        default: return NULL;
    }
}

static unsigned long kwcode_to_strhash (enum ry_LexerTokenCode code) {
    RY_ASSERT(IS_KWCODE(code));
    static unsigned long code_hashes[RYL_TOKEN_CODE_KW_COUNT] = {0};
    const size_t idx = code - (TK__KW_FIRST + 1);
    if( code_hashes[idx] == 0 ) {
        const char * kwstr = kwcode_to_str(code);
        code_hashes[idx] = ry_cstr_hash(kwstr, strlen(kwstr));
    }
    return code_hashes[idx];
}

// 
// [Public]
// 

void ry_LexerToken_free( struct ryL_Token * tk ) {
    if( tk->value_type == TKVAL_STRING )
        ry_String_free(&(tk->value.str));
}

enum ry_LexerTokenCode ry_LexerToken_string_to_keyword (const char * str, size_t len) {
    unsigned long strhash = ry_cstr_hash(str, len);
    for( enum ry_LexerTokenCode kwcode = TK__KW_FIRST + 1; kwcode < TK__KW_LAST; kwcode++ ) {
        unsigned long kwhash = kwcode_to_strhash(kwcode);
        if( strhash == kwhash )
            return kwcode;
    }
    return TK_NONE;
}

void ry_LexerToken_to_string( struct ry_LexerToken * tk, struct ry_String * outStr ) {
    
}

// 
// [Debug]
// 

void ryL_Token_to_string( struct ryL_Token * tk, struct ryUSTR_DynStr * out_str ) {
    switch( tk->code ) {
        case TK_NONE: return "NONE";

        case TK_NAME: return ""
    }
}

void ry_LexerToken_print( struct ryL_Token * token ) {
    switch( token->code ) {
        case TK_NONE: printf("none"); break;  
        
        case TK_NAME: printf("name \"%.*s\" (len: %u)", token->value.str.len, token->value.str.buf, token->value.str.len); break;
        case TK_HASH: printf("#"); break;
        case TK_SEMI: printf(";"); break;
        case TK_STRING: printf("\"%.*s\" (len: %u)", token->value.str.len, token->value.str.buf, token->value.str.len); break;
        case TK_NUMBER: printf("number: %d", token->value.num); break;
        case TK_OP_RND_BRACK: printf("("); break;
        case TK_CL_RND_BRACK: printf(")"); break;
        case TK_OP_CRL_BRACK: printf("{"); break;
        case TK_CL_CRL_BRACK: printf("}"); break;
        case TK_OP_SQ_BRACK: printf("["); break;
        case TK_CL_SQ_BRACK: printf("]"); break;
        case TK_EXMARK: printf("!"); break;

        // 
        // Arithmetic Operators
        // 

        case TK_ADD:  printf("+"); break;
        case TK_SUB:  printf("-"); break;
        case TK_DIV:  printf("/"); break;
        case TK_MUL:  printf("*"); break;
        case TK_MOD:  printf("%"); break;
        case TK_BXOR: printf("^"); break;
        case TK_BAND: printf("&"); break;
        case TK_BOR:  printf("|"); break;
        case TK_BNOT: printf("~"); break;
        case TK_BLS:  printf("<<"); break;
        case TK_BRS:  printf(">>"); break;

        case TK_SET:       printf("="); break;
        case TK_SET_ADD:   printf("+="); break;
        case TK_SET_SUB:   printf("-="); break;
        case TK_SET_DIV:   printf("/="); break;
        case TK_SET_MUL:   printf("*="); break;
        case TK_SET_MOD:   printf("%="); break;
        case TK_SET_BXOR:  printf("^="); break;
        case TK_SET_BAND:  printf("&="); break;
        case TK_SET_BOR:   printf("|="); break;
        case TK_SET_BLS:   printf("<<="); break;
        case TK_SET_BRS:   printf(">>="); break;

        case TK_INC: printf("++"); break;
        case TK_DEC: printf("--"); break;

        // 
        // Logical Operators
        // 

        case TK_EQ:    printf("=="); break;
        case TK_EQ_GR: printf(">="); break;
        case TK_EQ_LE: printf("<="); break;
        case TK_NE:    printf("!="); break;
        case TK_AND:   printf("&&"); break;
        case TK_OR:    printf("||"); break;
        case TK_GR:    printf(">"); break;
        case TK_LE:    printf("<"); break;

        case TK_QM:    printf("?"); break;
        case TK_COLON: printf(":"); break;

        default:
            if( IS_KWCODE(token->code) ) {
                const char * kwstr = kwcode_to_str(token->code);
                unsigned long kwhash = kwcode_to_strhash(token->code);
                printf("keyword \"%s\" (hash: %ul, len: %u)", kwstr, kwhash, strlen(kwstr));
            }
            else printf("[???]");
    }
}