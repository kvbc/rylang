#include "Parser.hpp"
#include "Token.hpp"
#include "ry.hpp"

#include <format>
#include <memory>
#include <iostream>

namespace ry {

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

    std::optional<ASTNode::TypePrimitive> Parser::getTokenTypeToPrimitiveType(Token::Type tokenType) {
        int firstType = int(Token::Type::_KW_FIRST_TYPE) + 1;
        int lastType = int(Token::Type::_KW_LAST_TYPE) - 1;
        int type = int(tokenType);
        if(type >= firstType && type <= lastType)
            return ASTNode::TypePrimitive(type - firstType);
        return {};
    }

    std::optional<ASTNode::ExpressionUnaryOperation::Kind> Parser::getTokenTypeToUnaryOperationKind(Token::Type type) {
        using TT = Token::Type;
        using UK = ASTNode::ExpressionUnaryOperation::Kind;
        if(type == TT::OP_NEG)       return UK::ArithmeticNegation;
        if(type == TT::OP_BIT_NEG)   return UK::BitwiseNegation;
        if(type == TT::OP_NOT)       return UK::LogicalNegation;
        if(type == TT::OP_ADDRESS)   return UK::AddressOf;
        if(type == TT::OP_COMP)      return UK::Comp;
        if(type == TT::OP_PTR_DEREF) return UK::PointerDereference;
        return {};
    }

    std::optional<ASTNode::ExpressionBinaryOperation::Kind> Parser::getTokenTypeToBinaryOperationKind(Token::Type type) {
        using TT = Token::Type;
        using BK = ASTNode::ExpressionBinaryOperation::Kind;

        if(type == TT::OP_ADD) return BK::Add;
        if(type == TT::OP_SUB) return BK::Sub;
        if(type == TT::OP_MUL) return BK::Mul;
        if(type == TT::OP_DIV) return BK::Div;
        if(type == TT::OP_MOD) return BK::Mod;

        if(type == TT::OP_BIT_OR)     return BK::BitOr;
        if(type == TT::OP_BIT_XOR)    return BK::BitXor;
        if(type == TT::OP_BIT_AND)    return BK::BitAnd;
        if(type == TT::OP_BIT_LSHIFT) return BK::BitLShift;
        if(type == TT::OP_BIT_RSHIFT) return BK::BitRShift;

        if(type == TT::OP_EQ)       return BK::Eq;
        if(type == TT::OP_UNEQ)     return BK::Uneq;
        if(type == TT::OP_LESS)     return BK::Less;
        if(type == TT::OP_LESS_EQ)  return BK::LessEqual;
        if(type == TT::OP_GREAT)    return BK::Great;
        if(type == TT::OP_GREAT_EQ) return BK::GreatEqual;

        if(type == TT::OP_OR)  return BK::Or;
        if(type == TT::OP_AND) return BK::Or;

        if(type == TT::OP_TYPE_CAST)     return BK::TypeCast;
        if(type == TT::OP_STRUCT_ACCESS) return BK::StructMemberAccess;

        return {};
    }

    // 

    bool Parser::isToken(Token::Type expectedType, std::optional<Token::Type> orExpectedType) {
        Token::Type type = getToken().GetType();
        if(orExpectedType.has_value() && type == orExpectedType.value())
            return true;
        return expectedType == type;
    }

