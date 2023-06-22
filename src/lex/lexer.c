#include "lexer.h"
#include "token.h"
#include "token_code.h"
#include <stdio.h>
#include "../util/dyn_arr.h"
#include "../out/out.h"

// 
// LexerPos 
// 

static void LexerPos_init (struct ryL_Lexer__Pos * pos) {
    pos->col = 1;
    pos->ln = 1;
}
static void LexerPos_clear (struct ryL_Lexer__Pos * pos) {
    LexerPos_init(pos);
}

// 
// LexerMsg 
// 

enum LexerMsgCode {
    LEX_INFO_UNTERMINATED_STRING,
    LEX_INFO_UNFINISHED_STRING,
    LEX_INFO_UNDEFINED_ESC_SEQ,
    LEX_INFO_ESC_SEQ_OVERFLOW,
    
    LEX_INFO_TRAILING_NUMBER_SEPARATOR,
    LEX_INFO_MALFORMED_NUMBER_LITERAL
};
struct LexerMsg {
    enum LexerMsgCode code;
    struct ryL_Lexer__Pos pos;
    const char * msg;
    bool msg_allocd;
};
static void LexerMsg_free( struct LexerMsg * info ) {
    if( info->msg_allocd )
        free((void*)info->msg);
}
static void LexerMsg_init( struct LexerMsg * info, enum LexerMsgCode code, struct ryL_Lexer__Pos pos, ... ) {
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
            ryL_int_t num = va_arg(args, ryL_int_t);
            va_end(args);

            const char * fmt = "escape sequence is out of bounds: \\\\(%u / 0x%X / 0%o) is not in range (%d, %d / 0x%X / 0%o)";
            int msglen = snprintf(NULL, 0, fmt, num, num, num, RYL_CHAR_MIN, RYL_CHAR_MAX, RYL_CHAR_MAX, RYL_CHAR_MAX);
            RY_ASSERT(msglen > 0);
            char * msg = (char *)RY_MALLOC(msglen + 1);
            snprintf(msg, msglen + 1, fmt, num, num, num, RYL_CHAR_MIN, RYL_CHAR_MAX, RYL_CHAR_MAX, RYL_CHAR_MAX);
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

static void lex_clear_tokens( struct ryL_Lexer * lex ) {
    usize len = ryU_Arr_get_len(&lex->_tokens);
    for( size_t i = 0; i < len; i++ ) {
        struct ryL_Token * tk = (struct ryL_Token *)ryU_Arr_get(&lex->_tokens, i);
        ryL_Token_free(tk);
    }
    ryU_Arr_clear(&lex->_tokens);
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

static bool lex_is_eofofs( struct ryL_Lexer * lex, int offset ) {
    size_t idx = lex->_src_idx + offset;
    return (
        ( idx < 0 ) ||
        ( idx >= ryU_DynArr_get_len(lex->_src) )
    );
}
static bool lex_is_eof( struct ryL_Lexer * lex ) {
    return lex_is_eofofs(lex, 0);
}
static bool lex_is_eofnext( struct ryL_Lexer * lex ) {
    return lex_is_eofofs(lex, 1);
}

// 
// Read
// 

static const u8 * lex_srcptrofs( struct ryL_Lexer * lex, int offset ) {
    if( lex_is_eofofs(lex, offset) )
        return NULL;
    return (const u8 *)ryU_DynArr_get_buf(lex->_src) + lex->_src_idx + offset;
}
static const u8 * lex_srcptr( struct ryL_Lexer * lex ) {
    return lex_srcptrofs(lex, 0);
}

static u8 lex_readofs( struct ryL_Lexer * lex, int offset ) {
    const u8 * src = lex_srcptrofs(lex, offset);
    if( src == NULL )
        return RYL_CHAR_EOF;
    return *src;
}
static u8 lex_read( struct ryL_Lexer * lex ) {
    return lex_readofs(lex, 0);
}
static u8 lex_read_next( struct ryL_Lexer * lex ) {
    return lex_readofs(lex, 1);
}
static u8 lex_read_prev( struct ryL_Lexer * lex ) {
    return lex_readofs(lex, -1);
}

// 
// Next
// 

static void lex_next( struct ryL_Lexer * lex ) {
    lex->_src_idx++;

    char c = lex_read(lex);
    char nc = lex_read_next(lex);
    char pc = lex_read_prev(lex);

    if( !is_nl_char(pc) ) {
        if( is_nl_char(c) ) {
            if( is_nl_char(nc) && (nc != c) )
                lex_next(lex);
            lex->_pos.ln++;
            lex->_pos.col = 1;
        }
        else 
            lex->_pos.col++;
    }
}

// 
// 
// 
// 
// 

static struct ryL_Token lex_name( struct ryL_Lexer * lex ) {
    char c = lex_read(lex);
    RY_ASSERT(is_name_start_char(c)); // starts on name

    const u8 * name = lex_srcptr(lex);

    for( ;; ) {
        char nc = lex_read_next(lex);
        if( !is_name_char(nc) )
            break;
        lex_next(lex);
        if( lex_is_eof(lex) )
            break;
    } // ends on last name char

    usize namelen = lex_srcptr(lex) - name + 1;

    struct ryL_Token tk;
    ryL_Token_init(&tk);
    enum ryL_TokenCode kwcode = ryL_Token_string_to_keyword(name, namelen);
    if( kwcode == TK_NONE ) {
        struct ryU_ArrView * view = (struct ryU_ArrView *)ryU_Arr_next(&lex->_str_views);
        ryU_ArrView_init(view, sizeof(ryL_char_t));
        ryU_ArrView_set(view, name, namelen);

        struct ryU_DynArr * dyn = (struct ryU_DynArr *)ryU_Arr_next(&lex->_dyn_strs);
        ryU_DynArr_init_view(dyn, view);

        ryL_Token_set_string(&tk, TK_NAME, dyn);
    }
    else ryL_Token_set(&tk, kwcode);

    return tk;
}

static ryL_int_t lex_number__base(
    struct ryL_Lexer * lex,
    size_t base,
    bool (* is_valid_char) (char c),
    uint8_t (* char_to_value) (char c)
) {
    char c = lex_read(lex);
    ryL_int_t num = 0;

    for( bool first = true ;; first = false ) {
        if( c == RYL_NUMLIT_SEP_CHAR ) {
            lex_next(lex);
            if( lex_is_eof(lex) ) {
                struct LexerMsg info;
                LexerMsg_init(&info, LEX_INFO_TRAILING_NUMBER_SEPARATOR, lex->_pos);
                ryU_Arr_push(&lex->_msgs, &info);
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
static ryL_int_t lex_number( struct ryL_Lexer * lex ) {
    char c1 = lex_read(lex);

    if( c1 == RYL_NUMLIT_SEP_CHAR ) {
        struct LexerMsg info;
        LexerMsg_init(&info, LEX_INFO_TRAILING_NUMBER_SEPARATOR, lex->_pos);
        ryU_Arr_push(&lex->_msgs, &info);
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
                struct LexerMsg info;
                LexerMsg_init(&info, LEX_INFO_MALFORMED_NUMBER_LITERAL, lex->_pos);
                ryU_Arr_push(&lex->_msgs, &info);
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

static struct ryL_Token lex_string( struct ryL_Lexer * lex ) {
    {
        char c = lex_read(lex);
        RY_ASSERT(c == '"');
    }

    lex_next(lex);
    struct ryU_Arr chars;
    ryU_Arr_init(&chars, sizeof(char));

    for( ;; ) {
        char c = lex_read(lex);
    
        if( is_nl_char(c) ) {
            struct LexerMsg info;
            LexerMsg_init(&info, LEX_INFO_UNFINISHED_STRING, lex->_pos);
            ryU_Arr_push(&lex->_msgs, &info);
        }
        else if( c == '\\' ) {
            if( lex_is_eofnext(lex) )
                goto next;
            lex_next(lex);
            char nc = lex_read(lex);

            char esc = RYL_CHAR_EOF;
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
                        ryL_int_t num = lex_number(lex);
                        if( (num < RYL_CHAR_MIN) || (num > RYL_CHAR_MAX) ) {
                            struct LexerMsg info;
                            LexerMsg_init(&info, LEX_INFO_ESC_SEQ_OVERFLOW, lex->_pos, num);
                            ryU_Arr_push(&lex->_msgs, &info);
                            break;
                        }
                        esc = num;
                    } else {
                        struct LexerMsg info;
                        LexerMsg_init(&info, LEX_INFO_UNDEFINED_ESC_SEQ, lex->_pos);
                        ryU_Arr_push(&lex->_msgs, &info);
                    }
                    break;
            }

            if( esc != RYL_CHAR_EOF )
                ryU_Arr_push(&chars, &esc);
        }
        else if( c == '"' ) {
            break;
        }
        else {
            ryU_Arr_push(&chars, &c);
        }

    next:

        if( lex_is_eofnext(lex) ) {
            struct LexerMsg info;
            LexerMsg_init(&info, LEX_INFO_UNTERMINATED_STRING, lex->_pos);
            ryU_Arr_push(&lex->_msgs, &info);
            break;
        }
        lex_next(lex);
    }
    // ends on closing "
    RY_ASSERT(!lex_is_eof(lex));

    struct ryL_Token tk;
    ryL_Token_init(&tk);

    struct ryU_Arr * str = (struct ryU_Arr *)ryU_Arr_next(&lex->_strs);
    ryU_Arr_init(str, sizeof(u8));
    ryU_Arr_copy_arr(str, &chars);

    struct ryU_DynArr * dynstr = (struct ryU_DynArr *)ryU_Arr_next(&lex->_dyn_strs);
    ryU_DynArr_init_arr(dynstr, str);

    ryL_Token_set_string(&tk, TK_STRING, dynstr);

    ryU_Arr_free(&chars);

    return tk;
}

// 
// 
// 
// 
// 

void ryL_Lexer_init( struct ryL_Lexer * out_lex, struct ryU_DynArr * id, struct ryU_DynArr * src ) {
    RY_ASSERT(out_lex != NULL);
    out_lex->_id = id;
    out_lex->_src = src;
    out_lex->_src_idx = 0;
    ryU_Arr_init(&out_lex->_msgs, sizeof(struct LexerMsg));
    ryU_Arr_init(&out_lex->_tokens, sizeof(struct ryL_Token));
    ryU_Arr_init(&out_lex->_str_views, sizeof(struct ryU_ArrView));
    ryU_Arr_init(&out_lex->_dyn_strs, sizeof(struct ryU_DynArr));
    ryU_Arr_init(&out_lex->_strs, sizeof(struct ryU_Arr));
    LexerPos_init(&out_lex->_pos);
}

void ryL_Lexer_free( struct ryL_Lexer * lex ) {
    usize infos_len = ryU_Arr_get_len(&lex->_msgs);
    for( size_t i = 0; i < infos_len; i++ ) {
        struct LexerMsg * info = (struct LexerMsg *)ryU_Arr_get(&lex->_msgs, i);
        LexerMsg_free(info);
    }
    lex_clear_tokens(lex);
    ryU_Arr_free(&lex->_msgs);
    ryU_Arr_free(&lex->_tokens);
    ryU_DynArr_free(lex->_id);
    ryU_DynArr_free(lex->_src);
    ryU_Arr_free(&lex->_str_views);
    ryU_Arr_free(&lex->_dyn_strs);
    ryU_Arr_free(&lex->_strs);
}

void ryL_Lexer_lex( struct ryL_Lexer * lex ) {
    ryU_Arr_clear(&lex->_msgs);
    lex_clear_tokens(lex);
    LexerPos_clear(&lex->_pos);
    lex->_src_idx = 0;

    for( ;; ) {
        char c = lex_read(lex);
        char nc = lex_read_next(lex);

        struct ryL_Token tk;
        ryL_Token_init(&tk);

        switch( c ) {
            #define TK(tkcode) (ryL_Token_set(&tk, tkcode))
            #define TKNEXT(nextc, tk) { \
                    if( nextc == nc ) { \
                        TK(tk); \
                        lex_next(lex); \
                        break; \
                    } \
                }
            #define CASE(val, stuff) case val: stuff; goto default_lbl;

            case ' ':
            case '\v':
            case '\f':
            case '\t':
                break;

            CASE('+', { TKNEXT('=', TK_OP_SET_ADD);  })
            CASE('-', { TKNEXT('=', TK_OP_SET_SUB);  })
            CASE('/', { TKNEXT('=', TK_OP_SET_DIV);  })
            CASE('*', { TKNEXT('=', TK_OP_SET_MUL);  })
            CASE('%', { TKNEXT('=', TK_OP_SET_MOD);  })
            CASE('^', { TKNEXT('=', TK_OP_SET_BXOR); })
            CASE('=', { TKNEXT('=', TK_OP_EQ);       })
            CASE('!', { TKNEXT('=', TK_OP_NE);       })
            CASE('&', { 
                TKNEXT('&', TK_OP_AND);
                TKNEXT('=', TK_OP_SET_BOR);
            })
            CASE('|', {
                TKNEXT('|', TK_OP_OR);
                TKNEXT('=', TK_OP_SET_BOR);
            })
            CASE('<', {
                TKNEXT('<', TK_OP_BLSHIFT);
                TKNEXT('=', TK_OP_LE);
            })
            CASE('>', {
                TKNEXT('>', TK_OP_BRSHIFT);
                TKNEXT('=', TK_OP_GE);
            })
            
            default:
            default_lbl:
                if( is_name_start_char(c) ) {
                    tk = lex_name(lex);
                }
                else if( c == '"' ) {
                    tk = lex_string(lex);
                }
                else if( is_char_decimal_digit(c) ) {
                    ryL_Token_set_int(&tk, TK_INT, lex_number(lex));
                }
                else {
                    ryL_Token_set(&tk, ryL_TokenCode_from_char(c));
                }

            #undef TK
            #undef TKNEXT
            #undef CASE
        }

        if( ryL_Token_get_code(&tk) != TK_NONE )
            ryU_Arr_push(&lex->_tokens, &tk);

        lex_next(lex);
        if( lex_is_eof(lex) )
            break;
    }
}

// 
// Debug
// 
#ifdef RY_DEBUG

void ryL_Lexer_msgs_to_string( struct ryL_Lexer * lex, struct ryU_Arr * out_str ) {
    usize msgs_len = ryU_Arr_get_len(&lex->_msgs);
    for( usize i = 0; i < msgs_len; i++ ) {
        const struct LexerMsg * msg = (const struct LexerMsg *)ryU_Arr_get(&lex->_msgs, i);
        printf("[%.*s:%u:%u] %s\n",
            ryU_DynArr_get_len(lex->_id), ryU_DynArr_get_buf(lex->_id),
            msg->pos.ln,
            msg->pos.col,
            msg->msg
        );
    }
}

void ryL_Lexer_tokens_to_string( struct ryL_Lexer * lex, struct ryU_Arr * out_str ) {
    usize tokens_len = ryU_Arr_get_len(&lex->_tokens);
    for( size_t i = 0; i < tokens_len; i++ ) {
        struct ryL_Token * tk = (struct ryL_Token *)ryU_Arr_get(&lex->_tokens, i);

        bool is_view;
        struct ryU_ArrView strview;
        ryU_ArrView_init(&strview, sizeof(u8));
        struct ryU_Arr str;
        ryU_Arr_init(&str, sizeof(u8));
        ryL_Token_to_string(tk, &strview, &str, &is_view);
        
        if( is_view )
            ryO_print_strview(&strview);
        else
            ryO_print_str(&str);
        ryO_print_char('\n');
    }
}

#endif