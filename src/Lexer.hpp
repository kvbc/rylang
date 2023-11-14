#pragma once

#include "Token.hpp"

#include <string_view>
#include <vector>
#include <cstddef>
#include <optional>
#include <set>

namespace ry {

    class Lexer {
    public:
        static constexpr char CHAR_EOF = 0;

        using intlit_t = Token::intlit_t;
        using floatlit_t = Token::floatlit_t;

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

            std::size_t GetStartLineNumber() const;
            std::size_t GetStartColumn() const;
            std::size_t GetEndLineNumber() const;
            std::size_t GetEndColumn() const;

        private:
            std::size_t m_startLn;
            std::size_t m_startCol;
            std::size_t m_endLn;
            std::size_t m_endCol;
            std::string m_msg;
            Level m_level;
        };

        Lexer(std::string_view id, std::string_view src);

        std::vector<Token> Lex();
        std::string_view GetSource() const;
        std::vector<Info> GetInfos() const;

        std::string StringifyInfo(const Info& info) const;

    private:
        std::string_view::const_pointer getSourcePointer(int offset = 0);

        std::optional<Token> tryLexNameOrKeyword();
        bool tryLexComment();
        std::optional<Token> tryLexOperator();
        bool trySkipWhitespace();
        std::optional<intlit_t> tryLexInteger(std::string_view allowedSuffixChars = "");
        std::optional<Token> tryLexNumber();
        std::optional<char> tryLexEscapeSequence(bool escapeNewlines);
        std::optional<Token> tryLexCharLiteral();
        std::optional<Token> tryLexStringLiteral();

        char getChar(int offset = 0) const;
        void eatChar(std::size_t count = 1);

        std::vector<std::size_t> m_lineStartIndices;
        std::vector<std::size_t> m_lineEndIndices;
        std::vector<Info> m_infos;
        std::size_t m_col;
        std::size_t m_ln;
        int m_srcIdx;
        std::string_view m_id;
        std::string_view m_src;
    };

}