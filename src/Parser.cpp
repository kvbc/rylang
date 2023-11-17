#include "Parser.hpp"
#include "Token.hpp"
#include "ry.hpp"

#include <format>
#include <memory>
#include <iostream>
#include <optional>
#include <variant>

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
        auto optType = parseType(false);
        if(optType)
            return ASTNode(optType.value());

        auto optExpr = parseExpression(false);
        if(optExpr)
            return ASTNode(optExpr.value());

        return {};
    }

    // 

    template<typename T>
    bool Parser::isToken(
        const std::optional<T>& expectedKind,
        const std::optional<T>& orExpectedKind
    ) {
        auto kind = getToken().GetKind();
        if(Token::IsKind<T>(kind)) {
            if(expectedKind && Token::IsKindEqual(kind, expectedKind.value()))
                return true;
            return orExpectedKind && isToken(orExpectedKind);
        }
        return false;
    }

    bool Parser::isToken(
        const std::optional<Token::Kind>& expectedKind,
        const std::optional<Token::Kind>& orExpectedKind
    ) {
        auto kind = getToken().GetKind();
        if(expectedKind && Token::IsKindEqual(kind, expectedKind.value()))
            return true;
        return orExpectedKind && isToken(orExpectedKind);
    }

    bool Parser::expectToken(const Token::Kind& kind) {
        if(!isToken(kind)) {
            errorExpectedToken(kind);
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
                token.Stringify()
            ),
            token.GetSourcePosition()
        ));
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

        if(isToken('*')) {
            // pointer
            eatToken();
            auto optBaseType = parseType();
            RY_PARSER__ASSERT(optBaseType);
            ASTNode::Type baseType = optBaseType.value();
            ASTNode::TypePointer ptrType = std::make_shared<ASTNode::Type>(baseType);
            return ASTNode::Type(ptrType, attribs);
        }
        else if(auto optPrimitiveType = ASTNode::Type::GetTokenKindToPrimitiveType(getToken().GetKind())) {
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

                bool isSep = isToken(',', ';');
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
                if(auto nameToken = std::get_if<TokenName>(&getToken().GetKind())) {
                    names.push_back(*nameToken);
                    eatToken();
                    if(isToken(','))
                        eatToken();
                    else
                        break;
                }
                else break;
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

    std::optional<ASTNode::Expression> Parser::parseExpression(bool mustParse) {
    #define TRY_RETURN(OPT) { \
        if(auto opt = OPT) \
            return ASTNode::Expression(opt.value()); \
    }

        RY_PARSER__WRAP_PARSE_FUNC("expression", std::optional<ASTNode::Expression>, {
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
                TRY_RETURN(parseFunctionCallExpression   (false, optExpr.value()));
                TRY_RETURN(parseBinaryOperationExpression(false, optExpr.value()));
                return optExpr.value();
            }
        });

    #undef TRY_RETURN
    }

    std::optional<ASTNode::ExpressionLiteral::Struct> Parser::parseStructLiteralExpression(bool mustParse) {
    #define ASSERT(cond)     { if(!cond) goto error; }
    #define ASSERT_RET(cond) { if(!cond) return {};  }

        using StructLiteral = ASTNode::ExpressionLiteral::Struct;

        if(isToken('[')) {
            eatToken();

            auto parseField = [&]() -> std::optional<StructLiteral::Field> {
                StructLiteral::Field::FieldName name;
                if(auto nameToken = std::get_if<TokenName>(&getToken().GetKind())) {
                    name = *nameToken;
                    eatToken();
                    ASSERT_RET(expectToken('='));
                    eatToken();
                }
                auto optExpr = parseExpression();
                ASSERT_RET(optExpr.has_value());
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
                ASSERT(optField.has_value())
                fields.push_back(optField.value());

                bool isSep = isToken(',', ';');
                bool isEnd = isToken(']');
                if(isSep)
                    eatToken();
                else if(!isEnd)
                    errorExpected("separator");
            }
            return StructLiteral(fields);
        }

    error:
        if(mustParse)
            errorExpected("struct literal");
        return {};
    #undef ASSERT
    #undef ASSERT_RET
    }

    std::optional<ASTNode::ExpressionLiteral> Parser::parseLiteralExpression(bool mustParse) {
        using Literal = ASTNode::ExpressionLiteral;

        auto optStructLiteral = parseStructLiteralExpression(false);
        if(optStructLiteral.has_value()) {
            return ASTNode::ExpressionLiteral(optStructLiteral.value());
        }

        if(auto literal = std::get_if<TokenLiteral>(&getToken().GetKind())) {
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
        if(auto string = getToken().GetLiteralValue<TokenLiteral::String>()) {
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
                if(isToken(';', ',')) {
                    eatToken();
                    auto optCondition = parseExpression();
                    ASSERT(optCondition.has_value());
                    condition = std::make_shared<ASTNode::Expression>(optCondition.value());
                }

            Loop::PostStatement postStatement;
            if(condition.has_value())
                if(isToken(';', ',')) {
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
        if(auto name = getToken().GetName()) {
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
            auto optUnaryKind = UnaryOp::GetTokenKindToUnaryKind(getToken().GetKind());
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

    std::optional<ASTNode::ExpressionBinaryOperation> Parser::parseBinaryOperationExpression(bool mustParse, const ASTNode::Expression& expr1) {
        using BinOp = ASTNode::ExpressionBinaryOperation;
        RY_PARSER__WRAP_PARSE_FUNC("binary operation", std::optional<BinOp>, {
            auto optBinaryKind = BinOp::GetTokenKindToBinaryKind(getToken().GetKind());
            RY_PARSER__ASSERT(optBinaryKind);
            eatToken();

            auto optExpr2 = parseExpression(mustParse);
            RY_PARSER__ASSERT(optExpr2);

            auto kind = optBinaryKind.value();
            auto operand1 = std::make_shared<ASTNode::Expression>(expr1);
            auto operand2 = std::make_shared<ASTNode::Expression>(optExpr2.value());
            return BinOp(kind, operand1, operand2);
        });
    }

    /*
     *
     * Statement
     *
     */

    std::optional<ASTNode::Statement> Parser::parseStatement(bool mustParse) {
        if(mustParse)
            errorExpected("statement");
        return {};
    }

}