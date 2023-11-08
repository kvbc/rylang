#include "Token.hpp"
#include <string_view>
#include <string>
#include <assert.h>
#include <cstring>

namespace ry {

    Token::Token(Token::Type type):
        m_type(type)
    {}

    Token::Token(Token::Type type, std::string_view value):
        m_type(type),
        m_data({ .string = value })
    {
        assert(m_type == Token::Type::STRING_LIT
            || m_type == Token::Type::NAME);
    }

    Token::Token(Token::Type type, int value):
        m_type(type),
        m_data({ .intv = value })
    {
        assert(m_type == Token::Type::INT_LIT);
    }

    Token::Token(Token::Type type, float value):
        m_type(type),
        m_data({ .floatv = value })
    {
        assert(m_type == Token::Type::FLOAT_LIT);
    }

    Token::Token(Token::Type type, char value):
        m_type(type),
        m_data({ .charv = value })
    {
        assert(m_type == Token::Type::CHAR_LIT);
    }

    // 

    std::optional<Token::Type> Token::GetStringToKeywordType(std::string_view str) {
        for(std::size_t i = 0; i < KEYWORDS_LEN; i++) {
            const char * keyword = KEYWORDS[i];
            if(strncmp(keyword, str.data(), str.length()) == 0)
                return Type(int(Type::_KW_FIRST) + 1 + i);
        }
        return {};
    }

    // 

    Token::Type Token::GetType() const {
        return m_type;
    }

    std::string_view Token::GetStringValue() const {
        assert(m_type == Token::Type::STRING_LIT
            || m_type == Token::Type::NAME);
        return m_data->string;
    }

    int Token::GetIntValue() const {
        assert(m_type == Token::Type::INT_LIT);
        return m_data->intv;
    }

    float Token::GetFloatValue() const {
        assert(m_type == Token::Type::FLOAT_LIT);
        return m_data->floatv;
    }

    char Token::GetCharValue() const {
        assert(m_type == Token::Type::CHAR_LIT);
        return m_data->charv;
    }

    // 

    bool Token::Is(char c) const {
        return char(m_type) == c;
    }

    std::string_view Token::Stringify() const {
        int stringsIdx = int(m_type) - int(Type::_FIRST);
        assert(stringsIdx >= 0
            && stringsIdx < TYPE_STRINGS_LEN);
        const char * cstr = TYPE_STRINGS[stringsIdx];
        if(m_type == Type::STRING_LIT || m_type == Type::NAME)
            return std::string(cstr) + '(' + std::string(GetStringValue()) + ')';
        if(m_type == Type::INT_LIT)
            return std::string(cstr) + '(' + std::to_string(GetIntValue()) + ')';
        if(m_type == Type::FLOAT_LIT)
            return std::string(cstr) + '(' + std::to_string(GetFloatValue()) + ')';
        if(m_type == Type::CHAR_LIT)
            return std::string(cstr) + '(' + std::to_string(GetCharValue()) + ')';
        return cstr;
    }

}