#pragma once

#include "Infos.hpp"
#include "Token.hpp"
#include "ASTNode.hpp"
#include "src/ASTNode.hpp"

#include <variant>
#include <vector>
#include <optional>

namespace ry {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens, const Infos& infos);

        const Infos& GetInfos() const;

        std::optional<ASTNode> Parse();

    private:
        template<typename T>
        bool isToken(const std::optional<T>& kind = {});
        bool isToken(const std::optional<Token::Kind>& kind);

        bool expectToken(const Token::Kind& kind);
        const Token * getToken(int offset = 0);
        void eatToken();

        void error(std::string_view msg);
        void errorExpected(std::string_view what);
        template<typename T>
        void errorExpectedToken();
        void errorExpectedToken(const Token::Kind& kind);

        std::optional<ASTNode::Type> parseType(bool mustParse = true);
        std::optional<ASTNode::TypeStruct::Field> parseStructTypeField();

        std::optional<ASTNode::Expression>                parseExpression                (bool mustParse = true, std::optional<ASTNode::ExpressionBinaryOperation::Kind> currentBinOpKind = {});
        std::optional<ASTNode::ExpressionLiteral::Struct> parseStructLiteralExpression   (bool mustParse = true);
        std::optional<ASTNode::ExpressionLiteral>         parseLiteralExpression         (bool mustParse = true);
        std::optional<ASTNode::ExpressionFunctionCall>    parseFunctionCallExpression    (bool mustParse, const ASTNode::Expression& expr);
        std::optional<ASTNode::ExpressionBlock>           parseBlockExpression           (bool mustParse = true);
        std::optional<ASTNode::ExpressionIf>              parseIfExpression              (bool mustParse = true);
        std::optional<ASTNode::ExpressionLoop>            parseLoopExpression            (bool mustParse = true);
        std::optional<ASTNode::ExpressionName>            parseNameExpression            (bool mustParse = true);
        std::optional<ASTNode::ExpressionUnaryOperation>  parseUnaryOperationExpression  (bool mustParse = true);
        std::optional<ASTNode::ExpressionBinaryOperation> parseBinaryOperationExpression (bool mustParse, const ASTNode::Expression& expr, std::optional<ASTNode::ExpressionBinaryOperation::Kind> currentBinOpKind = {});

        std::optional<ASTNode::Statement>                   parseStatement                   (bool mustParse = true);
        std::optional<ASTNode::StatementBinaryOperation>    parseBinaryOperationStatement    (bool mustParse = true);
        std::optional<ASTNode::StatementVariableDefinition> parseVariableDefinitionStatement (bool mustParse = true);
        std::optional<ASTNode::StatementAssignment>         parseAssignmentStatement         (bool mustParse = true);
        std::optional<ASTNode::StatementContinue>           parseContinueStatement           (bool mustParse = true);
        std::optional<ASTNode::StatementBreak>              parseBreakStatement              (bool mustParse = true);

        int m_tokenIdx;
        const std::vector<Token>& m_tokens;
        Infos m_infos;
    };

}