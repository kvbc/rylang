#pragma once

#include "Token.hpp"

#include <string_view>
#include <vector>
#include <cstddef>
#include <optional>

namespace ry {

    class Lexer {
    public:
        static constexpr char CHAR_EOF = 0;

        class Info {
        public:
            enum class Level {
                INFO, WARN, ERROR
            };

            Info(
                Level lvl, std::string_view msg,
                std::size_t startLn, std::size_t startCol
            );
            Info(
                Level lvl, std::string_view msg,
                std::size_t startLn, std::size_t startCol,
                std::size_t endLn, std::size_t endCol
            );

            std::string_view GetMessage() const;
            Level GetLevel() const;
            
            std::string Stringify() const;

        private:
            std::size_t m_startLn;
            std::size_t m_startCol;
            std::size_t m_endLn;
            std::size_t m_endCol;
            std::string m_msg;
            Level m_level;
        };

        Lexer(std::string_view src);

        std::vector<Token> Lex();
        std::string_view GetSource() const;
        std::vector<Info> GetInfos() const;

    private:
        using intlit_t = Token::intlit_t;
        using floatlit_t = Token::floatlit_t;

        std::string_view::const_pointer getSourcePointer();

        std::optional<Token> tryLexNameOrKeyword();
        bool tryLexComment();
        std::optional<Token> tryLexOperator();
        bool trySkipWhitespace();
        std::optional<intlit_t> tryLexInteger();
        std::optional<Token> tryLexNumber();
        std::optional<char> tryLexEscapeSequence(bool escapeNewlines);
        std::optional<Token> tryLexCharLiteral();
        std::optional<Token> tryLexStringLiteral();

        char getChar(int offset = 0) const;
        void eatChar(std::size_t count = 1);

        std::vector<Info> m_infos;
        std::size_t m_col;
        std::size_t m_ln;
        int m_srcIdx;
        std::string_view m_src;
    };

}