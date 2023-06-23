#include "lexer.h"
#include "token.h"
#include "token_code.h"
#include <stdio.h>
#include "../util/dyn_arr.h"
#include "../out/out.h"

// 
// [Static]
// 
#pragma region

// 
// [Static] LexerPos 
// 

static void LexerPos_init (struct ryL_Lexer__Pos * pos) {
    RY_ASSERT(pos);
    pos->col = 1;
    pos->ln = 1;
}
static void LexerPos_clear (struct ryL_Lexer__Pos * pos) {
    RY_ASSERT(pos);
    pos->col = 1;
    pos->ln = 1;
}

// 
// [Static] LexerMsg 
// 
#pragma region

enum LexerMsgCode {
    MSG_UNTERMINATED_STRING,
    MSG_UNFINISHED_STRING,
    MSG_UNDEFINED_ESC_EQ,
    MSG_ESC_SEQ_OVERFLOW,
    
    MSG_TRAILING_NUMBER_SEPARATOR,
    MSG_MALFORMED_NUMBER_LITERAL,

    MSG__LAST
};

#define MSG__COUNT (MSG__LAST - 1)

struct LexerMsg {
    enum LexerMsgCode code;
    struct ryL_Lexer__Pos pos;
    struct ryU_DynArr * str;
};

static void LexerMsg_init( struct LexerMsg * msg, struct ryL_Strings * strings, enum LexerMsgCode code, struct ryL_Lexer__Pos pos, ... ) {
    RY_ASSERT(msg);
    RY_ASSERT(strings);

    msg->code = code;
    msg->pos = pos;
    msg->str = NULL;

    const char * lit = NULL;
    struct ryU_Arr * str = NULL;

    switch( code ) {
        case MSG_UNTERMINATED_STRING:       lit = "unterminated string literal"; break;
        case MSG_UNFINISHED_STRING:         lit = "unfinished string literal";   break;
        case MSG_UNDEFINED_ESC_EQ:          lit = "undefined escape sequence";   break;
        case MSG_TRAILING_NUMBER_SEPARATOR: lit = "trailing number separator";   break;
        case MSG_MALFORMED_NUMBER_LITERAL:  lit = "malformed number literal";    break;
        case MSG_ESC_SEQ_OVERFLOW: {
            va_list args;
            va_start(args, pos);

            RY_ASSERT(str == NULL);
            str = ryL_Strings_get_next_str(strings);
            ryU_str_vformat(str,
                ryU_cstr("escape sequence is out of bounds: \\\\(%u / 0x%X / 0%o) is not in range (%d, %d / 0x%X / 0%o)"),
                args
            );

            va_end(args);
            break;
        }

        case MSG__LAST: RY_ASSERT(false); break;
    }

    struct ryU_DynArr * dyn = NULL;
    if( lit == NULL ) {
        RY_ASSERT(str);
        dyn = ryL_Strings_get_next_dyn(strings);
        ryU_DynArr_init_arr(dyn, str);
    } else { // lit
        RY_ASSERT(str == NULL);
        static struct ryU_DynArr * lit_dyn_cache[MSG__COUNT] = {NULL};
        dyn = lit_dyn_cache[code];
        if( dyn == NULL ) {
            struct ryU_ArrView * view = ryL_Strings_get_next_view(strings);
            ryU_ArrView_init(view, sizeof(u8));
            ryU_ArrView_set(view, ryU_cstr(lit), strlen(lit));

            dyn = ryL_Strings_get_next_dyn(strings);
            ryU_DynArr_init_view(dyn, view);
            lit_dyn_cache[code] = dyn;
        }
    }
    
    RY_ASSERT(dyn);
    RY_ASSERT(msg->str == NULL);
    msg->str = dyn;
}

#pragma endregion
// 
// [Static] char
// 
#pragma region

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

#pragma endregion

// 
// [Static] Lexer : eof
// 

static bool Lexer_is_eofofs( const struct ryL_Lexer * lex, int offset ) {
    RY_ASSERT(lex);
    int idx = lex->_src_idx + offset;
    return (
        ( idx < 0 ) ||
        ( idx >= ryU_DynArr_get_len(lex->_src) )
    );
}
static bool Lexer_is_eof( const struct ryL_Lexer * lex ) {
    return Lexer_is_eofofs(lex, 0);
}
static bool Lexer_is_eofnext( const struct ryL_Lexer * lex ) {
    return Lexer_is_eofofs(lex, 1);
}

