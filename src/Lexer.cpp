#include "Lexer.hpp"
#include <assert.h>
#include <iostream>
#include <string_view>

namespace ry {

    using Info = ry::Lexer::Info;

    Info::Info(
        Lexer::Info::Level level, std::string_view msg,
        std::size_t startLn, std::size_t startCol
    ):
        m_level(level), m_msg(msg),
        m_startLn(startLn), m_startCol(startCol),
        m_endLn(startLn), m_endCol(startCol)
    {}

    Info::Info(
        Lexer::Info::Level level, std::string_view msg,
        std::size_t startLn, std::size_t startCol,
        std::size_t endLn, std::size_t endCol
    ):
        m_level(level), m_msg(msg),
        m_startLn(startLn), m_startCol(startCol),
        m_endLn(endLn), m_endCol(endCol)
    {}

    Info::Level Info::GetLevel() const {
        return m_level;
    }

    std::string_view Info::GetMessage() const {
        return m_msg;
    }

    // 

    Lexer::Lexer(std::string_view src):
        m_src(src),
        m_srcIdx(0),
        m_col(1),
        m_ln(1)
    {}

    std::vector<Token> Lexer::Lex() {
        std::vector<Token> tokens;

        auto tryPushToken = [&](std::optional<Token> token) -> bool {
            if(token.has_value()) {
                tokens.push_back(token.value());
                return true;
            }
            return false;
        };

        while(getChar() != CHAR_EOF) {
            if(tryPushToken(tryLexNameOrKeyword()))
                continue;
            if(tryPushToken(tryLexOperator()))
                continue;
            if(tryLexComment())
                continue;
            eatChar();
        }

        return tokens;
    }

    std::string_view Lexer::GetSource() const {
        return m_src;
    }

    std::vector<Info> Lexer::GetInfos() const {
        return m_infos;
    }

    // 

    std::optional<Token> Lexer::tryLexNameOrKeyword() {
        auto isValidStartChar = [](char c) -> bool {
            return (c == '_') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        };
        auto isValidChar = [&](char c) -> bool {
            return (c >= '0' && c <= '9') || isValidStartChar(c);
        };

        if(isValidStartChar(getChar())) {
            auto srcStartPtr = m_src.data() + m_srcIdx;
            std::size_t len = 1;

            eatChar();
            while(isValidChar(getChar())) {
                eatChar();
                len++;
            }

            std::string_view str(srcStartPtr, len);
            std::optional<Token::Type> kwType = Token::GetStringToKeywordType(str);
            if(kwType.has_value())
                return Token(kwType.value());
            return Token(Token::Type::NAME, str);
        }

        return {};
    }

    bool Lexer::tryLexComment() {
        char c1 = getChar(0);
        char c2 = getChar(1);
        if(c1 == '/') {
            if(c2 == '/') { // "//" single line
                std::size_t ln = m_ln;
                while(ln == m_ln && getChar() != CHAR_EOF)
                    eatChar();
                return true;
            } else if(c2 == '*') {// "/*" multi line
                eatChar(2);
                for(;;) {
                    c1 = getChar(0);
                    c2 = getChar(1);
                    if(c1 == CHAR_EOF) {
                        m_infos.push_back(Info(
                            Info::Level::WARN,
                            "Unterminated multi-line comment",
                            m_ln, m_col
                        ));
                        break;
                    }
                    if(c1 == '*' && c2 == '/') {// "*/" end
                        eatChar(2);
                        break;
                    }
                    eatChar();
                }
                return true;
            }
        }
        return false;
    }

    std::optional<Token> Lexer::tryLexOperator() {
        char c1 = getChar(0);
        char c2 = getChar(1);
        char c3 = getChar(2);
        auto getType = [&]() -> std::optional<Token::Type> {
            auto is3 = [&](const char * str) -> bool {
                bool is = c1==str[0] && c2==str[1] && c3==str[2];
                if(is)
                    eatChar(3);
                return is;
            };
            auto is2 = [&](const char * str) -> bool {
                bool is = c1==str[0] && c2==str[1];
                if(is)
                    eatChar(2);
                return is;
            };
            using T = Token::Type;
            if(is3("//=")) return T::OP_FLOORDIV_EQ;
            if(is3("<<=")) return T::OP_BIT_LSHIFT_EQ;
            if(is3(">>=")) return T::OP_BIT_RSHIFT_EQ;
            // check 3-length ops first!
            if(is2("**" )) return T::OP_POWER;
            if(is2("+=" )) return T::OP_ADD_EQ;
            if(is2("-=" )) return T::OP_SUB_EQ;
            if(is2("*=" )) return T::OP_MUL_EQ;
            if(is2("/=" )) return T::OP_DIV_EQ;
            if(is2("%=" )) return T::OP_MOD_EQ;
            if(is2("<<" )) return T::OP_BIT_LSHIFT;
            if(is2(">>" )) return T::OP_BIT_RSHIFT;
            if(is2("|=" )) return T::OP_BIT_OR;
            if(is2("^=" )) return T::OP_BIT_XOR;
            if(is2("&=" )) return T::OP_BIT_AND;
            if(is2("==" )) return T::OP_EQ;
            if(is2("!=" )) return T::OP_UNEQ;
            if(is2("<=" )) return T::OP_LESS_EQ;
            if(is2(">=" )) return T::OP_GREAT_EQ;
            if(is2("||" )) return T::OP_OR;
            if(is2("&&" )) return T::OP_AND;
            if(is2(".." )) return T::OP_2DOT;
            return {};
        };
        std::optional<Token::Type> tkType = getType();
        if(tkType.has_value())
            return Token(tkType.value());
        return {};
    }

    // 

    char Lexer::getChar(int offset) const {
        if(m_srcIdx + offset < 0)
            return CHAR_EOF;
        if(m_srcIdx + offset >= m_src.length())
            return CHAR_EOF;
        return m_src.at(m_srcIdx + offset);
    }

    void Lexer::eatChar(std::size_t count) {
        assert(count >= 1);
        for(size_t i = 0; i < count; i++) {
            char c1 = getChar(0);
            char c2 = getChar(1);
            if(c1 == '\n' || c1 == '\r') {
                m_srcIdx++;
                if(c1 == '\r' && c2 == '\n') // CRLF
                    m_srcIdx++;
                else if(c1 == '\n' && c2 == '\r') // LFCR
                    std::cerr << "Error: LFCR line encoding (?)" << std::endl;
                m_col = 1;
                m_ln++;
            } else {
                m_col++;
                m_srcIdx++;
            }
        }
    }

}