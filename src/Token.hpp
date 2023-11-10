#pragma once

#include <limits.h>
#include <optional>
#include <string_view>
#include <string>
#include <stdint.h>

namespace ry {

    class Token {
    public:
        using intlit_t = __uint128_t;
        using floatlit_t = double;

        // must be in same order as in the Type enum
        static constexpr const char * const KEYWORDS[] = {
            "if",    "elif",     "else",
            "loop",  "continue", "break",
            "false", "true",
            "as"
        };
        static constexpr std::size_t KEYWORDS_LEN = sizeof(KEYWORDS) / sizeof(*KEYWORDS);

        // same order as in Type enum
        static constexpr const char * const PRIMITIVE_TYPES[] = {
            "char",
            "i8", "i16", "i32", "i64", "i128",
            "u8", "u16", "u32", "u64", "u128",
            "f32", "f64",
            "bool"
        };
        static constexpr std::size_t PRIMITIVE_TYPES_LEN = sizeof(PRIMITIVE_TYPES) / sizeof(*PRIMITIVE_TYPES);

        // keywords must be in same order as in KEYWORDS
        enum class Type {
            NAME,
            INT_LIT,
            FLOAT_LIT,
            STRING_LIT,
            CHAR_LIT,

            SEMICOLON,
            LPAREN,
            RPAREN,

            _KW_FIRST,
            KW_IF,    KW_ELIF,     KW_ELSE,
            KW_LOOP,  KW_CONTINUE, KW_BREAK,
            KW_FALSE, KW_TRUE,
            KW_AS,
            _KW_LAST,

            _TP_FIRST,
            TP_CHAR,
            TP_I8, TP_I16, TP_I32, TP_I64, TP_I128,
            TP_U8, TP_U16, TP_U32, TP_U64, TP_U128,
            TP_F32, TP_F64,
            TP_BOOL,
            _TP_LAST,

            OP_SUB,    OP_ADD,    OP_MUL,    OP_DIV,    OP_MOD,    OP_POWER,
            OP_SUB_EQ, OP_ADD_EQ, OP_MUL_EQ, OP_DIV_EQ, OP_MOD_EQ, OP_POWER_EQ,

            OP_BIT_NEG,
            OP_BIT_OR_EQ, OP_BIT_XOR_EQ, OP_BIT_LSHIFT_EQ, OP_BIT_RSHIFT_EQ, OP_BIT_AND_EQ,        
            OP_BIT_OR,    OP_BIT_XOR,    OP_BIT_LSHIFT,    OP_BIT_RSHIFT,    OP_BIT_AND,
            /**/ OP_ADDRESS = OP_BIT_AND, /**/

            OP_EQ,   OP_LESS,    OP_GREAT,
            OP_UNEQ, OP_LESS_EQ, OP_GREAT_EQ,

            OP_NOT, OP_OR, OP_AND,

            OP_TERNARY_TRUE, OP_TERNARY_FALSE,
            // OP_ADDRESS,
            OP_STRUCT_FIELD_ACCESS,
            OP_STRUCT_FIELD_IDX_ACCESS,
            OP_SCOPE_ACCESS,
            OP_ASSIGN,
        };

        // must be in the same order as the Type enum
        static constexpr const char * const TYPE_STRINGS[] = {
            "NAME",
            "INT_LIT",
            "FLOAT_LIT",
            "STRING_LIT",
            "CHAR_LIT",

            "SEMICOLON",
            "LPAREN",
            "RPAREN",

            "_KW_FIRST",
            "KW_IF",    "KW_ELIF",     "KW_ELSE",
            "KW_LOOP",  "KW_CONTINUE", "KW_BREAK",
            "KW_FALSE", "KW_TRUE",
            "KW_AS",
            "_KW_LAST",

            "_TP_FIRST",
            "TP_CHAR",
            "TP_I8", "TP_I16", "TP_I32", "TP_I64", "TP_I128",
            "TP_U8", "TP_U16", "TP_U32", "TP_U64", "TP_U128",
            "TP_F32", "TP_F64",
            "TP_BOOL",
            "_TP_LAST",

            "OP_SUB",    "OP_ADD",    "OP_MUL",    "OP_DIV",    "OP_MOD",    "OP_POWER",
            "OP_SUB_EQ", "OP_ADD_EQ", "OP_MUL_EQ", "OP_DIV_EQ", "OP_MOD_EQ", "OP_POWER_EQ",

            "OP_BIT_NEG",
            "OP_BIT_OR_EQ", "OP_BIT_XOR_EQ", "OP_BIT_LSHIFT_EQ", "OP_BIT_RSHIFT_EQ", "OP_BIT_AND_EQ",        
            "OP_BIT_OR",    "OP_BIT_XOR",    "OP_BIT_LSHIFT",    "OP_BIT_RSHIFT",    "OP_BIT_AND/OP_ADDRESS",

            "OP_EQ",   "OP_LESS",    "OP_GREAT",
            "OP_UNEQ", "OP_LESS_EQ", "OP_GREAT_EQ",

            "OP_NOT", "OP_OR", "OP_AND",

            "OP_TERNARY_TRUE", "OP_TERNARY_FALSE",
            // OP_ADDRESS,
            "OP_STRUCT_FIELD_ACCESS",
            "OP_STRUCT_FIELD_IDX_ACCESS",
            "OP_SCOPE_ACCESS",
            "OP_ASSIGN",
        };
        static constexpr std::size_t TYPE_STRINGS_LEN = sizeof(TYPE_STRINGS) / sizeof(*TYPE_STRINGS);

        Token(Type type);
        Token(Type type, std::string_view value);
        Token(Type type, intlit_t value);
        Token(Type type, floatlit_t value);
        Token(Type type, char value);

        static std::optional<Type> GetStringToKeywordOrType(std::string_view str);

        Type GetType() const;
        std::string_view GetStringValue() const;
        intlit_t   GetIntValue  () const;
        floatlit_t GetFloatValue() const;
        char       GetCharValue () const;

        bool IsPrimitiveType() const;
        std::string Stringify() const;

    private:
        union IntValue {
            intlit_t intv;
            floatlit_t floatv;
            char charv;
        };

        Type m_type;
        std::string m_stringValue;
        std::optional<IntValue> m_intValue;
    };

}