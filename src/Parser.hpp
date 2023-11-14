#pragma once

#include "Infos.hpp"
#include "Token.hpp"
#include "ASTNode.hpp"

#include <vector>
#include <optional>

namespace ry {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens, const Infos& infos);

        const Infos& GetInfos() const;

        ASTNode Parse();

    private:
        bool assertToken(Token::Type type, int offset = 0);
        const Token * getToken(int offset = 0) const;
        void eatToken();

        ASTNode::Statement parseStatement();
        ASTNode::ExpressionBlock parseBlock();

        int m_tokenIdx;
        const std::vector<Token>& m_tokens;
        Infos m_infos;
    };

}