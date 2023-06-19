#ifndef RY_LEXER_H
#define RY_LEXER_H

#include "lexer_token.h"

#include "util/str.h"
#include "util/array.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// 
// 
// 

#define CHAR_EOF (-1)
#define CHAR_NUMLIT_SEP ('_')

// 
// 
// 
// 
// 

struct ry_LexerPos {
    size_t ln;
    size_t col;
};
static void ry_LexerPos_init (struct ry_LexerPos * pos) {
    pos->col = 1;
    pos->ln = 1;
}
static void ry_LexerPos_clear (struct ry_LexerPos * pos) {
    ry_LexerPos_init(pos);
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
    LEX_INFO_ESC_SEQ_OVERFLOW,
    
    LEX_INFO_TRAILING_NUMBER_SEPARATOR,
    LEX_INFO_MALFORMED_NUMBER_LITERAL
};
struct ry_LexerInfo {
    enum ry_LexerInfoCode code;
    struct ry_LexerPos pos;
    const char * msg;
    bool msg_allocd;
};
static void ry_LexerInfo_free( struct ry_LexerInfo * info ) {
    if( info->msg_allocd )
        free( (void*) info->msg);
}
static void ry_LexerInfo_init( struct ry_LexerInfo * info, enum ry_LexerInfoCode code, struct ry_LexerPos pos, ... ) {
    info->code = code;
    info->pos = pos;
    info->msg = "[???]";
    info->msg_allocd = false;
    
    switch( code ) {
        case LEX_INFO_UNTERMINATED_STRING: info->msg = "unterminated string literal"; break;
        case LEX_INFO_UNFINISHED_STRING:   info->msg = "unfinished string literal";   break;
        case LEX_INFO_UNDEFINED_ESC_SEQ:   info->msg = "undefined escape sequence";   break;
        case LEX_INFO_ESC_SEQ_OVERFLOW: {
            va_list args;
            va_start(args, pos);
            ry_numlit_t num = va_arg(args, ry_numlit_t);
            va_end(args);

            const char * fmt = "escape sequence is out of bounds: \\\\(%u / 0x%X / 0%o) is not in range (%d, %d / 0x%X / 0%o)";
            int msglen = snprintf(NULL, 0, fmt, num, num, num, RY_CHAR_MIN, RY_CHAR_MAX, RY_CHAR_MAX, RY_CHAR_MAX);
            RY_ASSERT(msglen > 0);
            char * msg = RY_MALLOC(msglen + 1);
            snprintf(msg, msglen + 1, fmt, num, num, num, RY_CHAR_MIN, RY_CHAR_MAX, RY_CHAR_MAX, RY_CHAR_MAX);
            info->msg = msg;
            info->msg_allocd = true;

            break;
        }
        case LEX_INFO_TRAILING_NUMBER_SEPARATOR: info->msg = "trailing number separator"; break;
        case LEX_INFO_MALFORMED_NUMBER_LITERAL:  info->msg = "malformed number literal";  break;
    }
}

// 
// 
// 
// 
// 

struct ry_Lexer {
    struct ry_String id;
    const struct ry_StringView * src;

    size_t src_idx;
    struct ry_LexerPos pos;
    
    struct ry_Array tokens; // ry_LexerToken[]
    struct ry_Array infos; // ry_LexerInfo[]
};

static void lex_clear_tokens( struct ry_Lexer * lex ) {
    for( size_t i = 0; i < lex->tokens.len; i++ ) {
        struct ry_LexerToken * tk = ry_Array_get(&lex->tokens, i);
        ry_LexerToken_free(tk);
    }
    ry_Array_clear(&lex->tokens);
}

// 
// Char
// 

static bool is_char_decimal_digit( char c ) {
    return (c >= '0') && (c <= '9');
}
static uint8_t char_to_decimal_digit( char c ) {
    return c - '0';
}

static bool is_char_binary_digit( char c ) {
    return (c == '0') || (c == '1');
}
static uint8_t char_to_binary_digit( char c ) {
    RY_ASSERT(is_char_binary_digit(c));
    return c - '0';
}