// 
// [Static] Lexer : read
// 

static const u8 * Lexer_srcptrofs( const struct ryL_Lexer * lex, int offset ) {
    if( Lexer_is_eofofs(lex, offset) )
        return NULL;
    return (const u8 *)ryU_DynArr_get_buf(lex->_src) + lex->_src_idx + offset;
}
static const u8 * Lexer_srcptr( const struct ryL_Lexer * lex ) {
    return Lexer_srcptrofs(lex, 0);
}

static u8 Lexer_readofs( const struct ryL_Lexer * lex, int offset ) {
    const u8 * src = Lexer_srcptrofs(lex, offset);
    if( src == NULL )
        return RYL_CHAR_EOF;
    return *src;
}
static u8 Lexer_read( const struct ryL_Lexer * lex ) {
    return Lexer_readofs(lex, 0);
}
static u8 Lexer_read_next( const struct ryL_Lexer * lex ) {
    return Lexer_readofs(lex, 1);
}
static u8 Lexer_read_prev( const struct ryL_Lexer * lex ) {
    return Lexer_readofs(lex, -1);
}

// 
// [Static] Lexer : advance
// 

static void Lexer_advance( struct ryL_Lexer * lex ) {
    lex->_src_idx++;

    char c = Lexer_read(lex);
    char nc = Lexer_read_next(lex);
    char pc = Lexer_read_prev(lex);

    if( !is_nl_char(pc) ) {
        if( is_nl_char(c) ) {
            if( is_nl_char(nc) && (nc != c) )
                Lexer_advance(lex);
            lex->_pos.ln++;
            lex->_pos.col = 1;
        }
        else 
            lex->_pos.col++;
    }
}

#pragma endregion // [Static]
// 
// Lexer : name
// 

static void lex_name( struct ryL_Lexer * lex, struct ryL_Token * out_tk ) {
    char c = Lexer_read(lex);
    RY_ASSERT(is_name_start_char(c)); // starts on name

    const u8 * name = Lexer_srcptr(lex);

    for( ;; ) {
        char nc = Lexer_read_next(lex);
        if( !is_name_char(nc) )
            break;
        Lexer_advance(lex);
        if( Lexer_is_eof(lex) )
            break;
    } // ends on last name char

    usize namelen = Lexer_srcptr(lex) - name + 1;

    struct ryU_ArrView * name_view = ryL_Strings_get_next_view(&lex->_strings);
    ryU_ArrView_init(name_view, sizeof(ryL_char_t));
    ryU_ArrView_set(name_view, name, namelen);
    struct ryU_DynArr * name_dyn = ryL_Strings_get_next_dyn(&lex->_strings);
    ryU_DynArr_init_view(name_dyn, name_view);
    bool pop = true;

    enum ryL_TokenCode kwcode = ryL_Token_string_to_keyword(name_dyn, &lex->_strings);
    if( kwcode == TK_NONE ) {
        const struct ryU_DynArr * tk_dyn = name_dyn;
        const struct ryU_DynArr * cached_dyn = ryL_Strings_get_dyn(
            &lex->_strings,
            ryL_Strings_get_hash(&lex->_strings, name_dyn)
        );
        if( cached_dyn == NULL ) {
            pop = false; // use name_dyn for tk_dyn
        }
        else tk_dyn = cached_dyn;
        ryL_Token_set_string(out_tk, TK_NAME, tk_dyn);
    }
    else ryL_Token_set(out_tk, kwcode);

    if( pop ) {
        ryL_Strings_pop_view(&lex->_strings);
        ryL_Strings_pop_dyn(&lex->_strings);
    }
}

// 
// Lexer : number
// 

