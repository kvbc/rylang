#include "Token.hpp"
#include "ry.hpp"

#include <format>
#include <string_view>
#include <string>
#include <assert.h>
#include <cstring>
#include <variant>

namespace ry {                                                      

    /*
     *
     * TokenLiteral
     *
     */

    TokenLiteral::TokenLiteral(const Value& value):
        m_value(value)
    {}

    const TokenLiteral::Value& TokenLiteral::GetValue() const {
        return m_value;
    }

    std::string TokenLiteral::StringifyValue(const Value &value) {
        return std::visit(overloaded{
            [](Int intValue) -> std::string {
                std::string str = "";
                do {
                    str = std::string(1 ,'0' + (intValue % 10)) + str;
                    intValue /= 10;
                } while(intValue > 0);
                return str;
            },
            [](Float floatValue) -> std::string{
                return std::format("{:.14f}", floatValue);
            },
            [](Char charValue) -> std::string{
                return '\'' + std::to_string(charValue) + '\'';
            },
            [](const String& stringValue) -> std::string {
                return '"' + stringValue + '"';
            }
        }, value);
    }

    std::string TokenLiteral::Stringify() const {
        return StringifyValue(m_value);
    }

    /*
     *
     * Token
     *
     */

    Token::Token(const SourcePosition& srcPos, const Kind& kind):
        m_srcPos(srcPos),
        m_kind(kind)
    {}

    // 

    template<typename T>
    bool Token::IsKind(const Kind& kind) {
        return std::holds_alternative<T>(kind);
    }
    template<>
    bool Token::IsKind<TokenIntegerLiteral>(const Kind& kind) {
        if(auto literal = std::get_if<TokenLiteral>(&kind))
            return std::holds_alternative<TokenLiteral::Int>(literal->GetValue());
        return false;
    }
    template<>
    bool Token::IsKind<TokenFloatLiteral>(const Kind& kind) {
        if(auto literal = std::get_if<TokenLiteral>(&kind))
            return std::holds_alternative<TokenLiteral::Float>(literal->GetValue());
        return false;
    }
    template<>
    bool Token::IsKind<TokenStringLiteral>(const Kind& kind) {
        if(auto literal = std::get_if<TokenLiteral>(&kind))
            return std::holds_alternative<TokenLiteral::String>(literal->GetValue());
        return false;
    }
    template<>
    bool Token::IsKind<TokenCharLiteral>(const Kind& kind) {
        if(auto literal = std::get_if<TokenLiteral>(&kind))
            return std::holds_alternative<TokenLiteral::Char>(literal->GetValue());
        return false;
    }

    bool Token::IsKindEqual(const Kind& kind1, const Kind& kind2) {
        return kind1.index() == kind2.index();
    }

    std::optional<Token::NumericKind> Token::GetKindToNumericKind(const Kind& kind) {
        if(auto * codePtr = std::get_if<Code>(&kind))
            return *codePtr;
        if(auto * charPtr = std::get_if<char>(&kind))
            return *charPtr;
        return {};
    }

    int Token::GetNumericKindToInt(NumericKind kind) {
        return std::visit([](auto&&v){ return int(v); }, kind);
    }

    std::optional<std::pair<Token::Kind, std::size_t>> Token::GetCharsToKind(char c1, char c2, char c3) {
        if(c1 == 0)
            return {};

        int len = 1;
        if(c2 != 0) len++;
        if(c3 != 0) len++;

        const char chars[] = {c1, c2, c3};

        for(int i = 0; i < CODE_STRINGS_LEN; i++) {
            const char * str = CODE_STRINGS[i];
            Code code = Code( int(Code::_First) + 1 + i );
            if(strncmp(str, chars, len) == 0)
                return {{code, len}};
        }

        if(len == 3) return GetCharsToKind(c1, c2, 0);
        if(len == 2) return GetCharsToKind(c1, 0, 0);

        return {{c1, len}};
    }

    std::optional<Token::Code> Token::GetStringToKeywordCode(std::string_view str) {
        if(str.empty())
            return {};
        if(!std::isalpha(str.at(0)))
            return {};
        for(std::size_t i = 0; i < CODE_STRINGS_LEN; i++) {
            const char * codeStr = CODE_STRINGS[i];
            if(!std::isalpha(codeStr[0]))
                continue;
            if(strncmp(codeStr, str.data(), str.length()) == 0)
                return Code(int(Code::_First) + 1 + i);
        }
        return {};
    }

    // 

    const SourcePosition& Token::GetSourcePosition() const {
        return m_srcPos;
    }

    const Token::Kind& Token::GetKind() const {
        return m_kind;
    }

    // 

    template<typename T>
    const char * Token::StringifyKindType() {
        return "???";
    }
    template<>
    const char * Token::StringifyKindType<TokenName>() {
        return "name";
    }
    template<>
    const char * Token::StringifyKindType<TokenLiteral>() {
        return "literal";
    }
    template<>
    const char * Token::StringifyKindType<TokenIntegerLiteral>() {
        return "integer literal";
    }
    template<>
    const char * Token::StringifyKindType<TokenFloatLiteral>() {
        return "float literal";
    }
    template<>
    const char * Token::StringifyKindType<TokenStringLiteral>() {
        return "string literal";
    }
    template<>
    const char * Token::StringifyKindType<TokenCharLiteral>() {
        return "character literal";
    }

    std::string Token::StringifyKind(const Token::Kind& kind) {
        return std::visit(overloaded{
            [](const TokenName& name) {
                return std::string(name);
            },
            [](const TokenLiteral& literal) {
                return literal.Stringify();
            },
            [](Code code) {
                return std::string(CODE_STRINGS[ int(code) ]);
            },
            [](char c) {
                return std::to_string(c);
            }
        }, kind);
    }

    std::string Token::Stringify() const {
        return StringifyKind(m_kind);
    }

    //

    bool Token::operator==(char checkChar) const {
        if(auto ch = std::get_if<char>(&m_kind))
            return *ch == checkChar;
        return false;
    }

    bool Token::operator==(Code checkCode) const {
        if(auto code = std::get_if<Code>(&m_kind))
            return *code == checkCode;
        return false;
    }

    // 

    const std::string * Token::GetName() {
        if(auto name = std::get_if<TokenName>(&m_kind))
            return name;
        return nullptr;
    }

    template<typename T>
    const T * Token::GetLiteralValue() {
        if(auto literal = std::get_if<TokenLiteral>(&m_kind))
            if(auto value = std::get_if<T>(&literal->GetValue()))
                return value;
        return nullptr;
    }

}