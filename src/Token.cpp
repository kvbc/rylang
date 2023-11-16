#include "Token.hpp"
#include "ry.hpp"

#include <format>
#include <string_view>
#include <string>
#include <assert.h>
#include <cstring>

namespace ry {                                                      

    Token::Token(const SourcePosition& srcPos, Token::Type type):
        m_type(type),
        m_srcPos(srcPos)
    {}

    Token::Token(const SourcePosition& srcPos, Token::Type type, std::string_view value):
        m_type(type),
        m_stringValue(value),
        m_srcPos(srcPos)
    {
        assert(m_type == Token::Type::STRING_LIT
            || m_type == Token::Type::NAME);
    }

    Token::Token(const SourcePosition& srcPos, Token::Type type, intlit_t value):
        m_type(type),
        m_intValue({ .intv = value }),
        m_srcPos(srcPos)
    {
        assert(m_type == Token::Type::INT_LIT);
    }

    Token::Token(const SourcePosition& srcPos, Token::Type type, floatlit_t value):
        m_type(type),
        m_intValue({ .floatv = value }),
        m_srcPos(srcPos)
    {
        assert(m_type == Token::Type::FLOAT_LIT);
    }

    Token::Token(const SourcePosition& srcPos, Token::Type type, char value):
        m_type(type),
        m_intValue({ .charv = value }),
        m_srcPos(srcPos)
    {
        assert(m_type == Token::Type::CHAR_LIT);
    }

    // 

    std::optional<Token::Type> Token::GetStringToKeywordType(std::string_view str) {
        for(std::size_t i = 0; i < KEYWORDS_LEN; i++)
            if(strncmp(KEYWORDS[i], str.data(), str.length()) == 0)
                return Type(int(Type::_KW_FIRST) + 1 + i);
        return {};
    }

    const SourcePosition& Token::GetSourcePosition() const {
        return m_srcPos;
    }

    // 

    Token::Type Token::GetType() const {
        return m_type;
    }

    std::string_view Token::GetStringValue() const {
        assert(m_type == Token::Type::STRING_LIT
            || m_type == Token::Type::NAME);
        return m_stringValue;
    }

    Token::intlit_t Token::GetIntValue() const {
        assert(m_type == Token::Type::INT_LIT);
        return m_intValue->intv;
    }

    Token::floatlit_t Token::GetFloatValue() const {
        assert(m_type == Token::Type::FLOAT_LIT);
        return m_intValue->floatv;
    }

    char Token::GetCharValue() const {
        assert(m_type == Token::Type::CHAR_LIT);
        return m_intValue->charv;
    }

    // 

    std::string Token::StringifyLiteralValue(const LiteralValue& literalValue) {
        return std::visit(overloaded{
            [](intlit_t intValue) -> std::string {
                std::string str = "";
                do {
                    str = std::string(1 ,'0' + (intValue % 10)) + str;
                    intValue /= 10;
                } while(intValue > 0);
                return str;
            },
            [](floatlit_t floatValue) -> std::string{
                return std::format("{:.14f}", floatValue);
            },
            [](char charValue) -> std::string{
                return '\'' + std::to_string(charValue) + '\'';
            },
            [](const std::string& stringValue) -> std::string {
                return '"' + stringValue + '"';
            },
            [](bool boolValue) -> std::string {
                return std::to_string(boolValue);
            },
            [](NullValue) -> std::string {
                return "null";
            }
        }, literalValue);
    }

    const char * Token::Stringify(Token::Type type) {
        int stringsIdx = int(type);
        assert(stringsIdx >= 0
            && stringsIdx < TYPE_STRINGS_LEN);
        return TYPE_STRINGS[stringsIdx];
    }

    // 

    std::string Token::Stringify() const {
        const char * cstr = Token::Stringify(m_type);
        auto optLitValue = GetLiteralValue();
        if(optLitValue.has_value())
            return std::string(cstr) + '(' + StringifyLiteralValue(optLitValue.value()) + ')';
        return cstr;
    }

    // 

    std::optional<Token::LiteralValue> Token::GetLiteralValue() const {
        if(m_type == Type::STRING_LIT)
            return m_stringValue;
        if(m_type == Type::INT_LIT)
            return m_intValue->intv;
        if(m_type == Type::CHAR_LIT)
            return m_intValue->charv;
        if(m_type == Type::FLOAT_LIT)
            return m_intValue->floatv;
        if(m_type == Type::KW_NULL)
            return NullValue{};
        if(m_type == Type::KW_TRUE)
            return true;
        if(m_type == Type::KW_FALSE)
            return false;
        return {};
    }

}