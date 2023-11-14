#include "Parser.hpp"

#include <format>

namespace ry {

    Parser::Parser(
        const std::vector<Token>& tokens,
        const Infos& infos
    ):
        m_tokens(tokens),
        m_infos(infos),
        m_tokenIdx(0)
    {}

    const Infos& Parser::GetInfos() const {
        return m_infos;
    }

    ASTNode Parser::Parse() {
        return ASTNode(ASTNode::Expression(parseBlock()));
    }

    // 

    bool Parser::assertToken(Token::Type type, int offset) {
        const Token * token = getToken(offset);
        if(token == nullptr || token->GetType() != type) {
            m_infos.Push(Infos::Info(
                Infos::Info::Level::ERROR,
                std::format(
                    "Unexpected token: Expected \"{}\", got \"{}\"",
                    Token::Stringify(type),
                    (token == nullptr) ? "EOF" : Token::Stringify(token->GetType())
                ),
                token->GetSourcePosition()
            ));
            return false;
        }
        return true;
    }

    const Token * Parser::getToken(int offset) const {
        if(m_tokenIdx + offset < 0)
            return nullptr;
        if(m_tokenIdx + offset >= m_tokens.size())
            return nullptr;
        return &m_tokens.at(m_tokenIdx + offset);
    };

    void Parser::eatToken() {
        m_tokenIdx++;
    }

    // 

    ASTNode::Statement Parser::parseStatement() {
        
    }

    ASTNode::ExpressionBlock Parser::parseBlock() {
        assertToken(Token::Type::OP_POWER_EQ);
        return ASTNode::ExpressionBlock();
    }

}