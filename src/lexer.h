#ifndef RY_LEXER_H
#define RY_LEXER_H

#include "lexer_token.h"

#include "str.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 
// 
// 
// 
// 

struct ry_LexerPos {
    RY_SIZE_T ln;
    RY_SIZE_T col;
};
static void ry__LexerPos_init (struct ry_LexerPos * pos) {
    pos->col = 1;
    pos->ln = 1;
}
static void ry__LexerPos_clear (struct ry_LexerPos * pos) {
    ry__LexerPos_init(pos);
}

// 
// 
// 
// 
// 

enum ry_LexerInfoCode {
    LEX_INFO_UNTERMINATED_STRING,
    LEX_INFO_UNFINISHED_STRING
};
struct ry_LexerInfo {
    enum ry_LexerInfoCode code;
    struct ry_LexerPos pos;
};
static void ry__LexerInfo_init( struct ry_LexerInfo * info, enum ry_LexerInfoCode code, struct ry_LexerPos pos ) {
    info->code = code;
    info->pos = pos;
}
static const char * ry__LexerInfo_to_string( const struct ry_LexerInfo * info ) {
    switch( info->code ) {
        case LEX_INFO_UNTERMINATED_STRING: return "unterminated string literal";
        case LEX_INFO_UNFINISHED_STRING:   return "unfinished string literal";
    }
    return "[???]";
}

// 
// 
// 
// 
// 

struct ry_Lexer {
    struct ry_String id;
    struct ry_String src;

    struct ry_LexerPos pos;
    
    struct ry_Array tokens; // ry_LexerToken[]
    struct ry_Array infos; // ry_LexerInfo[]
};

// 
// 
// 
// 
// 

static RY_BOOL is_name_start_char (char c) {
    return (c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}
static RY_BOOL is_name_char (char c) {
    return is_name_start_char(c) || ((c >= '0') && (c <= '9'));
}

static RY_BOOL is_nl_char( char c ) {
    return (c == '\n') || (c == '\r');
}

// 
// 
// 

static void lex_clear_tokens( struct ry_Lexer * lex ) {
    for( RY_SIZE_T i = 0; i < lex->tokens.len; i++ ) {
        struct ry_LexerToken * tk = ry_Array_get(&lex->tokens, i);
        ry_LexerToken_free(tk);
    }
    ry_Array_clear(&lex->tokens);
}

// 
// 
// 
// 
// 

void ry_Lexer_init (
    struct ry_Lexer * lex,
    const struct ry_StringView * id,
    const struct ry_StringView * src
) {
    RY_ASSERT(lex != NULL);
    ry_String_fromview(&lex->id, id);
    ry_String_fromview(&lex->src, src);
    ry_Array_init(&lex->infos, sizeof(struct ry_LexerInfo));
    ry_Array_init(&lex->tokens, sizeof(struct ry_LexerToken));
    ry__LexerPos_init(&lex->pos);
}

void ry_Lexer_free (struct ry_Lexer * lex) {
    lex_clear_tokens(lex);
    ry_Array_free(&lex->infos);
    ry_Array_free(&lex->tokens);
    ry_String_free(&lex->id);
    ry_String_free(&lex->src);
}

void ry_Lexer_print_infos( struct ry_Lexer * lex ) {
    for( RY_SIZE_T i = 0; i < lex->infos.len; i++ ) {
        const struct ry_LexerInfo * info = ry_Array_get(&lex->infos, i);
        printf("[%.*s:%u:%u] %s\n",
            lex->id.len, lex->id.buf,
            info->pos.ln,
            info->pos.col,
            ry__LexerInfo_to_string(info)
        );
    }
}

void ry_Lexer_run (struct ry_Lexer * lex) {
    ry_Array_clear(&lex->infos);
    lex_clear_tokens(lex);
    ry__LexerPos_clear(&lex->pos);

    enum {
        LEX_ANY,
        LEX_NAME,
        LEX_STR
    } state = LEX_ANY;

    const char * buf = NULL;
    RY_SIZE_T buflen = 0;

    for( RY_SIZE_T i = 0; i < lex->src.len; i++ ) {
        char c = lex->src.buf[i];
        char nc = '\0';
        if (i + 1 < lex->src.len)
            nc = lex->src.buf[i + 1];

        struct ry_LexerToken token = ry_LexerToken_new(TK_NONE);
        #define TK(tk) (token.code = tk)
        
        switch( state ) {
            case LEX_NAME:
                if( is_name_char(c) )
                    buflen++;
                if( !is_name_char(nc) ) {
                    enum ry_LexerTokenCode kwcode = ry_LexerToken_string_to_keyword(buf, buflen);
                    if( kwcode == TK_NONE ) {
                        TK(TK_NAME);
                        struct ry_StringView view = ry_StringView_new(buf, buflen);
                        ry_String_fromview(&token.value.str, &view);
                    }
                    else TK(kwcode);
                    state = LEX_ANY;
                }
                break;

            case LEX_STR:
                if( c == '"' ) {
                    TK(TK_STRING);
                    struct ry_StringView view = ry_StringView_new(buf, buflen);
                    ry_String_fromview(&token.value.str, &view);
                    state = LEX_ANY;
                }
                else buflen++;
                break;

            case LEX_ANY:
                switch( c ) {
                    #define NEXT() do { \
                        i++; \
                        lex->pos.col++; \
                    } while( 0 );
                    #define TKNEXT(nextc, tk) { \
                            if( nextc == nc ) { \
                                TK(tk); \
                                NEXT(); \
                                break; \
                            } \
                        }
                    #define CASE(val, stuff) case val: stuff; break;

                    case ' ':
                    case '\v':
                    case '\f':
                    case '\t':
                        break;
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
                    CASE('!', {
                        TKNEXT('=', TK_NE);
                        TK(TK_EXMARK);
                    })
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
                    CASE('[', {
                        TK(TK_OP_SQ_BRACK);
                    })
                    CASE(']', {
                        TK(TK_CL_SQ_BRACK);
                    })
                    default:
                        if( is_name_start_char(c) ) {
                            buf = lex->src.buf + i;
                            buflen = 1;
                            state = LEX_NAME;
                        }
                        else if( c == '"' ) {
                            buf = lex->src.buf + i + 1;
                            buflen = 0;
                            state = LEX_STR;
                        }

                    #undef NEXT
                    #undef TKSET
                    #undef CASE
                }
        }

        #undef TK

        if( is_nl_char(c) ) {
            if( is_nl_char(nc) && (nc != c) )
                i++;
            lex->pos.ln++;
            lex->pos.col = 1;

            static RY_BOOL warned = RY_FALSE;
            if( !warned )
            if( state == LEX_STR ) {
                struct ry_LexerInfo info;
                ry__LexerInfo_init(&info, LEX_INFO_UNFINISHED_STRING, lex->pos);
                ry_Array_push(&lex->infos, &info);
                warned = RY_TRUE;
            }
        }
        else 
            lex->pos.col++;

        if( token.code != TK_NONE )
            ry_Array_push(&lex->tokens, &token);
    }

    if( state == LEX_STR ) {
        struct ry_LexerInfo info;
        ry__LexerInfo_init(&info, LEX_INFO_UNTERMINATED_STRING, lex->pos);
        ry_Array_push(&lex->infos, &info);
    }
}

#endif // RY_LEXER_H