#pragma once

#include "Token.hpp"
#include "Infos.hpp"

#include <string_view>
#include <vector>
#include <cstddef>
#include <optional>
#include <set>

namespace ry {

    class Lexer {
    public:
        static constexpr char CHAR_EOF = 0;

        Lexer(std::string_view id, std::string_view src);

        std::vector<Token> Lex();
        std::string_view GetSource() const;
        const Infos& GetInfos() const;

    private:
        using IntLit = TokenLiteral::Int;
        using FloatLit = TokenLiteral::Float;

        template<typename ...Args>
        Token createToken(Args&&... args) const;

        std::string_view::const_pointer getSourcePointer(int offset = 0);

        std::optional<Token> tryLexNameOrKeyword();
        bool tryLexComment();
        bool trySkipWhitespace();
        std::optional<IntLit> tryLexInteger(std::string_view allowedSuffixChars = "");
        std::optional<Token> tryLexNumber();
        std::optional<char> tryLexEscapeSequence(bool escapeNewlines);
        std::optional<Token> tryLexCharLiteral();
        std::optional<Token> tryLexStringLiteral();

        char getChar(int offset = 0) const;
        void eatChar(std::size_t count = 1);

        Infos m_infos;
        std::size_t m_col;
        std::size_t m_ln;
        int m_srcIdx;
        std::string_view m_id;
        std::string_view m_src;
    };

}