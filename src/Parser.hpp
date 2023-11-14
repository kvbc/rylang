#pragma once

#include "Token.hpp"
#include "ASTNode.hpp"

#include <vector>
#include <optional>

namespace ry {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);

        ASTNode Parse() const;

    private:
        void assertToken(Token::Type type, int offset = 0) const;
        const Token * getToken(int offset = 0) const;
        void eatToken();

        ASTNode::Statement parseStatement() const;
        ASTNode::ExpressionBlock parseBlock() const;

        int m_tokenIdx;
        const std::vector<Token>& m_tokens;
    };

}