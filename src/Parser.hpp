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

        std::optional<ASTNode> Parse();

    private:
        bool isToken(Token::Type type);
        bool assertToken(Token::Type type);
        const Token& getToken(int offset = 0);
        void eatToken();

        void errorExpected(std::string_view what);

        std::optional<ASTNode::Type> parseType();

        int m_tokenIdx;
        const std::vector<Token>& m_tokens;
        Infos m_infos;
    };

}