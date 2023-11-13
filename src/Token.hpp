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
            "if",    "else",
            "loop",  "do",   "continue", "break",
            "false", "true", "null",
            "not", "or", "and",
            "as",

            "char",
            "i8", "i16", "i32", "i64", "i128",
            "u8", "u16", "u32", "u64", "u128",
            "f32", "f64",
            "bool"
        };
        static constexpr std::size_t KEYWORDS_LEN = sizeof(KEYWORDS) / sizeof(*KEYWORDS);

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
            KW_IF,    KW_ELSE,
            KW_LOOP,  KW_DO,   KW_CONTINUE, KW_BREAK,
            KW_FALSE, KW_TRUE, KW_NULL,
            KW_NOT, KW_OR, KW_AND,
            KW_AS,
            // 
            KW_CHAR,
            KW_I8, KW_I16, KW_I32, KW_I64, KW_I128,
            KW_U8, KW_U16, KW_U32, KW_U64, KW_U128,
            KW_F32, KW_F64,
            KW_BOOL,
            _KW_LAST,

            OP_SUB_EQ, OP_ADD_EQ, OP_DIV_EQ, OP_MOD_EQ, OP_POWER_EQ, OP_MUL_EQ, 
            OP_SUB,    OP_ADD,    OP_DIV,    OP_MOD,    OP_POWER,    OP_MUL,
            /**/ OP_PTR_DEREF = OP_MUL, /**/

            OP_BIT_NEG,
            OP_BIT_OR_EQ, OP_BIT_XOR_EQ, OP_BIT_LSHIFT_EQ, OP_BIT_RSHIFT_EQ, OP_BIT_AND_EQ,        
            OP_BIT_OR,    OP_BIT_XOR,    OP_BIT_LSHIFT,    OP_BIT_RSHIFT,    OP_BIT_AND,
            /**/ OP_ADDRESS = OP_BIT_AND, /**/

            OP_EQ,   OP_LESS,    OP_GREAT,
            OP_UNEQ, OP_LESS_EQ, OP_GREAT_EQ,

            OP_NOT, OP_OR, OP_AND,

            // OP_ADDRESS,
            OP_BLOCK_ACCESS,
            OP_ASSIGN,
            OP_DEFINE,
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
            "KW_IF",     "KW_ELSE",
            "KW_LOOP",  "KW_DO",   "KW_CONTINUE", "KW_BREAK",
            "KW_FALSE", "KW_TRUE", "KW_NULL",
            "KW_NOT",   "KW_OR",   "KW_AND",
            "KW_AS",
            // 
            "KW_CHAR",
            "KW_I8", "KW_I16", "KW_I32", "KW_I64", "KW_I128",
            "KW_U8", "KW_U16", "KW_U32", "KW_U64", "KW_U128",
            "KW_F32", "KW_F64",
            "KW_BOOL",
            "_KW_LAST",

            "OP_SUB_EQ", "OP_ADD_EQ", "OP_DIV_EQ", "OP_MOD_EQ", "OP_POWER_EQ", "OP_MUL_EQ", 
            "OP_SUB",    "OP_ADD",    "OP_DIV",    "OP_MOD",    "OP_POWER",    "OP_MUL/OP_PTR_DEREF",    

            "OP_BIT_NEG",
            "OP_BIT_OR_EQ", "OP_BIT_XOR_EQ", "OP_BIT_LSHIFT_EQ", "OP_BIT_RSHIFT_EQ", "OP_BIT_AND_EQ",        
            "OP_BIT_OR",    "OP_BIT_XOR",    "OP_BIT_LSHIFT",    "OP_BIT_RSHIFT",    "OP_BIT_AND/OP_ADDRESS",

            "OP_EQ",   "OP_LESS",    "OP_GREAT",
            "OP_UNEQ", "OP_LESS_EQ", "OP_GREAT_EQ",

            "OP_NOT", "OP_OR", "OP_AND",

            // OP_ADDRESS,
            "OP_BLOCK_ACCESS",
            "OP_ASSIGN",
            "OP_DEFINE"
        };
        static constexpr std::size_t TYPE_STRINGS_LEN = sizeof(TYPE_STRINGS) / sizeof(*TYPE_STRINGS);

        Token(Type type);
        Token(Type type, std::string_view value);
        Token(Type type, intlit_t value);
        Token(Type type, floatlit_t value);
        Token(Type type, char value);

        static std::optional<Type> GetStringToKeywordType(std::string_view str);

        Type GetType() const;
        std::string_view GetStringValue() const;
        intlit_t   GetIntValue  () const;
        floatlit_t GetFloatValue() const;
        char       GetCharValue () const;

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