static bool is_char_hex_digit( char c ) {
    if( is_char_decimal_digit(c) )
        return true;
    c = tolower(c);
    return (c >= 'a') && (c <= 'f');
}
static uint8_t char_to_hex_digit( char c ) {
    RY_ASSERT(is_char_hex_digit(c));
    if( is_char_decimal_digit(c) )
        return char_to_decimal_digit(c);
    return 10 + tolower(c) - 'a';
}

static bool is_char_oct_digit( char c ) {
    return (c >= '0') && (c <= '7');
}
static uint8_t char_to_oct_digit( char c ) {
    RY_ASSERT(is_char_oct_digit(c));
    return c - '0';
}

// 

static bool is_name_start_char (char c) {
    return (c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}
static bool is_name_char (char c) {
    return is_name_start_char(c) || is_char_decimal_digit(c);
}

static bool is_nl_char( char c ) {
    return (c == '\n') || (c == '\r');
}

// 
// EOF
// 

static bool lex_is_eofofs( struct ry_Lexer * lex, int offset ) {
    size_t idx = lex->src_idx + offset;
    return (
        ( idx < 0 ) ||
        ( idx >= lex->src->len )
    );
}
static bool lex_is_eof( struct ry_Lexer * lex ) {
    return lex_is_eofofs(lex, 0);
}
static bool lex_is_eofnext( struct ry_Lexer * lex ) {
    return lex_is_eofofs(lex, 1);
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
    lex->src_idx++;

    char c = lex_read(lex);
    char nc = lex_read_next(lex);
    char pc = lex_read_prev(lex);

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
    RY_ASSERT(is_name_start_char(c)); // starts on name

    const char * name = lex_srcptr(lex);

    for( ;; ) {
        char nc = lex_read_next(lex);
        if( !is_name_char(nc) )
            break;
        lex_next(lex);
        if( lex_is_eof(lex) )
            break;
    } // ends on last name char

    size_t namelen = lex_srcptr(lex) - name + 1;

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

static ry_numlit_t lex_number__base(
    struct ry_Lexer * lex,
    size_t base,
    bool (* is_valid_char) (char c),
    uint8_t (* char_to_value) (char c)
) {
    char c = lex_read(lex);
    ry_numlit_t num = 0;

    for( bool first = true ;; first = false ) {
        if( c == CHAR_NUMLIT_SEP ) {
            lex_next(lex);
            if( lex_is_eof(lex) ) {
                struct ry_LexerInfo info;
                ry_LexerInfo_init(&info, LEX_INFO_TRAILING_NUMBER_SEPARATOR, lex->pos);
                ry_Array_push(&lex->infos, &info);
                break;
            }
            continue;
        }
        else if( is_valid_char(c) ) {
            num *= base;
            num += char_to_value(c);
        }
        else break;

        if( !first ) {
            if( lex_is_eofnext(lex) )
                break;
            lex_next(lex);
        }

        c = lex_read_next(lex);
    }

    return num;
}
static ry_numlit_t lex_number( struct ry_Lexer * lex ) {
    char c1 = lex_read(lex);

    if( c1 == CHAR_NUMLIT_SEP ) {
        struct ry_LexerInfo info;
        ry_LexerInfo_init(&info, LEX_INFO_TRAILING_NUMBER_SEPARATOR, lex->pos);
        ry_Array_push(&lex->infos, &info);
    }
    else RY_ASSERT(is_char_decimal_digit(c1));

    if( c1 == '0' ) {
        lex_next(lex);
        if( lex_is_eof(lex) )
            return 0;

        char c2 = lex_read(lex);
        if( (c2 == 'x') || (c2 == 'b') || (c2 == 'o') ) {
            lex_next(lex);
            if( lex_is_eof(lex) ) {
                struct ry_LexerInfo info;
                ry_LexerInfo_init(&info, LEX_INFO_MALFORMED_NUMBER_LITERAL, lex->pos);
                ry_Array_push(&lex->infos, &info);
                return -1;
            }

            if( c2 == 'x' ) // hex
                return lex_number__base(lex, 16, &is_char_hex_digit, &char_to_hex_digit); 
            else if( c2 == 'b' ) // bin
                return lex_number__base(lex, 2, &is_char_binary_digit, &char_to_binary_digit);            
            else if( c2 == 'o' ) // oct
                return lex_number__base(lex, 8, &is_char_oct_digit, &char_to_oct_digit);
        }
    }
    
    // dec
    return lex_number__base(lex, 10, &is_char_decimal_digit, &char_to_decimal_digit);
}

static struct ry_LexerToken lex_string( struct ry_Lexer * lex ) {
    {
        char c = lex_read(lex);
        RY_ASSERT(c == '"');
    }

    lex_next(lex);
    struct ry_Array chars;
    ry_Array_init(&chars, sizeof(char));

    for( ;; ) {
        char c = lex_read(lex);
    
        if( is_nl_char(c) ) {
            struct ry_LexerInfo info;
            ry_LexerInfo_init(&info, LEX_INFO_UNFINISHED_STRING, lex->pos);
            ry_Array_push(&lex->infos, &info);
        }
        else if( c == '\\' ) {
            if( lex_is_eofnext(lex) )
                goto next;
            lex_next(lex);
            char nc = lex_read(lex);

            char esc = CHAR_EOF;
            switch( nc ) {
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
                    if( is_char_decimal_digit(nc) ) {
                        ry_numlit_t num = lex_number(lex);
                        if( (num < RY_CHAR_MIN) || (num > RY_CHAR_MAX) ) {
                            struct ry_LexerInfo info;
                            ry_LexerInfo_init(&info, LEX_INFO_ESC_SEQ_OVERFLOW, lex->pos, num);
                            ry_Array_push(&lex->infos, &info);
                            break;
                        }
                        esc = num;
                    } else {
                        struct ry_LexerInfo info;
                        ry_LexerInfo_init(&info, LEX_INFO_UNDEFINED_ESC_SEQ, lex->pos);
                        ry_Array_push(&lex->infos, &info);
                    }
                    break;
            }

            if( esc != CHAR_EOF )
                ry_Array_push(&chars, &esc);
        }
        else if( c == '"' ) {
            break;
        }
        else {
            ry_Array_push(&chars, &c);
        }

    next:

        if( lex_is_eofnext(lex) ) {
            struct ry_LexerInfo info;
            ry_LexerInfo_init(&info, LEX_INFO_UNTERMINATED_STRING, lex->pos);
            ry_Array_push(&lex->infos, &info);
            break;
        }
        lex_next(lex);
    }
    // ends on closing "
    RY_ASSERT(!lex_is_eof(lex));

    struct ry_LexerToken tk;
    tk.code = TK_STRING;
    struct ry_StringView view = ry_StringView_new(chars.data, chars.len);
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
    ry_LexerPos_init(&lex->pos);
}

void ry_Lexer_free (struct ry_Lexer * lex) {
    for( size_t i = 0; i < lex->infos.len; i++ ) {
        struct ry_LexerInfo * info = ry_Array_get(&lex->infos, i);
        ry_LexerInfo_free(info);
    }
    lex_clear_tokens(lex);
    ry_Array_free(&lex->infos);
    ry_Array_free(&lex->tokens);
    ry_String_free(&lex->id);
}

void ry_Lexer_print_infos( struct ry_Lexer * lex ) {
    for( size_t i = 0; i < lex->infos.len; i++ ) {
        const struct ry_LexerInfo * info = ry_Array_get(&lex->infos, i);
        printf("[%.*s:%u:%u] %s\n",
            lex->id.len, lex->id.buf,
            info->pos.ln,
            info->pos.col,
            info->msg
        );
    }
}

void ry_Lexer_run (struct ry_Lexer * lex) {
    ry_Array_clear(&lex->infos);
    lex_clear_tokens(lex);
    ry_LexerPos_clear(&lex->pos);
    lex->src_idx = 0;

    for( ;; ) {
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
            CASE('#', {
                TK(TK_HASH);
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
                else if( is_char_decimal_digit(c) ) {
                    tk.code = TK_NUMBER;
                    tk.value.num = lex_number(lex);
                }

            #undef TK
            #undef TKSET
            #undef CASE
        }

        if( tk.code != TK_NONE )
            ry_Array_push(&lex->tokens, &tk);

        lex_next(lex);
        if( lex_is_eof(lex) )
            break;
    }
}

#endif // RY_LEXER_H