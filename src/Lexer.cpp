#include "Lexer.hpp"
#include <assert.h>
#include <format>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>
#include <math.h>

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
            if(trySkipWhitespace())
                continue;
            if(tryPushToken(tryLexNumber()))
                continue;
            tokens.push_back(Token(getChar()));
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

    bool Lexer::trySkipWhitespace() {
        for(bool skipped = false ;;) {
            char c = getChar();
            if(c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v') {
                eatChar();
                skipped = true;
            }
            else
                return skipped;
        }
    }

    std::optional<Token> Lexer::tryLexNumber() {
        auto tryLexInteger = [&]() -> std::optional<intlit_t> {
            auto isValidDigit = [](char c, int base = 10) -> bool {
                bool is09 = c >= '0' && c < '0' + base;
                if(base <= 10)
                    return is09;
                return is09
                    || (c >= 'a' && c < 'a' + base - 10)
                    || (c >= 'A' && c < 'A' + base - 10);
            };
            char c1 = getChar(0);
            char c2 = getChar(1);
            if(isValidDigit(c1)) {
                intlit_t base = 10;
                const char * baseName = "decimal";
                if(c1=='0' && (c2=='b' || c2=='o' || c2=='x')) {
                         if(c2 == 'b') { base = 2;  baseName = "binary"; eatChar(2); }
                    else if(c2 == 'o') { base = 8;  baseName = "octal";  eatChar(2); }
                    else if(c2 == 'x') { base = 16; baseName = "hex";    eatChar(2); }
                    char c3 = getChar();
                    if(c3!='_' && !isValidDigit(c3, base)) {
                        m_infos.push_back(Info(
                            Info::Level::ERROR,
                            "Unfinished integer literal",
                            m_ln, m_col
                        ));
                        return {};
                    }
                }

                auto srcStartPtr = m_src.data() + m_srcIdx;
                eatChar();
                for(;;) {
                    char c = getChar();
                    if(isValidDigit(c, base)) {
                        eatChar();
                    } else if(isValidDigit(c, 10)) {
                        m_infos.push_back(Info(
                            Info::Level::ERROR,
                            std::format("Invalid digit '{}' in {} integer literal", c, baseName),
                            m_ln, m_col
                        ));
                        eatChar();
                    } else { // not a digit
                        if(c == '_')
                            do eatChar(); while(getChar() == '_');
                        else
                            break;
                    }
                }
                auto srcEndPtr = m_src.data() + m_srcIdx;
                
                intlit_t num = 0;
                intlit_t curBase = 1;
                std::string_view numStr(srcStartPtr, srcEndPtr);
                for(auto it = numStr.crbegin(); it != numStr.crend(); it++) {
                    char c = *it;
                    if(c == '_')
                        continue;
                    bool is_09 = (c >= '0' && c <= '9');
                    bool is_az = (c >= 'a' && c <= 'z');
                    intlit_t digit = is_09 ? c-'0' : is_az ? c-'a'+10 : c-'A'+10;
                    num += digit * curBase;
                    curBase *= base;
                }
                return num;
            }
            return {};
        };
        auto tryLexExponent = [&]() -> std::optional<floatlit_t> {
            char c1 = getChar(0);
            char c2 = getChar(1);
            if(c1=='e' || c1=='E') {
                eatChar();
                floatlit_t e = 1;
                if(c2=='+' || c2=='-') {
                    eatChar();
                    if(c2 == '-')
                        e *= -1;
                }
                auto posE = tryLexInteger();
                if(posE.has_value())
                    e = std::pow(10.0, e * posE.value());
                else
                    m_infos.push_back(Info(
                        Info::Level::ERROR,
                        "Unfinished exponent",
                        m_ln, m_col
                    ));
                return e;
            }
            return {};
        };
        auto int1 = tryLexInteger();
        if(int1.has_value()) {
            if(getChar() == '.') {
                eatChar();
                floatlit_t num = floatlit_t(int1.value());
                auto int2 = tryLexInteger();
                if(int2.has_value()) {
                    std::size_t int2len = std::floor(std::log10(floatlit_t(int2.value()))) + 1;
                    num += floatlit_t(int2.value()) / std::pow(10, int2len);
                } else {
                     m_infos.push_back(Info(
                        Info::Level::ERROR,
                        "Unfinished float literal",
                        m_ln, m_col
                     ));
                }
                num *= tryLexExponent().value_or(1);
                return Token(Token::Type::FLOAT_LIT, num);
            }
            std::optional<floatlit_t> exp = tryLexExponent();
            if(exp.has_value())
                return Token(Token::Type::FLOAT_LIT, floatlit_t(int1.value()) * exp.value());
            return Token(Token::Type::INT_LIT, int1.value());
        }
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