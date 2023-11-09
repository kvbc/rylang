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

    std::size_t Info::GetStartLineNumber() const { return m_startLn;  }
    std::size_t Info::GetStartColumn    () const { return m_startCol; }
    std::size_t Info::GetEndLineNumber  () const { return m_endLn;    }
    std::size_t Info::GetEndColumn      () const { return m_endCol;   }

    // 

    Lexer::Lexer(std::string_view id, std::string_view src):
        m_src(src),
        m_id(id),
        m_srcIdx(0),
        m_col(1),
        m_ln(1)
    {
        m_lineStartIndices.push_back(0);
        for(std::size_t i = 0; i < m_src.length();) {
            char c1 = m_src.at(i);
            char c2 = (i + 1 < m_src.length()) ? m_src.at(i + 1) : '\0';
            if(c1 == '\n' || c1 == '\r') {
                m_lineEndIndices.push_back(i - 1);
                i++;
                if(c1 == '\r' && c2 == '\n')
                    i++;
                m_lineStartIndices.push_back(i);
            }
            else {
                if(c2 == '\0')
                    m_lineEndIndices.push_back(i + 1);
                i++;
            }
        }
    }

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
            if(tryPushToken(tryLexNameOrKeyword())) continue;
            if(             tryLexComment      () ) continue;
            if(             trySkipWhitespace  () ) continue;
            if(tryPushToken(tryLexNumber       ())) continue;
            if(tryPushToken(tryLexStringLiteral())) continue;
            if(tryPushToken(tryLexCharLiteral  ())) continue;
            if(tryPushToken(tryLexOperator     ())) continue; // after tryLexComment()!
            m_infos.push_back(Info(
                Info::Level::WARN,
                std::format("Unexpected character '{}'", getChar()),
                m_ln, m_col
            ));
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

    std::string Lexer::StringifyInfo(const Info& info) const {  
        std::size_t ln = info.GetStartLineNumber();
        std::size_t col = info.GetStartColumn();
        auto startPtr = m_src.data() + m_lineStartIndices[ln - 1];
        auto endPtr = m_src.data() + m_lineEndIndices[ln - 1];
        std::string_view line(startPtr, endPtr);
        std::size_t lnLen = std::floor(std::log10(ln)) + 1;
        std::size_t leftBarLen = std::max(lnLen, m_id.length());
        std::string underLine(col - 1, ' ');
        underLine += '^';
        underLine += ' ';
        underLine += info.GetMessage();
        return std::format(
            " {:>{}} |\n {:>{}} | {}\n {:>{}} | {}",
            m_id, leftBarLen,
            ln, leftBarLen, line,
            "", leftBarLen, underLine
        );
    }

    // 

    std::string_view::const_pointer Lexer::getSourcePointer(int offset) {
        return m_src.data() + m_srcIdx + offset;
    }

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
                            Info::Level::ERROR,
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
        auto getType = [&]() -> std::optional<Token::Type> {
            auto is = [&](const char * str) -> bool {
                size_t i = 0;
                for(; i < 3 && str[i]!='\0'; i++)
                    if(getChar(i) != str[i])
                        return false;
                eatChar(i);
                return true;
            };
            using T = Token::Type;
            // check 3-length ops
            if(is("**=")) return T::OP_POWER_EQ;
            if(is("<<=")) return T::OP_BIT_LSHIFT_EQ;
            if(is(">>=")) return T::OP_BIT_RSHIFT_EQ;
            // check 2-length ops
            if(is("**" )) return T::OP_POWER;
            if(is("+=" )) return T::OP_ADD_EQ;
            if(is("-=" )) return T::OP_SUB_EQ;
            if(is("*=" )) return T::OP_MUL_EQ;
            if(is("/=" )) return T::OP_DIV_EQ;
            if(is("%=" )) return T::OP_MOD_EQ;
            if(is("<<" )) return T::OP_BIT_LSHIFT;
            if(is(">>" )) return T::OP_BIT_RSHIFT;
            if(is("|=" )) return T::OP_BIT_OR;
            if(is("^=" )) return T::OP_BIT_XOR;
            if(is("&=" )) return T::OP_BIT_AND;
            if(is("==" )) return T::OP_EQ;
            if(is("!=" )) return T::OP_UNEQ;
            if(is("<=" )) return T::OP_LESS_EQ;
            if(is(">=" )) return T::OP_GREAT_EQ;
            if(is("||" )) return T::OP_OR;
            if(is("&&" )) return T::OP_AND;
            if(is(".." )) return T::OP_STRUCT_FIELD_IDX_ACCESS;
            // check 1-length ops
            if(is("-"  )) return T::OP_SUB;
            if(is("+"  )) return T::OP_ADD;
            if(is("*"  )) return T::OP_MUL;
            if(is("/"  )) return T::OP_DIV;
            if(is("%"  )) return T::OP_MOD;
            if(is("~"  )) return T::OP_BIT_NEG;
            if(is("|"  )) return T::OP_BIT_OR;
            if(is("&"  )) return T::OP_BIT_AND;
            if(is("<"  )) return T::OP_LESS;
            if(is(">"  )) return T::OP_GREAT;
            if(is("!"  )) return T::OP_NOT;
            if(is("?"  )) return T::OP_TERNARY_TRUE;
            if(is(":"  )) return T::OP_TERNARY_FALSE;
            if(is("."  )) return T::OP_STRUCT_FIELD_ACCESS;
            if(is(":"  )) return T::OP_SCOPE_ACCESS;
            if(is("="  )) return T::OP_ASSIGN;
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

    std::optional<Token::intlit_t> Lexer::tryLexInteger() {
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
    }

    std::optional<Token> Lexer::tryLexNumber() {
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

    std::optional<char> Lexer::tryLexEscapeSequence(bool escapeNewlines) {
        if(getChar() == '\\') {
            eatChar();
            char c2 = getChar();
            switch(c2) {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': {
                    std::optional<intlit_t> numOpt = tryLexInteger();
                    if(numOpt.has_value()) {
                        intlit_t num = numOpt.value();
                        if(num < 1 || num > 127)
                            m_infos.push_back(Info(
                                Info::Level::ERROR,
                                "Escape sequence out of bounds <1,127>",
                                m_ln, m_col
                            ));
                        return char(num);
                    }
                    return {};
                }
                case 'a':  eatChar(); return '\a';
                case 'b':  eatChar(); return '\b';
                case 'e':  eatChar(); return '\e';
                case 'f':  eatChar(); return '\f';
                case 'n':  eatChar(); return '\n';
                case 'r':  eatChar(); return '\r';
                case 't':  eatChar(); return '\t';
                case 'v':  eatChar(); return '\v';
                case '\\': eatChar(); return '\\';
                case '\r':
                case '\n':
                    if(escapeNewlines) {
                        eatChar();
                        return {};
                    }
                    // else fallback
                default:
                    m_infos.push_back(Info(
                        Info::Level::ERROR,
                        std::format("Invalid escape sequence '\\{}'", c2),
                        m_ln, m_col
                    ));
                    eatChar();
                    return {};
            }
        }
        return {};
    }

    std::optional<Token> Lexer::tryLexCharLiteral() {
        if(getChar() == '\'') {
            eatChar();
            char ch = getChar();

            std::optional<char> escChar = tryLexEscapeSequence(false);
            if(escChar.has_value())
                ch = escChar.value();
            else
                eatChar();

            if(getChar() != '\'')
                m_infos.push_back(Info(
                    Info::Level::ERROR,
                    "Unterminated character literal",
                    m_ln, m_col
                ));
            eatChar();

            return Token(Token::Type::CHAR_LIT, ch);
        }
        return {};
    }

    std::optional<Token> Lexer::tryLexStringLiteral() {
        char c1 = getChar(0);
        char c2 = getChar(1);
        char c3 = getChar(2);
        if(c1=='`' || c1=='"') {
            bool isRaw = (c1 == '`');
            bool isMultiline = (c2==c1 && c3==c1);
            size_t numQuotes = isMultiline ? 3 : 1;
            size_t ln = m_ln;
            eatChar(numQuotes);
            auto srcStartPos = getSourcePointer();
            std::string escapedStr;
            for(;;) {
                char c = getChar();
                if(!isMultiline && ln != m_ln) {
                    ln = m_ln;
                    m_infos.push_back(Info(
                        Info::Level::ERROR,
                        "Unexpected new line in single-line string literal",
                        m_ln, m_col
                    ));
                }
                if(c == c1) {
                    if(isMultiline) {
                        char c2 = getChar(1);
                        char c3 = getChar(2);
                        if(c2 != c1 || c3 != c1)
                            m_infos.push_back(Info(
                                Info::Level::ERROR,
                                "Expected termination of multi-line string literal",
                                m_ln, m_col
                            ));
                        eatChar(3);
                    }
                    else
                        eatChar();
                    break;
                }
                if(c == CHAR_EOF) {
                    m_infos.push_back(Info(
                        Info::Level::ERROR,
                        "Unterminated single-line string literal",
                        m_ln, m_col
                    ));
                    break;
                }
                if(isRaw) {
                    eatChar();
                }
                if(!isRaw) {
                    if(c == '\\') {
                        bool escapeNewlines = isMultiline;
                        std::optional<char> escChar = tryLexEscapeSequence(escapeNewlines);
                        if(escChar.has_value())
                            escapedStr += escChar.value();
                    }
                    else {
                        escapedStr += c;
                        eatChar();
                    }
                }
            }
            auto srcEndPos = getSourcePointer();
            if(isRaw)
                return Token(
                    Token::Type::STRING_LIT,
                    std::string_view(srcStartPos, srcEndPos - numQuotes)
                );
            return Token(
                Token::Type::STRING_LIT,
                escapedStr
            );
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