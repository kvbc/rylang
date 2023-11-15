#include "Parser.hpp"

#include <format>
#include <memory>

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

    std::optional<ASTNode> Parser::Parse() {
        std::optional<ASTNode::Type> typeNode = parseType();
        if(typeNode.has_value())
            return ASTNode(typeNode.value());
        return {};
    }

    // 

    bool Parser::isToken(Token::Type type) {
        return getToken().GetType() == type;
    }

    bool Parser::assertToken(Token::Type type) {
        if(!isToken(type)) {
            errorExpected(Token::Stringify(type));
            return false;
        }
        return true;
    }

    const Token& Parser::getToken(int offset) {
        bool tooLow = m_tokenIdx + offset < 0;
        bool tooHigh = m_tokenIdx + offset >= m_tokens.size();
        if(tooLow || tooHigh) {
            m_infos.Push(Infos::Info(
                Infos::Info::Level::ERROR,
                "Unexpected EOF"
            ));
        }
        return m_tokens.at(m_tokenIdx + offset);
    };

    void Parser::eatToken() {
        m_tokenIdx++;
    }

    // 

    void Parser::errorExpected(std::string_view what) {
        const Token& token = getToken();
        m_infos.Push(Infos::Info(
            Infos::Info::Level::ERROR,
            std::format(
                "Unexpected token: Expected {}, got \"{}\"",
                what,
                Token::Stringify(token.GetType())
            ),
            token.GetSourcePosition()
        ));
    }

    // 

    std::optional<ASTNode::Type> Parser::parseType() {
        ASTNode::Type::Attribs attribs;
        if(isToken(Token::Type::TILDE)) {
            eatToken();
            attribs.isMutable = true;
        }
        if(isToken(Token::Type::QUESTION)) {
            eatToken();
            attribs.isOptional = true;
        }

        if(isToken(Token::Type::ASTERISK)) {
            // pointer
            eatToken();
            std::optional<ASTNode::Type> optBaseType = parseType();
            if(optBaseType.has_value()) {
                ASTNode::Type baseType = optBaseType.value();
                ASTNode::TypePointer ptrType = std::make_shared<ASTNode::Type>(baseType);
                return ASTNode::Type(ptrType, attribs);
            }
            return {};
        }
        else if(getToken().IsPrimitiveType()) {
            // primitive
            auto primType = ASTNode::TypePrimitive( int(getToken().GetType()) - (int(Token::Type::_KW_FIRST_TYPE) + 1) );
            return ASTNode::Type(primType, attribs);
        }
        else if(isToken(Token::Type::LSQUARE)) {
            // struct or function
            eatToken();
            // ...
            assertToken(Token::Type::RSQUARE);
            eatToken();
            if(isToken(Token::Type::OP_FUNC_ARROW)) {
                // function
                eatToken();
                std::optional<ASTNode::Type> optRetType = parseType();
                if(optRetType.has_value()) {
                    auto retType = optRetType.value();
                }
                return {};
            }
            // struct
        }

        // not a type
        errorExpected("type");
        return {};
    }

}