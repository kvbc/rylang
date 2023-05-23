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

#define CHAR_EOF (-1)

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
    LEX_INFO_UNFINISHED_STRING,

    LEX_INFO_UNDEFINED_ESC_SEQ,
    LEX_INFO_UNTERMINATED_ESC_SEQ
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

        case LEX_INFO_UNDEFINED_ESC_SEQ: return "undefined escape sequence";
        case LEX_INFO_UNTERMINATED_ESC_SEQ: return "unterminated escape sequence";
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
    const struct ry_StringView * src;

    RY_SIZE_T src_idx;
    struct ry_LexerPos pos;
    
    struct ry_Array tokens; // ry_LexerToken[]
    struct ry_Array infos; // ry_LexerInfo[]
};

static void lex_clear_tokens( struct ry_Lexer * lex ) {
    for( RY_SIZE_T i = 0; i < lex->tokens.len; i++ ) {
        struct ry_LexerToken * tk = ry_Array_get(&lex->tokens, i);
        ry_LexerToken_free(tk);
    }
    ry_Array_clear(&lex->tokens);
}

// 
// Char
// 

static RY_BOOL is_digit_char( char c ) {
    return (c >= '0') && (c <= '9');
}

static RY_BOOL is_name_start_char (char c) {
    return (c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}
static RY_BOOL is_name_char (char c) {
    return is_name_start_char(c) || is_digit_char(c);
}

static RY_BOOL is_nl_char( char c ) {
    return (c == '\n') || (c == '\r');
}

// 
// EOF
// 

static RY_BOOL lex_is_eofofs( struct ry_Lexer * lex, int offset ) {
    RY_SIZE_T idx = lex->src_idx + offset;
    return (
        ( idx < 0 ) ||
        ( idx >= lex->src->len )
    );
}
static RY_BOOL lex_is_eof( struct ry_Lexer * lex ) {
    return lex_is_eofofs(lex, 0);
}

// 
// Read
// 

static const char * lex_srcptrofs( struct ry_Lexer * lex, int offset ) {
    if( lex_is_eofofs(lex, offset) )
        return NULL;
    return lex->src->buf + lex->src_idx + offset;
}
static const char * lex_srcptr( struct ry_Lexer * lex ) {
    return lex_srcptrofs(lex, 0);
}

static char lex_readofs( struct ry_Lexer * lex, int offset ) {
    const char * src = lex_srcptrofs(lex, offset);
    if( src == NULL )
        return CHAR_EOF;
    return *src;
}
static char lex_read( struct ry_Lexer * lex ) {
    return lex_readofs(lex, 0);
}
static char lex_read_next( struct ry_Lexer * lex ) {
    return lex_readofs(lex, 1);
}
static char lex_read_prev( struct ry_Lexer * lex ) {
    return lex_readofs(lex, -1);
}

// 
// Next
// 

static void lex_next( struct ry_Lexer * lex ) {
    char c = lex_read(lex);
    char nc = lex_read_next(lex);
    char pc = lex_read_prev(lex);

    lex->src_idx++;

    if( !is_nl_char(pc) ) {
        if( is_nl_char(c) ) {
            if( is_nl_char(nc) && (nc != c) )
                lex_next(lex);
            lex->pos.ln++;
            lex->pos.col = 1;
        }
        else 
            lex->pos.col++;
    }
}

// 
// 
// 
// 
// 

static struct ry_LexerToken lex_name( struct ry_Lexer * lex ) {
    char c = lex_read(lex);
    assert(is_name_start_char(c)); // starts on name

    const char * name = lex_srcptr(lex);

    for( ;; ) {
        char nc = lex_read_next(lex);
        if( !is_name_char(nc) )
            break;
        lex_next(lex);
        if( lex_is_eof(lex) )
            break;
    } // ends on last name char

    RY_SIZE_T namelen = lex_srcptr(lex) - name + 1;

    struct ry_LexerToken tk;
    enum ry_LexerTokenCode kwcode = ry_LexerToken_string_to_keyword(name, namelen);
    if( kwcode == TK_NONE ) {
        tk.code = TK_NAME;
        struct ry_StringView view = ry_StringView_new(name, namelen);
        ry_String_fromview(&tk.value.str, &view);
    }
    else tk.code = kwcode;

    return tk;
}

static struct ry_LexerToken lex_string( struct ry_Lexer * lex ) {
    assert(lex_read(lex) == '"');

    lex_next(lex);
    const char * str = lex_srcptr(lex);

    for( ;; ) {
        char c = lex_read(lex);
        lex_next(lex);

        if( lex_is_eof(lex) ) {
            struct ry_LexerInfo info;
            ry__LexerInfo_init(&info, LEX_INFO_UNTERMINATED_STRING, lex->pos);
            ry_Array_push(&lex->infos, &info);
            break;
        }

        if( is_nl_char(c) ) {
            struct ry_LexerInfo info;
            ry__LexerInfo_init(&info, LEX_INFO_UNFINISHED_STRING, lex->pos);
            ry_Array_push(&lex->infos, &info);
        }

        if( c == '\\' ) {
            char esc = -1;
            switch( c ) {
                case 'a': esc = '\a'; break;
                case 'b': esc = '\b'; break;
                case 'f': esc = '\f'; break;
                case 'n': esc = '\n'; break;
                case 'r': esc = '\r'; break;
                case 't': esc = '\t'; break;
                case 'v': esc = '\v'; break;
                case '"': esc = '\"'; break;
                case '\\': esc = '\\'; break;
                default:
                    if( is_digit_char(c) ) {
                        // read
                    } else {
                        struct ry_LexerInfo info;
                        ry__LexerInfo_init(&info, LEX_INFO_UNDEFINED_ESC_SEQ, lex->pos);
                        ry_Array_push(&lex->infos, &info);
                    }
                    break;
            }
        }
        else if( c == '"' ) {
            break;
        }
    } // ends on closing "

    RY_SIZE_T str_len = lex_srcptr(lex) - str + 1;

    struct ry_LexerToken tk;
    tk.code = TK_STRING;
    struct ry_StringView view = ry_StringView_new(str, str_len);
    ry_String_fromview(&tk.value.str, &view);

    return tk;
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
    lex->src = src;
    lex->src_idx = 0;
    ry_Array_init(&lex->infos, sizeof(struct ry_LexerInfo));
    ry_Array_init(&lex->tokens, sizeof(struct ry_LexerToken));
    ry__LexerPos_init(&lex->pos);
}

void ry_Lexer_free (struct ry_Lexer * lex) {
    lex_clear_tokens(lex);
    ry_Array_free(&lex->infos);
    ry_Array_free(&lex->tokens);
    ry_String_free(&lex->id);
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
    lex->src_idx = 0;

    for( ;; ) {
        printf(">>> %s\n", lex_srcptr(lex));

        char c = lex_read(lex);
        char nc = lex_read_next(lex);

        struct ry_LexerToken tk;
        tk.code = TK_NONE;

        switch( c ) {
            #define TK(tkcode) (tk.code = tkcode)
            #define TKNEXT(nextc, tk) { \
                    if( nextc == nc ) { \
                        TK(tk); \
                        lex_next(lex); \
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
                    tk = lex_name(lex);
                }
                else if( c == '"' ) {
                    tk = lex_string(lex);
                }

            #undef TK
            #undef TKSET
            #undef CASE
        }

        if( tk.code != TK_NONE )
            ry_Array_push(&lex->tokens, &tk);

        printf("1\n");
        lex_next(lex);
        printf("2\n");
        if( lex_is_eof(lex) )
            break;
        printf("3\n");
    }
}

#endif // RY_LEXER_H