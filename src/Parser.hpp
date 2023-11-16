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
        static std::optional<ASTNode::TypePrimitive> getTokenTypeToPrimitiveType(Token::Type type);
        static std::optional<ASTNode::ExpressionUnaryOperation::Kind>  getTokenTypeToUnaryOperationKind(Token::Type type);
        static std::optional<ASTNode::ExpressionBinaryOperation::Kind> getTokenTypeToBinaryOperationKind(Token::Type type);

        bool isToken(Token::Type type, std::optional<Token::Type> orType = {});
        bool expectToken(Token::Type type);
        const Token& getToken(int offset = 0);
        void eatToken();

        void errorExpected(std::string_view what);
        void errorExpectedToken(Token::Type type);

        std::optional<ASTNode::Type> parseType(bool mustParse = true);
        std::optional<ASTNode::TypeStruct::Field> parseStructTypeField();

        std::optional<ASTNode::Expression>                parseExpression               (bool mustParse = true);
        std::optional<ASTNode::ExpressionLiteral::Struct> parseStructLiteralExpression  (bool mustParse = true);
        std::optional<ASTNode::ExpressionLiteral>         parseLiteralExpression        (bool mustParse = true);
        std::optional<ASTNode::ExpressionBlock>           parseBlockExpression          (bool mustParse = true);
        std::optional<ASTNode::ExpressionIf>              parseIfExpression             (bool mustParse = true);
        std::optional<ASTNode::ExpressionLoop>            parseLoopExpression           (bool mustParse = true);
        std::optional<ASTNode::ExpressionName>            parseNameExpression           (bool mustParse = true);
        std::optional<ASTNode::ExpressionUnaryOperation>  parseUnaryOperationExpression (bool mustParse = true);

        std::optional<ASTNode::Statement> parseStatement(bool mustParse = true);

        int m_tokenIdx;
        const std::vector<Token>& m_tokens;
        Infos m_infos;
    };

}