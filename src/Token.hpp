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
        static constexpr const char * KEYWORDS[] = {
            "if",    "elif",     "else",
            "loop",  "continue", "break",
            "false", "true",
            "as"
        };
        static constexpr std::size_t KEYWORDS_LEN = sizeof(KEYWORDS) / sizeof(*KEYWORDS);

        // keywords must be in same order as in KEYWORDS
        enum class Type {
            NAME,
            INT_LIT,
            FLOAT_LIT,
            STRING_LIT,
            CHAR_LIT,

            _KW_FIRST,
            KW_IF,    KW_ELIF,     KW_ELSE,
            KW_LOOP,  KW_CONTINUE, KW_BREAK,
            KW_FALSE, KW_TRUE,
            KW_AS,
            _KW_LAST,

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
        static constexpr const char * TYPE_STRINGS[] = {
            "NAME",
            "INT_LIT",
            "FLOAT_LIT",
            "STRING_LIT",
            "CHAR_LIT",

            "_KW_FIRST",
            "KW_IF",    "KW_ELIF",     "KW_ELSE",
            "KW_LOOP",  "KW_CONTINUE", "KW_BREAK",
            "KW_FALSE", "KW_TRUE",
            "KW_AS",
            "_KW_LAST",

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