static ryL_int_t lex_number__base(
    struct ryL_Lexer * lex,
    size_t base,
    bool (* is_valid_char) (char c),
    uint8_t (* char_to_value) (char c)
) {
    char c = Lexer_read(lex);
    ryL_int_t num = 0;

    for( bool first = true ;; first = false ) {
        if( c == RYL_NUMLIT_SEP_CHAR ) {
            Lexer_advance(lex);
            if( Lexer_is_eof(lex) ) {
                struct LexerMsg info;
                LexerMsg_init(&info, &lex->_strings, MSG_TRAILING_NUMBER_SEPARATOR, lex->_pos);
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
            if( Lexer_is_eofnext(lex) )
                break;
            Lexer_advance(lex);
        }

        c = Lexer_read_next(lex);
    }

    return num;
}
static ryL_int_t lex_number( struct ryL_Lexer * lex ) {
    char c1 = Lexer_read(lex);

    if( c1 == RYL_NUMLIT_SEP_CHAR ) {
        struct LexerMsg info;
        LexerMsg_init(&info, &lex->_strings, MSG_TRAILING_NUMBER_SEPARATOR, lex->_pos);
        ryU_Arr_push(&lex->_msgs, &info);
    }
    else RY_ASSERT(is_char_decimal_digit(c1));

    if( c1 == '0' ) {
        Lexer_advance(lex);
        if( Lexer_is_eof(lex) )
            return 0;

        char c2 = Lexer_read(lex);
        if( (c2 == 'x') || (c2 == 'b') || (c2 == 'o') ) {
            Lexer_advance(lex);
            if( Lexer_is_eof(lex) ) {
                struct LexerMsg info;
                LexerMsg_init(&info, &lex->_strings, MSG_MALFORMED_NUMBER_LITERAL, lex->_pos);
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
        char c = Lexer_read(lex);
        RY_ASSERT(c == '"');
    }

    Lexer_advance(lex);
    struct ryU_Arr chars;
    ryU_Arr_init(&chars, sizeof(char));

    for( ;; ) {
        char c = Lexer_read(lex);
    
        if( is_nl_char(c) ) {
            struct LexerMsg info;
            LexerMsg_init(&info, &lex->_strings, MSG_UNFINISHED_STRING, lex->_pos);
            ryU_Arr_push(&lex->_msgs, &info);
        }
        else if( c == '\\' ) {
            if( Lexer_is_eofnext(lex) )
                goto next;
            Lexer_advance(lex);
            char nc = Lexer_read(lex);

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
                            LexerMsg_init(&info, &lex->_strings, MSG_ESC_SEQ_OVERFLOW, lex->_pos, num);
                            ryU_Arr_push(&lex->_msgs, &info);
                            break;
                        }
                        esc = num;
                    } else {
                        struct LexerMsg info;
                        LexerMsg_init(&info, &lex->_strings, MSG_UNDEFINED_ESC_EQ, lex->_pos);
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

        if( Lexer_is_eofnext(lex) ) {
            struct LexerMsg info;
            LexerMsg_init(&info, &lex->_strings, MSG_UNTERMINATED_STRING, lex->_pos);
            ryU_Arr_push(&lex->_msgs, &info);
            break;
        }
        Lexer_advance(lex);
    }
    // ends on closing "
    RY_ASSERT(!Lexer_is_eof(lex));

    struct ryL_Token tk;
    ryL_Token_init(&tk);

    struct ryU_Arr * str = ryL_Strings_get_next_str(&lex->_strings);
    ryU_Arr_init(str, sizeof(u8));
    ryU_Arr_copy_arr(str, &chars);

    struct ryU_DynArr * dynstr = ryL_Strings_get_next_dyn(&lex->_strings);
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
    ryL_Strings_init(&out_lex->_strings);
    LexerPos_init(&out_lex->_pos);
}

void ryL_Lexer_free( struct ryL_Lexer * lex ) {
    ryU_Arr_free(&lex->_msgs);
    ryU_Arr_free(&lex->_tokens);
    ryU_DynArr_free(lex->_id);
    ryU_DynArr_free(lex->_src);
    ryL_Strings_free(&lex->_strings);
}

void ryL_Lexer_lex( struct ryL_Lexer * lex ) {
    ryU_Arr_clear(&lex->_msgs);
    ryU_Arr_clear(&lex->_tokens);
    LexerPos_clear(&lex->_pos);
    lex->_src_idx = 0;

    for( ;; ) {
        char c = Lexer_read(lex);
        char nc = Lexer_read_next(lex);

        struct ryL_Token tk;
        ryL_Token_init(&tk);

        switch( c ) {
            #define TK(tkcode) (ryL_Token_set(&tk, tkcode))
            #define TKNEXT(nextc, tk) { \
                    if( nextc == nc ) { \
                        TK(tk); \
                        Lexer_advance(lex); \
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
                    lex_name(lex, &tk);
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

        Lexer_advance(lex);
        if( Lexer_is_eof(lex) )
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
        printf("[%.*s:%u:%u] %.*s\n",
            ryU_DynArr_get_len(lex->_id), ryU_DynArr_get_buf(lex->_id),
            msg->pos.ln,
            msg->pos.col,
            ryU_DynArr_get_len(msg->str), ryU_DynArr_get_buf(msg->str)
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