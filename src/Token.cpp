#include "Token.hpp"
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

    const char * Token::Stringify(Token::Type type) {
        int stringsIdx = int(type);
        assert(stringsIdx >= 0
            && stringsIdx < TYPE_STRINGS_LEN);
        return TYPE_STRINGS[stringsIdx];
    }

    std::string Token::Stringify() const {
        const char * cstr = Token::Stringify(m_type);
        if(m_type == Type::STRING_LIT || m_type == Type::NAME)
            return std::string(cstr) + '(' + std::string(GetStringValue()) + ')';
        if(m_type == Type::INT_LIT) {
            std::string str = "";
            intlit_t val = GetIntValue();
            do {
                str = std::string(1 ,'0' + (val % 10)) + str;
                val /= 10;
            } while(val > 0);
            return std::string(cstr) + '(' + str + ')';
        }
        if(m_type == Type::FLOAT_LIT)
            return std::string(cstr) + '(' + std::format("{:.14f}", GetFloatValue()) + ')';
        if(m_type == Type::CHAR_LIT)
            return std::string(cstr) + '(' + std::string(1, GetCharValue()) + ')';
        return cstr;
    }

}