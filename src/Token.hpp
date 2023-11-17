#pragma once

#include "SourcePosition.hpp"
#include "ParserTokens.hpp"

#include <limits.h>
#include <optional>
#include <string_view>
#include <string>
#include <stdint.h>
#include <variant>

namespace ry {

    using TokenName = std::string;

    class TokenLiteral {
    public:
        using Int = __uint128_t;
        using Float = double;
        using String = std::string;
        using Char = char;
        using Value = std::variant<Int, Float, String, Char>;

        TokenLiteral(const Value& value);

        const Value& GetValue() const;

        static std::string StringifyValue(const Value& value);
        std::string Stringify() const;

    private:
        Value m_value;
    };

    class Token {
    public:
    #define CODES_E_ENUM(TK) TK ,
    #define CODES_E_STR(TK) #TK ,
    #define CODES(E, TOKENS_E) /* E - expand macro */ \
        RY_PARSER_SPECIAL_TOKENS(TOKENS_E) \
        // E(Name) \
        // E(IntegerLiteral) \
        // E(FloatLiteral) \
        // E(StringLiteral) \
        // E(CharLiteral) \

        enum class Code {
            _First = UCHAR_MAX + 1,
            CODES(CODES_E_ENUM, RY_PARSER_SPECIAL_TOKENS_EXPAND_NAMES)
        };
        using NumericKind = std::variant<Code, char>;
        using Kind = std::variant<TokenName, TokenLiteral, Code, char>;

        Token(const SourcePosition& srcPos, const Kind& kind);

        static bool IsKindEqual(const Kind& kind1, const Kind& kind2);
        static std::optional<NumericKind> GetKindToNumericKind(const Kind& kind);
        static int GetNumericKindToInt(NumericKind kind);
        static std::optional<std::pair<Kind, std::size_t>> GetCharsToKind(char c1, char c2, char c3);
        static std::optional<Code> GetStringToKeywordCode(std::string_view str);

        const SourcePosition& GetSourcePosition() const;
        const Kind& GetKind() const;

        static std::string StringifyKind(const Kind& value);
        std::string Stringify() const;

        bool operator==(char c) const;
        bool operator==(Code code) const;

        bool IsName() const;
        bool IsLiteral() const;

    private:
        static constexpr const char * const CODE_STRINGS[] = {
            CODES(CODES_E_STR, RY_PARSER_SPECIAL_TOKENS_EXPAND_VALUES)
        };
        static const std::size_t CODE_STRINGS_LEN = sizeof(CODE_STRINGS) / sizeof(*CODE_STRINGS);

    #undef CODES
    #undef CODES_E_STR
    #undef CODES_E_ENUM

        Kind m_kind;
        SourcePosition m_srcPos;
    };

}