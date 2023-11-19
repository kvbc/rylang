#include "Token.hpp"
#include "ry.hpp"

#include <format>
#include <string_view>
#include <string>
#include <assert.h>
#include <cstring>
#include <variant>
#include <iostream>

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
        m_kind(kind),
        m_srcPos(srcPos)
    {}

    // 

    std::optional<Token::NumericKind> Token::GetIntToNumericKind(int v) {
        if(v < int(Code::_First))
            return char(v);
        if(v < int(Code::_Last))
            return Code(v);
        return {};
    }

    std::optional<Token::Kind> Token::GetNumericKindToKind(NumericKind kind) {
        if(auto * codePtr = std::get_if<Code>(&kind))
            return *codePtr;
        if(auto * charPtr = std::get_if<char>(&kind))
            return *charPtr;
        return {};
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
        if(auto lit1 = std::get_if<TokenLiteral>(&kind1))
            if(auto lit2 = std::get_if<TokenLiteral>(&kind2))
                return lit1->GetValue().index() == lit2->GetValue().index();
        if(auto code1 = std::get_if<Code>(&kind1))
            if(auto code2 = std::get_if<Code>(&kind2))
                return *code1 == *code2;
        if(auto ch1 = std::get_if<char>(&kind1))
            if(auto ch2 = std::get_if<char>(&kind2))
                return *ch1 == *ch2;
        return kind1.index() == kind2.index();
    }

    std::optional<Token::NumericKind> Token::GetKindToNumericKind(const Kind& kind) {
        if(auto * codePtr = std::get_if<Code>(&kind))
            return *codePtr;
        if(auto * charPtr = std::get_if<char>(&kind))
            return *charPtr;
        return {};
    }

    int Token::GetNumericKindToInt(const NumericKind& kind) {
        return std::visit(overloaded{
            [](Code code){ return int(code); },
            [](char ch){ return int(ch); }
        }, kind);
    }

    std::optional<std::pair<Token::Kind, std::size_t>> Token::GetCharsToKind(char c1, char c2, char c3) {
        if(c1 == 0)
            return {};

        int len = 1;
        if(c2 != 0) len++;
        if(c3 != 0) len++;

        const char chars[] = {c1, c2, c3, 0};

        for(; len > 0; len--) {
            for(int i = 0; i < CODE_STRINGS_LEN; i++) {
                const char * str = CODE_STRINGS[i];
                Code code = Code( int(Code::_First) + 1 + i );
                if(strlen(str) == len && strcmp(str, chars) == 0)
                    return {{code, len}};
            }
        }

        return {{c1, 1}};
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
            std::size_t len = std::max(strlen(codeStr), str.length());
            if(strncmp(codeStr, str.data(), len) == 0)
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

    // template<>
    // const char * Token::StringifyKindType<TokenName>() {
    //     return "name";
    // }
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

    std::string Token::StringifyNumericKind(NumericKind kind) {
        return std::visit(overloaded{
            [](Code code) {
                return std::string(CODE_STRINGS[ int(code) - (int(Code::_First) + 1) ]);
            },
            [](char c) {
                return std::string(1, c);
            }
        }, kind);
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
                return Token::StringifyNumericKind(code);
            },
            [](char c) {
                return Token::StringifyNumericKind(c);
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

    const std::string * Token::GetName() const {
        if(auto name = std::get_if<TokenName>(&m_kind))
            return name;
        return nullptr;
    }

}