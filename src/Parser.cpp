#include "Parser.hpp"

#include <format>
#include <memory>
#include <iostream>

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

    bool Parser::isToken(Token::Type type, int offset) {
        return getToken(offset).GetType() == type;
    }

    bool Parser::assertToken(Token::Type type) {
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

    std::optional<ASTNode::Expression> Parser::parseExpression(bool mustParse) {
        return {};
    }

    std::optional<ASTNode::Type> Parser::parseType(bool mustParse) {
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
            
            ASTNode::TypeStruct::Fields structFields;
            for(;;) {
                if(isToken(Token::Type::RSQUARE)) {
                    eatToken();
                    break;
                }

                using TypeStruct = ASTNode::TypeStruct;
                using Field = TypeStruct::Field;
                using NamedField = TypeStruct::NamedField;
                using UnnamedField = TypeStruct::UnnamedField;

                enum class TypeRepsPos {Pre, Post};

                auto tryParseTypeReps = [&](TypeRepsPos pos) -> UnnamedField::TypeReps {
                    if(pos == TypeRepsPos::Post) {
                        if(isToken(Token::Type::OP_MUL))
                            eatToken();
                        else
                            return {};
                    }

                    auto optExpr = parseExpression(false);
                    if(optExpr.has_value()) {
                        if(pos == TypeRepsPos::Pre) {
                            assertToken(Token::Type::OP_MUL);
                            eatToken();
                        }
                        return std::make_shared<ASTNode::Expression>(optExpr.value());
                    }

                    return {};
                };

                auto parseNames = [&](bool mustParse = true) -> std::optional<NamedField::Names> {
                    NamedField::Names names;
                    if(!isToken(Token::Type::NAME)) {
                        if(mustParse)
                            errorExpectedToken(Token::Type::NAME);
                        return {};
                    }
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
                    if(optType.has_value())
                        return std::make_shared<ASTNode::Type>(optType.value());
                    return {};
                };

                auto tryParseDefaultValue = [&]() -> TypeStruct::FieldDefaultValue {
                    if(isToken(Token::Type::OP_ASSIGN)) {
                        eatToken();
                        auto optExpr = parseExpression();
                        if(optExpr.has_value())
                            return std::make_shared<ASTNode::Expression>(optExpr.value());
                        return {};
                    }
                    return {};
                };

                // either: typeReps, name, type
                auto optPreTypeReps = tryParseTypeReps(TypeRepsPos::Pre);
                if(!optPreTypeReps.has_value()) {
                    // typeReps ...
                    auto optFieldType = parseFieldType();
                    if(optFieldType.has_value()) {
                        auto fieldType = optFieldType.value();
                        auto optDefaultValue = tryParseDefaultValue();
                        UnnamedField field(fieldType, optPreTypeReps, optDefaultValue);
                        structFields.push_back(field);
                    }
                } else {
                    // either: name, type
                    auto optNames = parseNames(false);
                    if(optNames.has_value()) {
                        // name ...
                        auto names = optNames.value();
                        auto optFieldType = parseFieldType();
                        if(optFieldType.has_value()) {
                            auto fieldType = optFieldType.value();
                            auto optDefaultValue = tryParseDefaultValue();
                            NamedField field(names, fieldType, optDefaultValue);
                            structFields.push_back(field);
                        }
                    } else {
                        // type ...
                        auto optFieldType = parseFieldType();
                        if(optFieldType.has_value()) {
                            auto fieldType = optFieldType.value();
                            auto optPostTypeReps = tryParseTypeReps(TypeRepsPos::Post);
                            if(optPreTypeReps.has_value()) {
                                auto optDefaultValue = tryParseDefaultValue();
                                UnnamedField field(fieldType, optPreTypeReps, optDefaultValue);
                                structFields.push_back(field);
                            }
                        }
                    }
                }

                bool isSep = isToken(Token::Type::COLON) || isToken(Token::Type::SEMICOLON);
                bool isNextEnd = isToken(Token::Type::RSQUARE, 1);
                if(isSep)
                    eatToken();
                else if(!isNextEnd)
                    errorExpected("separator");
            }
            auto structType = ASTNode::TypeStruct(structFields);

            if(isToken(Token::Type::OP_FUNC_ARROW)) {
                // function
                eatToken();
                std::optional<ASTNode::Type> optRetType = parseType();
                if(optRetType.has_value()) {
                    auto retType = optRetType.value();
                    auto retTypePtr = std::make_shared<ASTNode::Type>(retType);
                    auto funcType = ASTNode::TypeFunction(structType, retTypePtr);
                    return ASTNode::Type(funcType, attribs);
                }
                return {};
            }

            // struct
            return ASTNode::Type(structType, attribs);
        }

        // not a type
        if(mustParse)
            errorExpected("type");
        return {};
    }

}