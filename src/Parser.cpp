#include "Parser.hpp"

namespace ry {

    Parser::Parser(const std::vector<Token>& tokens):
        m_tokens(tokens)
    {}

    ASTNode Parser::Parse() const {
        return ASTNode(ASTNode::Expression(parseBlock()));
    }

    // 

    void Parser::assertToken(Token::Type type, int offset) const {
        const Token * token = getToken(offset);
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

    ASTNode::Statement Parser::parseStatement() const {
        
    }

    ASTNode::ExpressionBlock Parser::parseBlock() const {
        
    }

}