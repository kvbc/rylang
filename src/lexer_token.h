#ifndef RY_LEXER_TOKEN_H
#define RY_LEXER_TOKEN_H

#include "cstr.h"
#include "core.h"
#include <stdint.h>

enum ry_LexerTokenCode {
    TK_NONE,

    TK_NAME, // name
    TK_META, // @
    TK_SEMI, // ;
    TK_STRING, // "string"
    TK_NUMBER, // 123
    TK_OP_RND_BRACK, // (
    TK_CL_RND_BRACK, // )
    TK_OP_CRL_BRACK, // {
    TK_CL_CRL_BRACK, // }

    // 
    // Keywords
    // 

    TK__KW_FIRST,

    TK_KW_STRUCT,
    TK_KW_EXTEND,
    TK_KW_CONST,
    TK_KW_RETURN,
    TK_KW_CASE,
    TK_KW_SWITCH,
    TK_KW_FOR,
    TK_KW_DO,
    TK_KW_WHILE,
    TK_KW_BREAK,
    TK_KW_CONTINUE,
    TK_KW_ELSE,
    TK_KW_IF,
    TK_KW_FALSE,
    TK_KW_TRUE,

    TK_KW_VOID,
    TK_KW_BOOL,
    TK_KW_I8,
    TK_KW_I16,
    TK_KW_I32,
    TK_KW_I64,
    TK_KW_U8,
    TK_KW_U16,
    TK_KW_U32,
    TK_KW_U64,
    TK_KW_CHAR,
    TK_KW_F32,
    TK_KW_F64,

    TK__KW_LAST,

    // 
    // Arithmetic Operators
    // 

    TK_ADD,  // +
    TK_SUB,  // -
    TK_DIV,  // /
    TK_MUL,  // *
    TK_MOD,  // %
    TK_BXOR, // ^
    TK_BAND, // &
    TK_BOR,  // |
    TK_BNOT, // ~
    TK_BLS,  // <<
    TK_BRS,  // >>

    TK_SET,       // =
    TK_SET_ADD,   // +=
    TK_SET_SUB,   // -=
    TK_SET_DIV,   // /=
    TK_SET_MUL,   // *=
    TK_SET_MOD,   // %=
    TK_SET_BXOR,  // ^=
    TK_SET_BAND,  // &=
    TK_SET_BOR,   // |=
    TK_SET_BLS,   // <<=
    TK_SET_BRS,   // >>=

    TK_INC, // ++
    TK_DEC, // --

    // 
    // Logical Operators
    // 

    TK_EQ,    // ==
    TK_EQ_GR, // >=
    TK_EQ_LE, // <=
    TK_NE,    // !=
    TK_AND,   // &&
    TK_OR,    // ||
    TK_GR,    // >
    TK_LE,    // <
    TK_NOT,   // !

    TK_QM,    // ?
    TK_COLON, // :
};

#define KW_COUNT (TK__KW_LAST - TK__KW_FIRST - 1)
#define IS_KWCODE(code) ((code > TK__KW_FIRST) && (code < TK__KW_LAST))

struct ry_LexerToken {
    enum ry_LexerTokenCode code;
    union value {
        union str {
            const char * name;
            RY_SIZE_T namelen;
        };
        int64_t num;
    };
};

// 
// 
// 

static const char * kwcode_to_str (enum ry_LexerTokenCode code) {
    RY_ASSERT(IS_KWCODE(code));
    switch( code ) {
        case TK_KW_STRUCT:   return "struct";
        case TK_KW_EXTEND:   return "extend";
        case TK_KW_CONST:    return "const";
        case TK_KW_RETURN:   return "return";
        case TK_KW_CASE:     return "case";
        case TK_KW_SWITCH:   return "switch";
        case TK_KW_FOR:      return "for";
        case TK_KW_DO:       return "do";
        case TK_KW_WHILE:    return "while";
        case TK_KW_BREAK:    return "break";
        case TK_KW_CONTINUE: return "continue";
        case TK_KW_ELSE:     return "else";
        case TK_KW_IF:       return "if";
        case TK_KW_FALSE:    return "false";
        case TK_KW_TRUE:     return "true";

        case TK_KW_VOID: return "void";
        case TK_KW_BOOL: return "bool";
        case TK_KW_I8:   return "i8";
        case TK_KW_I16:  return "i16";
        case TK_KW_I32:  return "i32";
        case TK_KW_I64:  return "i64";
        case TK_KW_U8:   return "u8";
        case TK_KW_U16:  return "u16";
        case TK_KW_U32:  return "u32";
        case TK_KW_U64:  return "u64";
        case TK_KW_CHAR: return "char";
        case TK_KW_F32:  return "f32";
        case TK_KW_F64:  return "f64";
    }
    return NULL;
}

static unsigned long kwcode_to_strhash (enum ry_LexerTokenCode code) {
    RY_ASSERT(IS_KWCODE(code));
    static unsigned long code_hashes[KW_COUNT] = {0};
    const RY_SIZE_T idx = code - (TK__KW_FIRST + 1);
    if( code_hashes[idx] == 0 ) {
        const char * kwstr = kwcode_to_str(code);
        code_hashes[idx] = ry_cstr_hash(kwstr, strlen(kwstr));
    }
    return code_hashes[idx];
}

// 
// 
// 

struct ry_LexerToken ry_LexerToken_new (enum ry_LexerTokenCode code) {
    struct ry_LexerToken token;
    token.code = code;
    token.name = NULL;
    token.namelen = 0;
    return token;
}

enum ry_LexerTokenCode ry_LexerToken_string_to_keyword (const char * str, RY_SIZE_T len) {
    unsigned long strhash = ry_cstr_hash(str, len);
    for( enum ry_LexerTokenCode kwcode = TK__KW_FIRST + 1; kwcode < TK__KW_LAST; kwcode++ ) {
        unsigned long kwhash = kwcode_to_strhash(kwcode);
        if( strhash == kwhash )
            return kwcode;
    }
    return TK_NONE;
}

// 
// 
// 

void ry_LexerToken_print (struct ry_LexerToken * token) {
    switch( token->code ) {
        case TK_NONE: printf("none"); break;  
        
        case TK_NAME: printf("name: %.*s", token->namelen, token->name); break;
        case TK_META: printf("@"); break;
        case TK_SEMI: printf(";"); break;
        case TK_OP_RND_BRACK: printf("("); break;
        case TK_CL_RND_BRACK: printf(")"); break;
        case TK_OP_CRL_BRACK: printf("{"); break;
        case TK_CL_CRL_BRACK: printf("}"); break;

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
        case TK_NOT:   printf("!"); break;

        case TK_QM:    printf("?"); break;
        case TK_COLON: printf(":"); break;

        default:
            if( IS_KWCODE(token->code) ) {
                const char * kwstr = kwcode_to_str(token->code);
                unsigned long kwhash = kwcode_to_strhash(token->code);
                printf("%s (hash: %ul)", kwstr, kwhash);
            }
            else printf("[???]");
    }
}

#endif // RY_LEXER_TOKEN_H