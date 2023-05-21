#ifndef RY_LEXER_H
#define RY_LEXER_H

#include "lexer_token.h"
#include "lexer_info.h"

#include "array.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct ry_Lexer {
    const char * src;
    RY_SIZE_T srclen;
    
    struct ry_Array tokens; // ry_LexerToken[]
    struct ry_Array infos; // ry_LexerInfo[]
};

static RY_BOOL is_name_start_char (char c) {
    return (c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}
static RY_BOOL is_name_char (char c) {
    return is_name_start_char(c) || ((c >= '0') && (c <= '9'));
}

// 
// 
// 
// 
// 

void ry_Lexer_init (
    struct ry_Lexer * lex,
    const char * src,
    RY_SIZE_T srclen
) {
    RY_ASSERT(lex != NULL);
    lex->src = src;
    lex->srclen = srclen;
    ry_Array_init(&lex->infos, sizeof(struct ry_LexerInfo));
    ry_Array_init(&lex->tokens, sizeof(struct ry_LexerToken));
}

void ry_Lexer_free (struct ry_Lexer * lex) {
    ry_Array_free(&lex->infos);
    ry_Array_free(&lex->tokens);
}

void ry_Lexer_run (struct ry_Lexer * lex) {
    ry_Array_clear(&lex->tokens);

    for( RY_SIZE_T i = 0; i < lex->srclen; i++ ) {
        char c = lex->src[i];
        char nc = '\0';
        if (i + 1 < lex->srclen)
            nc = lex->src[i + 1];

        struct ry_LexerToken token = ry_LexerToken_new(TK_NONE);
        #define TK(tk) (token.code = tk)
        #define TKNEXT(nextc, tk) { \
                if( nextc == nc ) { \
                    TK(tk); \
                    i++; \
                    break; \
                } \
            }
        #define CASE(val, stuff) case val: stuff; break;

        switch( c ) {
            CASE('+', {
                TKNEXT('+', TK_INC);
                TKNEXT('=', TK_SET_ADD);
                TK(TK_ADD);
            })
            CASE('-', {
                TKNEXT('-', TK_DEC);
                TKNEXT('=', TK_SET_SUB);
                TK(TK_SUB);
            })
            CASE('/', { TKNEXT('=', TK_SET_DIV);  TK(TK_DIV); })
            CASE('*', { TKNEXT('=', TK_SET_MUL);  TK(TK_MUL); })
            CASE('%', { TKNEXT('=', TK_SET_MOD);  TK(TK_MOD); })
            CASE('^', { TKNEXT('=', TK_SET_BXOR); TK(TK_BXOR); })
            CASE('=', { TKNEXT('=', TK_EQ);       TK(TK_SET); })
            CASE('!', { TKNEXT('=', TK_NE);       TK(TK_NOT); })
            CASE('&', { 
                TKNEXT('&', TK_AND);
                TKNEXT('=', TK_SET_BOR);
                TK(TK_BOR);
            })
            CASE('|', {
                TKNEXT('|', TK_OR);
                TKNEXT('=', TK_SET_BOR);
                TK(TK_BOR);
            })
            CASE('~', {
                TK(TK_BNOT);
            })
            CASE('<', {
                TKNEXT('<', TK_BLS);
                TKNEXT('=', TK_EQ_LE);
                TK(TK_LE);
            })
            CASE('>', {
                TKNEXT('>', TK_BRS);
                TKNEXT('=', TK_EQ_GR);
                TK(TK_GR);
            })
            CASE('?', {
                TK(TK_QM);
            })
            CASE(':', {
                TK(TK_COLON);
            })
            CASE('@', {
                TK(TK_META);
            })
            CASE(';', {
                TK(TK_SEMI);
            })
            CASE('(', {
                TK(TK_OP_RND_BRACK);
            })
            CASE(')', {
                TK(TK_CL_RND_BRACK);
            })
            CASE('{', {
                TK(TK_OP_CRL_BRACK);
            })
            CASE('}', {
                TK(TK_CL_CRL_BRACK);
            })
            default:
                if( is_name_start_char(c) ) {
                    const char * name = lex->src + i;
                    RY_SIZE_T namelen = 1;
                    for(;;) {
                        if( i + 1 >= lex->srclen )
                            break;
                        c = lex->src[++i];
                        if( !is_name_char(c) ) {
                            i--;
                            break;
                        }
                        namelen++;
                    }
                    
                    enum ry_LexerTokenCode kwcode = ry_LexerToken_string_to_keyword(name, namelen);
                    if( kwcode == TK_NONE ) {
                        TK(TK_NAME);
                        token.name = name;
                        token.namelen = namelen;
                    }
                    else TK(kwcode);
                }
        }

        #undef TK
        #undef TKSET
        #undef CASE

        // RY_ASSERT(token != TK_NONE);
        ry_Array_push(&lex->tokens, &token);
    }
}

#endif // RY_LEXER_H