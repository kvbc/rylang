#pragma once

#include <limits.h>
#include <optional>
#include <string_view>
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
            _FIRST = CHAR_MAX,

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

            OP_POWER,
            OP_ADD_EQ, OP_SUB_EQ, OP_MUL_EQ, OP_POWER_EQ, OP_DIV_EQ, OP_FLOORDIV_EQ, OP_MOD_EQ,
            OP_BIT_LSHIFT, OP_BIT_RSHIFT, OP_BIT_OR, OP_BIT_XOR, OP_BIT_AND, OP_BIT_LSHIFT_EQ, OP_BIT_RSHIFT_EQ,
            OP_EQ, OP_UNEQ, OP_LESS_EQ, OP_GREAT_EQ,
            OP_OR, OP_AND,
            OP_2DOT,

            _LAST
        };

        // must be in the same order as the Type enum
        static constexpr const char * TYPE_STRINGS[] = {
            "???",
            "NAME",
            "INT_LIT",
            "FLOAT_LIT",
            "STRING_LIT",
            "CHAR_LIT",

            "???",
            "KW_IF",    "KW_ELIF",     "KW_ELSE",
            "KW_LOOP",  "KW_CONTINUE", "KW_BREAK",
            "KW_FALSE", "KW_TRUE",
            "KW_AS",
            "???",

            "OP_POWER",
            "OP_ADD_EQ", "OP_SUB_EQ", "OP_MUL_EQ", "OP_POWER_EQ", "OP_DIV_EQ", "OP_FLOORDIV_EQ", "OP_MOD_EQ",
            "OP_BIT_LSHIFT", "OP_BIT_RSHIFT", "OP_BIT_OR", "OP_BIT_XOR", "OP_BIT_AND", "OP_BIT_LSHIFT_EQ", "OP_BIT_RSHIFT_EQ",
            "OP_EQ", "OP_UNEQ", "OP_LESS_EQ", "OP_GREAT_EQ",
            "OP_OR", "OP_AND",
            "OP_2DOT",
            "???"
        };
        static constexpr std::size_t TYPE_STRINGS_LEN = sizeof(TYPE_STRINGS) / sizeof(*TYPE_STRINGS);

        Token(char c);
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

        bool Is(char c) const;
        std::string Stringify() const;

    private:
        union Data {
            std::string_view string;
            intlit_t intv;
            floatlit_t floatv;
            char charv;
        };

        Type m_type;
        std::optional<Data> m_data;
    };

}