    bool Parser::expectToken(Token::Type type) {
        if(!isToken(type)) {
            errorExpectedToken(type);
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

    void Parser::errorExpectedToken(Token::Type type) {
        errorExpected(Token::Stringify(type));
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
        if(isToken(Token::Type::TILDE)) {
            eatToken();
            attribs.isMutable = true;
        }
        if(isToken(Token::Type::QUESTION)) {
            eatToken();
            attribs.isOptional = true;
        }

        // type

        if(isToken(Token::Type::ASTERISK)) {
            // pointer
            eatToken();
            auto optBaseType = parseType();
            RY_PARSER__ASSERT(optBaseType.has_value());
            ASTNode::Type baseType = optBaseType.value();
            ASTNode::TypePointer ptrType = std::make_shared<ASTNode::Type>(baseType);
            return ASTNode::Type(ptrType, attribs);
        }
        else if(auto primitiveType = getTokenTypeToPrimitiveType(getToken().GetType())) {
            // primitive
            eatToken();
            return ASTNode::Type(primitiveType.value(), attribs);
        }
        else if(isToken(Token::Type::LSQUARE)) {
            // struct or function
            eatToken();
            
            ASTNode::TypeStruct::Fields structFields;
            for(;;) {
                if(isToken(Token::Type::RSQUARE)) {
                    eatToken();
                    break;
                }

                auto optField = parseStructTypeField();
                RY_PARSER__ASSERT(optField.has_value());
                structFields.push_back(optField.value());

                bool isSep = isToken(Token::Type::COLON) || isToken(Token::Type::SEMICOLON);
                bool isEnd = isToken(Token::Type::RSQUARE);
                if(isSep)
                    eatToken();
                else if(!isEnd)
                    errorExpected("separator");
            }
            auto structType = ASTNode::TypeStruct(structFields);

            if(isToken(Token::Type::OP_FUNC_ARROW)) {
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

        auto tryParseTypeReps = [&]() -> UnnamedField::TypeReps {
            if(isToken(Token::Type::KW_TIMES)) {
                eatToken();
                auto optExpr = parseExpression();
                RY_PARSER__ASSERT(optExpr);
                return std::make_shared<ASTNode::Expression>(optExpr.value());
            }
            return {};
        };

        auto parseNames = [&](bool mustParse = true) -> std::optional<NamedField::Names> {
            if(!isToken(Token::Type::NAME)) {
                if(mustParse)
                    errorExpectedToken(Token::Type::NAME);
                return {};
            }
            NamedField::Names names;
            for(;;) {
                if(isToken(Token::Type::NAME)) {
                    names.push_back(getToken().GetStringValue());
                    eatToken();
                    if(isToken(Token::Type::COLON))
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
            if(isToken(Token::Type::OP_ASSIGN)) {
                eatToken();
                auto optExpr = parseExpression();
                RY_PARSER__ASSERT(optExpr.has_value());
                return std::make_shared<ASTNode::Expression>(optExpr.value());
            }
            return {};
        };

        auto tryParseUnnamedField = [&]() -> std::optional<UnnamedField> {
            auto optFieldType = parseFieldType();
            if(optFieldType.has_value()) {
                auto fieldType = optFieldType.value();

                UnnamedField::TypeReps typeReps;
                auto optTypeReps = tryParseTypeReps();
                if(optTypeReps)
                    typeReps = optTypeReps.value();

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
        auto opt = OPT; \
        if(opt.has_value()) \
            return ASTNode::Expression(opt.value()); \
    }

        auto tryParse = [&]() -> std::optional<ASTNode::Expression> {
            TRY_RETURN(parseUnaryOperationExpression(false));
            TRY_RETURN(parseNameExpression          (false));
            TRY_RETURN(parseLiteralExpression       (false));
            TRY_RETURN(parseBlockExpression         (false));
            TRY_RETURN(parseIfExpression            (false));
            TRY_RETURN(parseLoopExpression          (false));
            return {};
        };

        auto optExpr = tryParse();
        if(optExpr.has_value()) {
            auto optStructLit = parseStructLiteralExpression(false);
            if(optStructLit.has_value()) {
                auto func = std::make_shared<ASTNode::Expression>(optExpr.value());
                auto params = optStructLit.value();
                return ASTNode::Expression(
                    ASTNode::ExpressionFunctionCall(func, params)
                );
            }

            auto optBinOpKind = getTokenTypeToBinaryOperationKind(getToken().GetType());
            if(optBinOpKind.has_value()) {
                eatToken();

                auto optExpr2 = parseExpression();
                if(optExpr2.has_value()) {
                    auto kind = optBinOpKind.value();
                    auto firstOperand = std::make_shared<ASTNode::Expression>(optExpr.value());
                    auto secondOperand = std::make_shared<ASTNode::Expression>(optExpr2.value());
                    return ASTNode::Expression(
                        ASTNode::ExpressionBinaryOperation(kind, firstOperand, secondOperand)
                    );
                }
            }

            return optExpr.value();
        }

        if(mustParse)
            errorExpected("expression");
        return {};
    #undef TRY_RETURN
    }

    std::optional<ASTNode::ExpressionLiteral::Struct> Parser::parseStructLiteralExpression(bool mustParse) {
    #define ASSERT(cond)     { if(!cond) goto error; }
    #define ASSERT_RET(cond) { if(!cond) return {};  }

        using StructLiteral = ASTNode::ExpressionLiteral::Struct;

        if(isToken(Token::Type::LSQUARE)) {
            eatToken();

            auto parseField = [&]() -> std::optional<StructLiteral::Field> {
                StructLiteral::Field::FieldName name;
                if(isToken(Token::Type::NAME)) {
                    name = getToken().GetStringValue();
                    eatToken();
                    ASSERT_RET(expectToken(Token::Type::OP_ASSIGN));
                    eatToken();
                }
                auto optExpr = parseExpression();
                ASSERT_RET(optExpr.has_value());
                auto value = std::make_shared<ASTNode::Expression>(optExpr.value());
                return StructLiteral::Field(value, name);
            };

            StructLiteral::Fields fields;
            for(;;) {
                if(isToken(Token::Type::RSQUARE)) {
                    eatToken();
                    break;
                }

                auto optField = parseField();
                ASSERT(optField.has_value())
                fields.push_back(optField.value());

                bool isSep = isToken(Token::Type::COLON, Token::Type::SEMICOLON);
                bool isEnd = isToken(Token::Type::RSQUARE);
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

        auto optLitValue = getToken().GetLiteralValue();
        if(optLitValue.has_value()) {
            eatToken();
            Literal::Data litData = std::visit(overloaded{
                [](Token::intlit_t intValue)       -> Literal::Data { return intValue; },
                [](Token::floatlit_t floatValue)   -> Literal::Data { return floatValue; },
                [](char charValue)                 -> Literal::Data { return charValue; },
                [](const std::string& stringValue) -> Literal::Data { return stringValue; },
                [](bool boolValue)                 -> Literal::Data { return boolValue; },
                [](Token::NullValue)               -> Literal::Data { return {}; }
            }, optLitValue.value());
            return ASTNode::ExpressionLiteral(litData);
        }

        if(mustParse)
            errorExpected("literal");
        return {};
    }

    std::optional<ASTNode::ExpressionBlock> Parser::parseBlockExpression(bool mustParse) {
        using Block = ASTNode::ExpressionBlock;

        Block::Label label;
        if(isToken(Token::Type::STRING_LIT)) {
            label = getToken().GetStringValue();
            eatToken();
        }

        if(isToken(Token::Type::LCURLY)) {
            eatToken();
            Block::Statements statements;
            for(;;) {
                if(isToken(Token::Type::RCURLY)) {
                    eatToken();
                    break;
                }

                auto optStatement = parseStatement();
                if(optStatement.has_value())
                    statements.push_back(optStatement.value());

                if(isToken(Token::Type::SEMICOLON))
                    eatToken();
                else if(!isToken(Token::Type::RCURLY)) {
                    errorExpectedToken(Token::Type::SEMICOLON);
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
        if(isToken(Token::Type::KW_IF)) {
            eatToken();

            auto optCondExpr = parseExpression();
            if(optCondExpr.has_value()) {
                if (expectToken(Token::Type::KW_DO)) {
                    eatToken();
                    auto optThenExpr = parseExpression();
                    if(optThenExpr.has_value()) {
                        // else
                        ASTNode::ExpressionIf::FailExpression failExpr;
                        if(isToken(Token::Type::KW_ELSE)) {
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

        if(isToken(Token::Type::KW_LOOP)) {
            eatToken();

            Loop::InitStatement initStatement; {
                auto optInitStatement = parseStatement(false);
                if(optInitStatement.has_value())
                    initStatement = std::make_shared<ASTNode::Statement>(optInitStatement.value());
            }

            Loop::Condition condition;
            if(initStatement.has_value())
                if(isToken(Token::Type::SEMICOLON, Token::Type::COLON)) {
                    eatToken();
                    auto optCondition = parseExpression();
                    ASSERT(optCondition.has_value());
                    condition = std::make_shared<ASTNode::Expression>(optCondition.value());
                }

            Loop::PostStatement postStatement;
            if(condition.has_value())
                if(isToken(Token::Type::SEMICOLON, Token::Type::COLON)) {
                    eatToken();
                    auto optPostStatement = parseExpression();
                    ASSERT(optPostStatement.has_value());
                    postStatement = std::make_shared<ASTNode::Statement>(optPostStatement.value());
                }

            if(initStatement.has_value()) {
                ASSERT(expectToken(Token::Type::KW_DO))
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
        if(isToken(Token::Type::NAME)) {
            auto name = getToken().GetStringValue();
            eatToken();
            return name;
        }

        if(mustParse)
            errorExpected("name");
        return {};
    }

    std::optional<ASTNode::ExpressionUnaryOperation> Parser::parseUnaryOperationExpression(bool mustParse) {
        auto optUnaryKind = getTokenTypeToUnaryOperationKind(getToken().GetType());
        if(optUnaryKind.has_value()) {
            eatToken();

            auto optExpr = parseExpression();
            if(optExpr.has_value()) {
                auto kind = optUnaryKind.value();
                auto operand = std::make_shared<ASTNode::Expression>(optExpr.value());
                return ASTNode::ExpressionUnaryOperation(kind, operand);
            }
        }

        if(mustParse)
            errorExpected("unary operation");
        return {};
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