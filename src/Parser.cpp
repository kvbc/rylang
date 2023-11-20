#include "Parser.hpp"
#include "Token.hpp"
#include "ry.hpp"
#include "src/ASTNode.hpp"
#include "src/Token.hpp"

#include <format>
#include <memory>
#include <iostream>
#include <optional>
#include <stack>
#include <variant>
#include <stacktrace>

namespace ry {

#define RY_PARSER__WRAP_PARSE_FUNC(WHAT, RET_TYPE, FUNC_BLOCK) \
    { \
        auto start_token_idx = m_tokenIdx; \
        auto parse_func = [&]() -> RET_TYPE { \
            {FUNC_BLOCK} \
            return {}; \
        }; \
        auto ret = parse_func(); \
        if(ret) \
            return ret; \
        m_tokenIdx = start_token_idx; \
        if(mustParse) \
            errorExpected((WHAT)); \
        return {}; \
    }

#define RY_PARSER__ASSERT(cond) { if(!(cond)) return {}; }

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
        if(auto stmt = parseStatement(false))
            return ASTNode(stmt.value());
        return {};
    }

    // 

    template<typename T>
    bool Parser::isToken(const std::optional<T>& expectedKind) {
        if(auto token = getToken()) {
            auto kind = token->GetKind();
            if(Token::IsKind<T>(kind)) {
                if(expectedKind)
                    return Token::IsKindEqual(kind, expectedKind.value());
                return true;
            }
        }
        return false;
    }

    bool Parser::isToken(const std::optional<Token::Kind>& expectedKind) {
        if(auto token = getToken()) {
            auto kind = token->GetKind();
            if(expectedKind)
                return Token::IsKindEqual(kind, expectedKind.value());
        }
        return false;
    }

    bool Parser::expectToken(const Token::Kind& kind) {
        if(!isToken(kind)) {
            errorExpectedToken(kind);
            return false;
        }
        return true;
    }

    const Token * Parser::getToken(int offset) {
        bool tooLow = m_tokenIdx + offset < 0;
        bool tooHigh = m_tokenIdx + offset >= m_tokens.size();
        if(tooLow || tooHigh)
            return nullptr;
        return &m_tokens.at(m_tokenIdx + offset);
    };

    void Parser::eatToken() {
        m_tokenIdx++;
    }

    // 

    void Parser::errorExpected(std::string_view what) {
        std::cout << std::stacktrace::current() << std::endl;
        if(auto token = getToken()) {
            m_infos.Push(Infos::Info(
                Infos::Info::Level::ERROR,
                std::format(
                    "Unexpected token: Expected {}, got \"{}\"",
                    what,
                    token->Stringify()
                ),
                token->GetSourcePosition()
            ));
        }
    }

    template<typename T>
    void Parser::errorExpectedToken() {
        errorExpected(Token::StringifyKindType<T>());
    }

    void Parser::errorExpectedToken(const Token::Kind& kind) {
        errorExpected(Token::StringifyKind(kind));
    }

    // 

    /*
     *
     * Type
     *
     */

    std::optional<ASTNode::Type> Parser::parseType(bool mustParse) {
        // type attribs

        ASTNode::Type::Attribs attribs;
        if(isToken('~')) {
            eatToken();
            attribs.isMutable = true;
        }
        if(isToken('?')) {
            eatToken();
            attribs.isOptional = true;
        }

        // type

        auto token = getToken();
        RY_PARSER__ASSERT(token);

        if(isToken('*')) {
            // pointer
            eatToken();
            auto optBaseType = parseType();
            RY_PARSER__ASSERT(optBaseType);
            ASTNode::Type baseType = optBaseType.value();
            ASTNode::TypePointer ptrType = std::make_shared<ASTNode::Type>(baseType);
            return ASTNode::Type(ptrType, attribs);
        }
        else if(auto optPrimitiveType = ASTNode::Type::GetTokenKindToPrimitiveType(token->GetKind())) {
            // primitive
            eatToken();
            return ASTNode::Type(optPrimitiveType.value(), attribs);
        }
        else if(isToken('[')) {
            // struct or function
            eatToken();
            
            ASTNode::TypeStruct::Fields structFields;
            for(;;) {
                if(isToken(']')) {
                    eatToken();
                    break;
                }

                auto optField = parseStructTypeField();
                RY_PARSER__ASSERT(optField);
                structFields.push_back(optField.value());

                bool isSep = isToken(',') || isToken(';');
                bool isEnd = isToken(']');
                if(isSep)
                    eatToken();
                else if(!isEnd)
                    errorExpected("separator");
            }
            auto structType = ASTNode::TypeStruct(structFields);

            if(isToken(Token::Code::FunctionArrow)) {
                // function
                eatToken();
                std::optional<ASTNode::Type> optRetType = parseType();
                RY_PARSER__ASSERT(optRetType.has_value());
                auto retType = optRetType.value();
                auto retTypePtr = std::make_shared<ASTNode::Type>(retType);
                auto funcType = ASTNode::TypeFunction(structType, retTypePtr);
                return ASTNode::Type(funcType, attribs);
            }

            // struct
            return ASTNode::Type(structType, attribs);
        }

        // not a type

        if(mustParse)
            errorExpected("type");
        return {};
    }

    std::optional<ASTNode::TypeStruct::Field> Parser::parseStructTypeField() {
        using TypeStruct = ASTNode::TypeStruct;
        using Field = TypeStruct::Field;
        using NamedField = TypeStruct::NamedField;
        using UnnamedField = TypeStruct::UnnamedField;

        enum class TypeRepsPos {Pre, Post};

        auto tryParseTypeReps = [&](TypeRepsPos pos) -> UnnamedField::TypeReps {
            if(pos == TypeRepsPos::Post) {
                if(isToken('*'))
                    eatToken();
                else
                    return {};
            }

            auto optExpr = parseExpression(false);
            if(optExpr.has_value()) {
                if(pos == TypeRepsPos::Pre) {
                    RY_PARSER__ASSERT(expectToken('*'));
                    eatToken();
                }
                return std::make_shared<ASTNode::Expression>(optExpr.value());
            }

            return {};
        };

        auto parseNames = [&](bool mustParse = true) -> std::optional<NamedField::Names> {
            if(!isToken<TokenName>()) {
                if(mustParse)
                    errorExpectedToken<TokenName>();
                return {};
            }
            NamedField::Names names;
            for(;;) {
                if(auto token = getToken())
                    if(auto name = token->GetName()) {
                        names.push_back(*name);
                        eatToken();
                        if(isToken(',')) {
                            eatToken();
                            continue;
                        }
                        else
                            break;
                    }
                break;
            }
            return names;
        };

        auto parseFieldType = [&](bool mustParse = true) -> std::optional<TypeStruct::FieldType> {
            auto optType = parseType(mustParse);
            RY_PARSER__ASSERT(optType.has_value());
            return std::make_shared<ASTNode::Type>(optType.value());
        };

        auto tryParseDefaultValue = [&]() -> TypeStruct::FieldDefaultValue {
            if(isToken('=')) {
                eatToken();
                auto optExpr = parseExpression();
                RY_PARSER__ASSERT(optExpr.has_value());
                return std::make_shared<ASTNode::Expression>(optExpr.value());
            }
            return {};
        };

        auto tryParseUnnamedField = [&]() -> std::optional<UnnamedField> {
            UnnamedField::TypeReps typeReps;
            auto optPreTypeReps = tryParseTypeReps(TypeRepsPos::Pre);
            if(optPreTypeReps)
                typeReps = optPreTypeReps.value();

            auto optFieldType = parseFieldType();
            if(optFieldType.has_value()) {
                auto fieldType = optFieldType.value();

                if(!typeReps) {
                    auto optPostTypeReps = tryParseTypeReps(TypeRepsPos::Post);
                    if(optPostTypeReps)
                        typeReps = optPostTypeReps.value();
                }

                auto defaultValue = tryParseDefaultValue();
                return UnnamedField(fieldType, typeReps, defaultValue);
            }
            return {};
        };

        auto tryParseNamedField = [&]() -> std::optional<NamedField> {
            auto optNames = parseNames(false);
            if(optNames) {
                auto names = optNames.value();
                auto optFieldType = parseFieldType();
                RY_PARSER__ASSERT(optFieldType.has_value());
                auto fieldType = optFieldType.value();
                auto optDefaultValue = tryParseDefaultValue();
                return NamedField(names, fieldType, optDefaultValue);
            }
            return {};
        };

        auto optNamedField = tryParseNamedField();
        if(optNamedField) {
            return optNamedField.value();
        } else {
            auto optUnnamedField = tryParseUnnamedField();
            RY_PARSER__ASSERT(optUnnamedField);
            return optUnnamedField.value();
        }

        return {};
    }


    /*
     *
     * Expression
     *
     */

    std::optional<ASTNode::Expression> Parser::parseExpression(bool mustParse, std::optional<ASTNode::ExpressionBinaryOperation::Kind> currentBinOpKind) {
    #define TRY_RETURN(OPT) { \
        if(auto opt = OPT) \
            return ASTNode::Expression(opt.value(), isGrouped); \
    }
        RY_PARSER__WRAP_PARSE_FUNC("expression", std::optional<ASTNode::Expression>, {
            bool isGrouped = false;
            if(isToken('(')) {
                eatToken();
                isGrouped = true;
                currentBinOpKind = {};
            }

            auto tryParse = [&]() -> std::optional<ASTNode::Expression> {
                TRY_RETURN(parseNameExpression          (false));
                TRY_RETURN(parseLiteralExpression       (false));
                TRY_RETURN(parseBlockExpression         (false));
                TRY_RETURN(parseIfExpression            (false));
                TRY_RETURN(parseLoopExpression          (false));
                TRY_RETURN(parseUnaryOperationExpression(false));
                return {};
            };
            auto optExpr = tryParse();
            if(optExpr.has_value()) {
                if(auto optFuncCall = parseFunctionCallExpression(false, optExpr.value()))
                    optExpr = ASTNode::Expression(optFuncCall.value(), isGrouped);
                else if(auto optBinOp = parseBinaryOperationExpression(false, optExpr.value(), currentBinOpKind))
                    optExpr = ASTNode::Expression(optBinOp.value(), isGrouped);
            }

            if(isGrouped) {
                RY_PARSER__ASSERT(expectToken(')'));
                eatToken();
            }

            RY_PARSER__ASSERT(optExpr);
            return optExpr.value();
        });
    #undef TRY_RETURN
    }

    std::optional<ASTNode::ExpressionLiteral::Struct> Parser::parseStructLiteralExpression(bool mustParse) {
        using StructLiteral = ASTNode::ExpressionLiteral::Struct;
        RY_PARSER__WRAP_PARSE_FUNC("struct literal", std::optional<StructLiteral>, {
            isToken('[');

            if(isToken('[')) {
                eatToken();

                auto parseField = [&]() -> std::optional<StructLiteral::Field> {
                    StructLiteral::Field::FieldName name;
                    if(auto token = getToken())
                        if(auto namePtr = token->GetName()) {
                            name = *namePtr;
                            eatToken();
                            RY_PARSER__ASSERT(expectToken('='));
                            eatToken();
                        }
                    auto optExpr = parseExpression(mustParse);
                    RY_PARSER__ASSERT(optExpr.has_value());
                    auto value = std::make_shared<ASTNode::Expression>(optExpr.value());
                    return StructLiteral::Field(value, name);
                };

                StructLiteral::Fields fields;
                for(;;) {
                    if(isToken(']')) {
                        eatToken();
                        break;
                    }

                    auto optField = parseField();
                    RY_PARSER__ASSERT(optField.has_value())
                    fields.push_back(optField.value());

                    bool isSep = isToken(',') || isToken(';');
                    bool isEnd = isToken(']');
                    if(isSep)
                        eatToken();
                    else if(!isEnd)
                        errorExpected("separator");
                }
                return StructLiteral(fields);
            }
        })
    }

    std::optional<ASTNode::ExpressionLiteral> Parser::parseLiteralExpression(bool mustParse) {
        using Literal = ASTNode::ExpressionLiteral;

        auto optStructLiteral = parseStructLiteralExpression(false);
        if(optStructLiteral.has_value()) {
            return ASTNode::ExpressionLiteral(optStructLiteral.value());
        }

        if(auto token = getToken())
            if(auto literal = std::get_if<TokenLiteral>(&token->GetKind())) {
                eatToken();
                Literal::Data litData = std::visit(overloaded{
                    [](Literal::Int intValue)              -> Literal::Data { return intValue; },
                    [](Literal::Float floatValue)          -> Literal::Data { return floatValue; },
                    [](Literal::Char charValue)            -> Literal::Data { return charValue; },
                    [](const Literal::String& stringValue) -> Literal::Data { return stringValue; },
                    [](bool boolValue)                     -> Literal::Data { return boolValue; },
                }, literal->GetValue());
                return ASTNode::ExpressionLiteral(litData);
            }

        if(mustParse)
            errorExpected("literal");
        return {};
    }

    std::optional<ASTNode::ExpressionFunctionCall> Parser::parseFunctionCallExpression(bool mustParse, const ASTNode::Expression& expr) {
        RY_PARSER__WRAP_PARSE_FUNC("function call", std::optional<ASTNode::ExpressionFunctionCall>, {
            auto optStructLit = parseStructLiteralExpression(mustParse);
            RY_PARSER__ASSERT(optStructLit);
            auto func = std::make_shared<ASTNode::Expression>(expr);
            auto params = optStructLit.value();
            return ASTNode::ExpressionFunctionCall(func, params);
        });
    }

    std::optional<ASTNode::ExpressionBlock> Parser::parseBlockExpression(bool mustParse) {
        using Block = ASTNode::ExpressionBlock;

        Block::Label label;
        if(auto token = getToken())
            if(auto string = token->GetLiteralValue<TokenLiteral::String>()) {
                label = *string;
                eatToken();
            }

        if(isToken('{')) {
            eatToken();
            Block::Statements statements;
            for(;;) {
                if(isToken('}')) {
                    eatToken();
                    break;
                }

                auto optStatement = parseStatement();
                if(optStatement.has_value())
                    statements.push_back(optStatement.value());

                if(isToken(';'))
                    eatToken();
                else if(!isToken('}')) {
                    errorExpectedToken(';');
                    break;
                }
            }
            return Block(label, statements);
        }

        if(mustParse)
            errorExpected("block");
        return {};
    }

    std::optional<ASTNode::ExpressionIf> Parser::parseIfExpression(bool mustParse) {
        if(isToken(Token::Code::KeywordIf)) {
            eatToken();

            auto optCondExpr = parseExpression();
            if(optCondExpr.has_value()) {
                if (expectToken(Token::Code::KeywordDo)) {
                    eatToken();
                    auto optThenExpr = parseExpression();
                    if(optThenExpr.has_value()) {
                        // else
                        ASTNode::ExpressionIf::FailExpression failExpr;
                        if(isToken(Token::Code::KeywordElse)) {
                            eatToken();
                            auto optFailExpr = parseExpression(false);
                            if(optFailExpr.has_value())
                                failExpr = std::make_shared<ASTNode::Expression>(optFailExpr.value());
                        }

                        auto condExpr = std::make_shared<ASTNode::Expression>(optCondExpr.value());
                        auto thenExpr = std::make_shared<ASTNode::Expression>(optThenExpr.value());
                        return ASTNode::ExpressionIf(condExpr, thenExpr, failExpr);
                    }
                }
            }
        }

        if(mustParse)
            errorExpected("if");
        return {};
    }

    std::optional<ASTNode::ExpressionLoop> Parser::parseLoopExpression(bool mustParse) {
    #define ASSERT(cond) if(cond) goto error;

        using Loop = ASTNode::ExpressionLoop;

        if(isToken(Token::Code::KeywordLoop)) {
            eatToken();

            Loop::InitStatement initStatement; {
                auto optInitStatement = parseStatement(false);
                if(optInitStatement.has_value())
                    initStatement = std::make_shared<ASTNode::Statement>(optInitStatement.value());
            }

            Loop::Condition condition;
            if(initStatement.has_value())
                if(isToken(';') || isToken(',')) {
                    eatToken();
                    auto optCondition = parseExpression();
                    ASSERT(optCondition.has_value());
                    condition = std::make_shared<ASTNode::Expression>(optCondition.value());
                }

            Loop::PostStatement postStatement;
            if(condition.has_value())
                if(isToken(';') || isToken(',')) {
                    eatToken();
                    auto optPostStatement = parseExpression();
                    ASSERT(optPostStatement.has_value());
                    postStatement = std::make_shared<ASTNode::Statement>(optPostStatement.value());
                }

            if(initStatement.has_value()) {
                ASSERT(expectToken(Token::Code::KeywordDo))
                eatToken();
            }

            auto optBodyStatement = parseExpression();
            ASSERT(optBodyStatement.has_value());
            Loop::BodyStatement bodyStatement = std::make_shared<ASTNode::Statement>(optBodyStatement.value());

            return ASTNode::ExpressionLoop(initStatement, condition, postStatement, bodyStatement);
        }

    #undef ASSERT
    error:
        if(mustParse)
            errorExpected("loop");
        return {};
    }

    std::optional<ASTNode::ExpressionName> Parser::parseNameExpression(bool mustParse) {
        if(auto token = getToken())
            if(auto name = token->GetName()) {
                eatToken();
                return *name;
            }

        if(mustParse)
            errorExpected("name");
        return {};
    }

    std::optional<ASTNode::ExpressionUnaryOperation> Parser::parseUnaryOperationExpression(bool mustParse) {
        using UnaryOp = ASTNode::ExpressionUnaryOperation;
        RY_PARSER__WRAP_PARSE_FUNC("unary operation", std::optional<UnaryOp>, {
            RY_PARSER__ASSERT(getToken());
            auto optUnaryKind = UnaryOp::GetTokenKindToUnaryKind(getToken()->GetKind());
            if(optUnaryKind.has_value()) {
                eatToken();

                auto optExpr = parseExpression(mustParse);
                RY_PARSER__ASSERT(optExpr);
                auto kind = optUnaryKind.value();
                auto operand = std::make_shared<ASTNode::Expression>(optExpr.value());
                return UnaryOp(kind, operand);
            }
        });
    }

    std::optional<ASTNode::ExpressionBinaryOperation> Parser::parseBinaryOperationExpression(
        bool mustParse,
        const ASTNode::Expression& expr1,
        std::optional<ASTNode::ExpressionBinaryOperation::Kind> currentBinOpKind
    ) {
        using BinOp = ASTNode::ExpressionBinaryOperation;
        RY_PARSER__WRAP_PARSE_FUNC("binary operation", std::optional<BinOp>, {
            RY_PARSER__ASSERT(getToken());
            auto optBinaryKind = BinOp::GetTokenKindToBinaryKind(getToken()->GetKind());
            RY_PARSER__ASSERT(optBinaryKind);
            auto binaryKind = optBinaryKind.value();
            eatToken();

            if(currentBinOpKind) {
                auto leftPriority = BinOp::GetKindPriority(currentBinOpKind.value());
                auto thisPriority = BinOp::GetKindPriority(binaryKind);
                RY_PARSER__ASSERT(thisPriority > leftPriority);
            }

            auto optExpr2 = parseExpression(mustParse, binaryKind);
            RY_PARSER__ASSERT(optExpr2);

            auto kind = optBinaryKind.value();
            auto operand1 = std::make_shared<ASTNode::Expression>(expr1);
            auto operand2 = std::make_shared<ASTNode::Expression>(optExpr2.value());
            auto binOp = BinOp(kind, operand1, operand2);
            auto expr = ASTNode::Expression(binOp);

            auto optBinOp2 = parseBinaryOperationExpression(false, expr);
            if(optBinOp2)
                return optBinOp2.value();

            return binOp;
        });
    }

    /*
     *
     * Statement
     *
     */

    std::optional<ASTNode::Statement> Parser::parseStatement(bool mustParse) {
    #define TRY_RETURN(OPT) { \
        if(auto opt = OPT) \
            return ASTNode::Statement(opt.value()); \
    }

        RY_PARSER__WRAP_PARSE_FUNC("statement", std::optional<ASTNode::Statement>, {
            TRY_RETURN(parseContinueStatement          (false));
            TRY_RETURN(parseBreakStatement             (false));
            TRY_RETURN(parseVariableDefinitionStatement(false));
            TRY_RETURN(parseAssignmentStatement        (false));
            TRY_RETURN(parseBinaryOperationStatement   (false));
            TRY_RETURN(parseExpression                 (false)); // last
        });
    
    #undef TRY_RETURN
    }

    std::optional<ASTNode::StatementBinaryOperation> Parser::parseBinaryOperationStatement(bool mustParse) {
        using BinOp = ASTNode::StatementBinaryOperation;
        RY_PARSER__WRAP_PARSE_FUNC("binary operation statement", std::optional<BinOp>, {
            auto optExpr1 = parseExpression(mustParse);
            RY_PARSER__ASSERT(optExpr1);
            auto optLValue1 = optExpr1->ToLValue();
            RY_PARSER__ASSERT(optLValue1);
            auto expr1 = optLValue1.value();

            RY_PARSER__ASSERT(getToken());
            auto optBinKind = BinOp::GetTokenKindToBinaryKind(getToken()->GetKind());
            RY_PARSER__ASSERT(optBinKind);
            eatToken();
            auto binKind = optBinKind.value();

            auto optExpr2 = parseExpression(mustParse);
            RY_PARSER__ASSERT(optExpr2);
            auto expr2 = optExpr2.value();

            return BinOp(binKind, {expr1, expr2});
        });
    }

    std::optional<ASTNode::StatementVariableDefinition> Parser::parseVariableDefinitionStatement(bool mustParse) {
        RY_PARSER__WRAP_PARSE_FUNC("variable definition", std::optional<ASTNode::StatementVariableDefinition>, {
            if(auto token = getToken())
            if(auto name = token->GetName()) {
                eatToken();
                if(isToken(Token::Code::Define)) {
                    eatToken();

                    auto optExpr = parseExpression(mustParse);
                    RY_PARSER__ASSERT(optExpr);
                    auto expr = optExpr.value();

                    return ASTNode::StatementUntypedVariableDefinition(*name, expr);
                }
                else {
                    auto optType = parseType(mustParse);
                    RY_PARSER__ASSERT(optType);
                    auto type = optType.value();

                    if(isToken('=')) {
                        eatToken();

                        auto optExpr = parseExpression(mustParse);
                        RY_PARSER__ASSERT(optExpr)
                        auto expr = optExpr.value();

                        return ASTNode::StatementTypedVariableDefinition(*name, type, expr);
                    }

                    return ASTNode::StatementTypedVariableDefinition(*name, type);
                }
            }
        });
    }

    std::optional<ASTNode::StatementAssignment> Parser::parseAssignmentStatement(bool mustParse) {
        RY_PARSER__WRAP_PARSE_FUNC("assignment", std::optional<ASTNode::StatementAssignment>, {
            auto optExpr1 = parseExpression(mustParse);
            RY_PARSER__ASSERT(optExpr1);
            auto optLValue1 = optExpr1->ToLValue();
            RY_PARSER__ASSERT(optLValue1);
            auto expr1 = optLValue1.value();

            RY_PARSER__ASSERT(isToken('='))
            eatToken();

            auto optExpr2 = parseExpression(mustParse);
            RY_PARSER__ASSERT(optExpr2);
            auto expr2 = optExpr2.value();

            return ASTNode::StatementAssignment(expr1, expr2);
        });
    }

    std::optional<ASTNode::StatementContinue> Parser::parseContinueStatement(bool mustParse) {
        RY_PARSER__WRAP_PARSE_FUNC("continue", std::optional<ASTNode::StatementContinue>, {
            if(isToken(Token::Code::KeywordContinue))
                return ASTNode::StatementContinue();
        });
    }

    std::optional<ASTNode::StatementBreak> Parser::parseBreakStatement(bool mustParse) {
        RY_PARSER__WRAP_PARSE_FUNC("break", std::optional<ASTNode::StatementBreak>, {
            RY_PARSER__ASSERT(isToken(Token::Code::KeywordBreak));
            eatToken();

            RY_PARSER__ASSERT(getToken());
            ASTNode::StatementBreak::Label label;
            if(auto optStringLiteral = getToken()->GetLiteralValue<TokenLiteral::String>()) {
                label = *optStringLiteral;
                eatToken();
            }

            auto optExpr = parseExpression(false);

            return ASTNode::StatementBreak(label, optExpr);
        });
    }

}