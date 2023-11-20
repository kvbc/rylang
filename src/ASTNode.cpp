#include "ASTNode.hpp"
#include "src/ASTNode.hpp"

#include <optional>
#include <ratio>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <assert.h>
#include <utility>
#include <iostream>

namespace ry {
    
    std::string ASTNode::GetIndentString(std::size_t indent) {
        std::string str;
        for(; indent > 0; indent--) {
            str += "|   ";
        }
        return str;
    }

    /*
     *
     * Type
     *
     */

    const std::unordered_map<ASTNode::TypePrimitive, const char *> ASTNode::PRIMITIVE_TYPES_STRINGS_MAP = { 
        RY_ASTNODE__PRIMITIVE_TYPES(RY_ASTNODE__PRIMITIVE_TYPES_E_MAP)
    };

    const std::set<Token::NumericKind> ASTNode::PRIMITIVE_TYPES_TOKEN_KINDS = {
        RY_ASTNODE__PRIMITIVE_TYPES(RY_ASTNODE__PRIMITIVE_TYPES_E_VALUES)
    };

    // 

    using TypeStruct = ASTNode::TypeStruct;
    
        TypeStruct::NamedField::NamedField(const Names& names, const FieldType& type, const FieldDefaultValue& defaultValue):
            m_names(names),
            m_type(type),
            m_defaultValue(defaultValue)
        {}
        const TypeStruct::FieldType         & TypeStruct::NamedField::GetType         () const { return m_type;         }
        const TypeStruct::NamedField::Names & TypeStruct::NamedField::GetNames        () const { return m_names;        }
        const TypeStruct::FieldDefaultValue & TypeStruct::NamedField::GetDefaultValue () const { return m_defaultValue; }

        TypeStruct::UnnamedField::UnnamedField(const FieldType& type, const TypeReps& typeReps, const FieldDefaultValue& defaultValue):
            m_type(type),
            m_typeReps(typeReps),
            m_defaultValue(defaultValue)
        {}
        const TypeStruct::FieldType              & TypeStruct::UnnamedField::GetType         () const { return m_type;         }
        const TypeStruct::UnnamedField::TypeReps & TypeStruct::UnnamedField::GetTypeReps     () const { return m_typeReps;     }
        const TypeStruct::FieldDefaultValue      & TypeStruct::UnnamedField::GetDefaultValue () const { return m_defaultValue; }

    TypeStruct::TypeStruct() {}

    TypeStruct::TypeStruct(const Fields& fields):
        m_fields(fields)
    {}

    const TypeStruct::Fields& TypeStruct::GetFields() const {
        return m_fields;
    }

    std::string TypeStruct::StringifyField(const Field& field, std::size_t indent) const {
        auto stringifyDefaultValue = [&](const FieldDefaultValue& defaultValue, std::size_t indent) -> std::string {
            std::string str = "none";
            if(defaultValue)
                str = defaultValue.value()->Stringify(indent);
            return
                GetIndentString(indent)
                + "Default Value: "
                + str;
        };

        auto stringifyFieldType = [&](const FieldType& fieldType, std::size_t indent) -> std::string {
            return 
                GetIndentString(indent)
                + "Type: "
                + fieldType->Stringify(indent);
        };

        auto stringifyPretty = [&](const Field& field, std::size_t indent) -> std::string {
            return
                GetIndentString(indent)
                + "Pretty: "
                + StringifyFieldPretty(field);
        };

        return std::visit(overloaded{
            [&](const NamedField& namedField) -> std::string {
                std::string str = "NamedField";
                str += '\n';

                str +=
                    GetIndentString(indent + 1)
                    + "Names"
                    + '\n';
                for(auto name : namedField.GetNames())
                    str +=
                        GetIndentString(indent + 2)
                        + std::string(name)
                        + '\n';

                str += stringifyFieldType(namedField.GetType(), indent + 1);
                str += '\n';
                str += stringifyDefaultValue(namedField.GetDefaultValue(), indent + 1);
                str += '\n';
                str += stringifyPretty(namedField, indent + 1);

                return str;
            },
            [&](const UnnamedField& unnamedField) -> std::string {
                std::string str = "UnnamedField";
                str += '\n';

                std::string typeReps = "none";
                if(auto optTypeReps = unnamedField.GetTypeReps())
                    typeReps = optTypeReps.value()->Stringify(indent + 1);
                str +=
                    GetIndentString(indent + 1) + "TypeReps: "
                    + typeReps
                    + '\n';

                str += stringifyFieldType(unnamedField.GetType(), indent + 1);
                str += '\n';
                str += stringifyDefaultValue(unnamedField.GetDefaultValue(), indent + 1);
                str += '\n';
                str += stringifyPretty(unnamedField, indent + 1);

                return str;
            }
        }, field);
    }

    std::string TypeStruct::StringifyFieldPretty(const Field& field) const {
        auto stringifyDefaultValue = [&](const FieldDefaultValue& defaultValue) -> std::string {
            if(defaultValue.has_value())
                return " = " + defaultValue.value()->StringifyPretty();
            return "";
        };

        auto stringifyTypeReps = [&](const UnnamedField::TypeReps& typeReps) -> std::string {
            if(typeReps.has_value())
                return " * " + typeReps.value()->StringifyPretty();
            return "";
        };

        auto stringifyNames = [&](const NamedField::Names& names) -> std::string {
            std::string str;
            for(auto it = names.cbegin(); it != names.cend(); it++) {
                str += *it;
                if(it != names.cend() - 1)
                    str += ", ";
            }
            return str + ' ';
        };

        if(auto * unnamedField = std::get_if<UnnamedField>(&field))
            return
                unnamedField->GetType()->StringifyPretty()
                + stringifyTypeReps(unnamedField->GetTypeReps())
                + stringifyDefaultValue(unnamedField->GetDefaultValue());
        if(auto * namedField = std::get_if<NamedField>(&field))
            return
                stringifyNames(namedField->GetNames())
                + namedField->GetType()->StringifyPretty()
                + stringifyDefaultValue(namedField->GetDefaultValue());
        assert(false);
        return "???";
    }

    std::string TypeStruct::Stringify(std::size_t indent) const {
        std::string str = "TypeStruct";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Fields"
            + '\n';
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            auto field = *it;
            str +=
                GetIndentString(indent + 2)
                + StringifyField(field, indent + 2);
            if(it != m_fields.cend() - 1)
                str += '\n';
        }

        return str;
    }

    std::string TypeStruct::StringifyPretty() const {
        std::string str = "[";
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            str += StringifyFieldPretty(*it);
            if(it != m_fields.cend() - 1)
                str += "; ";
        }
        return str + ']';
    }

    // 

    using TypeFunction = ASTNode::TypeFunction;
    using ArgumentsType = TypeFunction::ArgumentsType;
    using ReturnType = TypeFunction::ReturnType;

    TypeFunction::TypeFunction(const ArgumentsType& argumentsType, const ReturnType& returnType):
        m_argumentsType(argumentsType),
        m_returnType(returnType)
    {}

    const ArgumentsType& TypeFunction::GetArgumentsType() const {
        return m_argumentsType;
    }

    const ReturnType& TypeFunction::GetReturnType() const {
        return m_returnType;
    }

    std::string TypeFunction::Stringify(std::size_t indent) const {
        std::string str = "TypeFunction";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Arguments: "
            + GetArgumentsType().Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "ReturnType: "
            + GetReturnType()->Stringify(indent + 1)
            + '\n';

        return str;
    }

    std::string TypeFunction::StringifyPretty() const {
        return m_argumentsType.StringifyPretty() + " => " + m_returnType->StringifyPretty();
    }

    // 

    using Type = ASTNode::Type;

    Type::Type(const Data& data, const std::optional<Attribs>& attribs):
        m_attribs(attribs.value_or(m_attribs)),
        m_data(data)
    {}

    const Type::Attribs& Type::GetAttribs() const {
        return m_attribs;
    }

    const Type::Data& Type::Get() const {
        return m_data;
    }

    const char * Type::StringifyPrimitiveType(TypePrimitive primitiveType) {
        return PRIMITIVE_TYPES_STRINGS_MAP.at(primitiveType);
    }

    std::string Type::Stringify(std::size_t indent) const {
        std::string str = "Type";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Attribs:"
            + '\n';
        str +=
            GetIndentString(indent + 2)
            + "Mutable: "
            + std::to_string(m_attribs.isMutable)
            + '\n';
        str +=
            GetIndentString(indent + 2)
            + "Optional: "
            + std::to_string(m_attribs.isOptional)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + std::visit(overloaded{
                [&](const TypePrimitive& primitive) -> std::string {
                    return std::string("TypePrimitive(") + StringifyPrimitiveType(primitive) + ')';
                },
                [&](const TypePointer& pointer) -> std::string {
                    return std::string("TypePointer -> ") + (*pointer).Stringify(indent + 1);
                },
                [&](const TypeFunction& function) -> std::string {
                    return function.Stringify(indent + 1);
                },
                [&](const TypeStruct& structType) -> std::string {
                    return structType.Stringify(indent + 1);
                }
            }, m_data);

        return str;
    }

    std::string Type::StringifyPretty() const {
        std::string str;

        if(m_attribs.isMutable)
            str += '~';
        if(m_attribs.isOptional)
            str += '?';

        return std::visit(overloaded{
            [&](const TypePrimitive& primitive) -> std::string {
                return str + StringifyPrimitiveType(primitive);
            },
            [&](const TypePointer& pointer) -> std::string {
                return str + "*" + (*pointer).StringifyPretty();
            },
            [&](const TypeFunction& function) -> std::string {
                return str + function.StringifyPretty();
            },
            [&](const TypeStruct& structType) -> std::string {
                return str + structType.StringifyPretty();
            }
        }, m_data);
    }

    std::optional<ASTNode::TypePrimitive> Type::GetTokenKindToPrimitiveType(const Token::Kind& tokenKind) {
        auto optNumericKind = Token::GetKindToNumericKind(tokenKind);
        if(!optNumericKind)
            return {};
        auto numericKind = optNumericKind.value();
        if(PRIMITIVE_TYPES_TOKEN_KINDS.contains(numericKind))
            return ASTNode::TypePrimitive(Token::GetNumericKindToInt(numericKind));
        return {};
    }

    /*
     *
     * Expression
     *
     */

    using ExpressionLiteral = ASTNode::ExpressionLiteral;
    using StructLit = ExpressionLiteral::Struct;
    using StructLitField = StructLit::Field;

    StructLitField::Field(const Value& value):
        m_value(value)
    {}

    StructLitField::Field(const Value& value, const FieldName& name):
        m_value(value),
        m_name(name)
    {}

    const StructLitField::FieldName& StructLitField::GetName() const {
        return m_name;
    }

    const StructLitField::Value& StructLitField::GetValue() const {
        return m_value;
    }

    std::string StructLitField::Stringify(std::size_t indent) const {
        std::string str = "StructLitField";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Name: "
            + std::string(m_name.value_or("none"))
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Value: "
            + m_value->Stringify(indent + 1);

        return str;
    }

    std::string StructLitField::StringifyPretty() const {
        std::string str;
        if(m_name.has_value()) {
            str += m_name.value();
            str += " = ";
        }
        str += m_value->StringifyPretty();
        return str;
    }

    StructLit::Struct(const Fields& fields):
        m_fields(fields)
    {}

    const StructLit::Fields& StructLit::GetFields() const {
        return m_fields;
    }

    std::string StructLit::Stringify(std::size_t indent) const {
        std::string str = "StructLit";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str += 
            GetIndentString(indent + 1)
            + "Fields: "
            + '\n';
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            auto field = *it;
            str +=
                GetIndentString(indent + 2)
                + field.Stringify(indent + 2);
            if(it != m_fields.cend() - 1)
                str += '\n';
        }

        return str;
    }

    std::string StructLit::StringifyPretty() const {
        std::string str = "[";
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            str += it->StringifyPretty();
            if(it != m_fields.cend() - 1)
                str += ", ";
        }
        return str + ']';
    }

    ExpressionLiteral::ExpressionLiteral() {}

    ExpressionLiteral::ExpressionLiteral(const Data& data):
        m_data(data)
    {}

    const ExpressionLiteral::Data& ExpressionLiteral::Get() const {
        return m_data;
    }

    std::string ExpressionLiteral::Stringify(std::size_t indent) const {
        std::string str = "ExprLit";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: ";
        if(!m_data.has_value())
            str += "NULL";
        else
            str += std::visit(overloaded{
                [&](Int intValue)              -> std::string { return "Int("    + StringifyPretty() + ')'; },
                [&](Float floatValue)          -> std::string { return "Float("  + StringifyPretty() + ')'; },
                [&](const String& stringValue) -> std::string { return "String(" + StringifyPretty() + ')'; },
                [&](Char charValue)            -> std::string { return "Char("   + StringifyPretty() + ')'; },
                [&](Bool boolValue)            -> std::string { return "Bool("   + StringifyPretty() + ')'; },
                [&](const Struct& structValue) -> std::string { return structValue.Stringify(indent + 1); }
            }, m_data.value());

        return str;
    }

    std::string ExpressionLiteral::StringifyPretty() const {
        if(!m_data.has_value())
            return "null";
        return std::visit(overloaded{
            [&](Int intValue)              -> std::string { return TokenLiteral::StringifyValue(intValue); },
            [&](Float floatValue)          -> std::string { return TokenLiteral::StringifyValue(floatValue); },
            [&](const String& stringValue) -> std::string { return TokenLiteral::StringifyValue(stringValue); },
            [&](Char charValue)            -> std::string { return TokenLiteral::StringifyValue(charValue); },
            [&](Bool boolValue)            -> std::string { return std::to_string(boolValue); },
            [&](const Struct& structValue) -> std::string { return structValue.StringifyPretty(); }
        }, m_data.value());
    }

    std::optional<ExpressionLiteral::Float> ExpressionLiteral::TryGetNumberValue() const {
        if(!m_data)
            return {};
        if(auto intValue = std::get_if<Int>(&m_data.value()))
            return *intValue;
        if(auto floatValue = std::get_if<Float>(&m_data.value()))
            return *floatValue;
        return {};
    }

    // 

    using ExpressionFunctionCall = ASTNode::ExpressionFunctionCall;

    ExpressionFunctionCall::ExpressionFunctionCall(const Function& function, const Parameters& parameters):
        m_function(function),
        m_parameters(parameters)
    {}

    const ExpressionFunctionCall::Function& ExpressionFunctionCall::GetFunction() const {
        return m_function;
    }

    const ExpressionFunctionCall::Parameters& ExpressionFunctionCall::GetParameters() const {
        return m_parameters;
    }

    std::string ExpressionFunctionCall::Stringify(std::size_t indent) const {
        std::string str = "ExprFuncCall";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Function: "
            + m_function->Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Parameters: "
            + m_parameters.Stringify(indent + 1);

        return str;
    }

    std::string ExpressionFunctionCall::StringifyPretty() const {
        return m_function->StringifyPretty() + m_parameters.StringifyPretty();
    }

    // 

    using ExpressionBlock = ASTNode::ExpressionBlock;

    ExpressionBlock::ExpressionBlock() {}

    ExpressionBlock::ExpressionBlock(const Statements& statements):
        m_statements(statements)
    {}

    ExpressionBlock::ExpressionBlock(const Label& label, const Statements& statements):
        m_label(label),
        m_statements(statements)
    {}

    const ExpressionBlock::Label& ExpressionBlock::GetLabel() const {
        return m_label;
    }

    const ExpressionBlock::Statements& ExpressionBlock::GetStatements() const {
        return m_statements;
    }

    std::string ExpressionBlock::Stringify(std::size_t indent) const {
        std::string str = "ExprBlock";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Label: "
            + m_label.value_or("none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Statements: ";
        for(auto stmt : m_statements) {
            str += '\n';
            str +=
                GetIndentString(indent + 2)
                + stmt.Stringify(indent + 2);
        }

        return str;
    }

    std::string ExpressionBlock::StringifyPretty() const {
        std::string str;

        if(m_label.has_value())
            str += ExpressionLiteral(m_label).StringifyPretty();

        str += '{';

        for(auto it = m_statements.cbegin(); it != m_statements.cend(); it++) {
            str += it->StringifyPretty();
            if(it != m_statements.cend() - 1)
                str += "; ";
        }

        str += '}';
        return str;
    }

    // 

    using ExpressionIf = ASTNode::ExpressionIf;

    ExpressionIf::ExpressionIf(
        const Condition& condition,
        const SuccessExpression& successExpression,
        const FailExpression& failExpression
    ):
        m_condition(condition),
        m_successExpression(successExpression),
        m_failExpression(failExpression)
    {}

    const ExpressionIf::Condition& ExpressionIf::GetCondition() const {
        return m_condition;
    }

    const ExpressionIf::SuccessExpression& ExpressionIf::GetSuccessExpression() const {
        return m_successExpression;
    }

    const ExpressionIf::FailExpression& ExpressionIf::GetFailExpression() const {
        return m_failExpression;
    }
    
    std::string ExpressionIf::Stringify(std::size_t indent) const {
        std::string str = "ExprIf";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str += 
            GetIndentString(indent + 1)
            + "Condition: "
            + m_condition->Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "SuccessExpr: "
            + m_successExpression->Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "FailExpr: "
            + (m_failExpression ? m_failExpression.value()->Stringify(indent + 1) : "none");

        return str;
    }

    std::string ExpressionIf::StringifyPretty() const {
        std::string str = "if ";
        str += m_condition->StringifyPretty();
        str += " do ";
        str += m_successExpression->StringifyPretty();
        if(m_failExpression.has_value()) {
            str += " else ";
            str += m_failExpression.value()->StringifyPretty();
        }
        return str;
    }

    // 

    using ExpressionLoop = ASTNode::ExpressionLoop;

    ExpressionLoop::ExpressionLoop(
        const InitStatement& initStatement,
        const Condition& condition,
        const PostStatement& postStatement,
        const BodyStatement& bodyStatement
    ):
        m_initStatement(initStatement),
        m_condition(condition),
        m_postStatement(postStatement),
        m_bodyStatement(bodyStatement)
    {}

    const ExpressionLoop::InitStatement& ExpressionLoop::GetInitStatement() const {
        return m_initStatement;
    }

    const ExpressionLoop::Condition& ExpressionLoop::GetCondition() const {
        return m_condition;
    }

    const ExpressionLoop::PostStatement& ExpressionLoop::GetPostStatement() const {
        return m_postStatement;
    }

    const ExpressionLoop::BodyStatement& ExpressionLoop::GetBodyStatement() const {
        return m_bodyStatement;
    }

    std::string ExpressionLoop::Stringify(std::size_t indent) const {
        std::string str = "ExprLoop";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Pretty: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "InitStatement: "
            + (m_initStatement ? m_initStatement.value()->Stringify(indent + 1) : "none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Condition: "
            + (m_condition ? m_condition.value()->Stringify(indent + 1) : "none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "PostStatement: "
            + (m_postStatement ? m_postStatement.value()->Stringify(indent + 1) : "none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "BodyStatement: "
            + m_bodyStatement->Stringify(indent + 1);

        return str;
    }

    std::string ExpressionLoop::StringifyPretty() const {
        std::string str = "loop ";
        if(m_initStatement.has_value()) {
            str += m_initStatement.value()->StringifyPretty();
            if(m_condition.has_value()) {
                str += "; ";
                str += m_condition.value()->StringifyPretty();
                if(m_postStatement.has_value()) {
                    str += "; ";
                    str += m_postStatement.value()->StringifyPretty();
                }
            }
            str += " do ";
        }
        str += m_bodyStatement->StringifyPretty();
        return str;
    } 

    // 

    using ExpressionUnaryOperation = ASTNode::ExpressionUnaryOperation;

    const std::set<Token::NumericKind> ExpressionUnaryOperation::TOKEN_KINDS {
        RY_ASTNODE__UNARYOP_KINDS(RY_ASTNODE__UNARYOP_KINDS_E_VALUES)
    };
    std::optional<ExpressionUnaryOperation::Kind> ExpressionUnaryOperation::GetTokenKindToUnaryKind(const Token::Kind& tokenKind) {
        auto optNumericKind = Token::GetKindToNumericKind(tokenKind);
        if(!optNumericKind.has_value())
            return {};
        auto numericKind = optNumericKind.value();
        if(TOKEN_KINDS.contains(numericKind))
            return static_cast<Kind>(Token::GetNumericKindToInt(numericKind));
        return {};
    }

    ExpressionUnaryOperation::ExpressionUnaryOperation(Kind kind, const Operand& operand):
        m_kind(kind),
        m_operand(operand)
    {}

    ExpressionUnaryOperation::Kind ExpressionUnaryOperation::GetKind() const {
        return m_kind;
    }

    const ExpressionUnaryOperation::Operand& ExpressionUnaryOperation::GetOperand() const {
        return m_operand;
    }

    std::string ExpressionUnaryOperation::StringifyKind(Kind kind) {
        static const std::unordered_map<Kind, const char *> nameMap = {
            RY_ASTNODE__UNARYOP_KINDS(RY_ASTNODE__UNARYOP_KINDS_E_NAME_MAP)
        };
        return nameMap.at(kind);
    }

    std::string ExpressionUnaryOperation::StringifyKindPretty(Kind kind) {
        auto optTokenNumericKind = Token::GetIntToNumericKind(int(kind));
        assert(optTokenNumericKind);
        return Token::StringifyNumericKind(optTokenNumericKind.value());
    }

    std::string ExpressionUnaryOperation::Stringify(std::size_t indent) const {
        return Stringify(m_kind, m_operand, indent);
    }

    std::string ExpressionUnaryOperation::StringifyPretty() const {
        return StringifyPretty(m_kind, m_operand);
    }

    std::optional<ExpressionLiteral::Float> ExpressionUnaryOperation::TryGetNumberValue() const {
        if(m_kind == Kind::ArithmeticNegation)
            if(auto optNumValue = m_operand->TryGetNumberValue())
                return - optNumValue.value();
        return {};
    }

    std::string ExpressionUnaryOperation::Stringify(Kind kind, const Operand& operand, std::size_t indent) {
        std::string str = "ExprUnaryOp";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty(kind, operand)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + StringifyKind(kind)
            + " ("
            + StringifyKindPretty(kind)
            + ')'
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Operand: "
            + operand->Stringify(indent + 1);

        return str;
    }

    std::string ExpressionUnaryOperation::StringifyPretty(Kind kind, const Operand& operand) {
        return StringifyKindPretty(kind) + operand->StringifyPretty();
    }

    // 

    using ExpressionBinaryOperation = ASTNode::ExpressionBinaryOperation;

    int ExpressionBinaryOperation::GetKindPriority(Kind kind) {
        static const std::unordered_map<Kind, int> priorityMap = {
            RY_ASTNODE__BINOP_KINDS(RY_ASTNODE__BINOP_KINDS_E_PRIORITY)
        };
        return priorityMap.at(kind);
    }

    std::optional<ExpressionBinaryOperation::Kind> ExpressionBinaryOperation::GetTokenKindToBinaryKind(const Token::Kind& tokenKind) {
        static const std::set<Token::NumericKind> tokenKinds {
            RY_ASTNODE__BINOP_KINDS(RY_ASTNODE__BINOP_KINDS_E_VALUES)
        };
        auto optNumericKind = Token::GetKindToNumericKind(tokenKind);
        if(!optNumericKind.has_value())
            return {};
        auto numericKind = optNumericKind.value();
        if(tokenKinds.contains(numericKind))
            return Kind(Token::GetNumericKindToInt(numericKind));
        return {};
    }

    ExpressionBinaryOperation::ExpressionBinaryOperation(
        Kind kind,
        const Operand& firstOperand,
        const Operand& secondOperand
    ):
        m_kind(kind),
        m_operands(firstOperand, secondOperand)
    {}

    ExpressionBinaryOperation::Kind ExpressionBinaryOperation::GetKind() const {
        return m_kind;
    }

    const ExpressionBinaryOperation::Operands& ExpressionBinaryOperation::GetOperands() const {
        return m_operands;
    }

    std::string ExpressionBinaryOperation::StringifyKind(Kind kind) {
        static const std::unordered_map<Kind, const char *> nameMap = {
            RY_ASTNODE__BINOP_KINDS(RY_ASTNODE__BINOP_KINDS_E_NAME_MAP)
        };
        return nameMap.at(kind);
    }

    std::string ExpressionBinaryOperation::StringifyKindPretty(Kind kind) {
        auto optTokenNumericKind = Token::GetIntToNumericKind(int(kind));
        assert(optTokenNumericKind);
        return Token::StringifyNumericKind(optTokenNumericKind.value());
    }

    std::string ExpressionBinaryOperation::Stringify(std::size_t indent) const {
        return Stringify(m_kind, m_operands, indent);
    }

    std::string ExpressionBinaryOperation::StringifyPretty() const {
        return StringifyPretty(m_kind, m_operands);
    }

    std::string ExpressionBinaryOperation::Stringify(Kind kind, const Operands& operands, std::size_t indent) {
        std::string str = "ExprBinOp";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty(kind, operands)
            + '\n';

        auto optNumValue = TryGetNumberValue(kind, operands);
        str +=
            GetIndentString(indent + 1)
            + "[InterpretedValue]: "
            + (optNumValue ? std::to_string(optNumValue.value()) : "none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + StringifyKind(kind)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "FirstOperand: "
            + operands.first->Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "SecondOperand: "
            + operands.second->Stringify(indent + 1);

        return str;
    }

    std::string ExpressionBinaryOperation::StringifyPretty(Kind kind, const Operands& operands) {
        return operands.first->StringifyPretty() + ' ' + StringifyKindPretty(kind) + ' ' + operands.second->StringifyPretty();
    }

    std::optional<ExpressionLiteral::Float> ExpressionBinaryOperation::TryGetNumberValue() const {
        return TryGetNumberValue(m_kind, m_operands);
    }

    std::optional<ExpressionLiteral::Float> ExpressionBinaryOperation::TryGetNumberValue(Kind kind, const Operands& operands) {
        auto optNum1 = operands.first->TryGetNumberValue();
        auto optNum2 = operands.second->TryGetNumberValue();
        if(optNum1 && optNum2) {
            auto a = optNum1.value();
            auto b = optNum2.value();
            using Int = ExpressionLiteral::Int;
            switch(kind) { 
                case Kind::Add : return a + b;
                case Kind::Sub : return a - b;
                case Kind::Div : return a / b;
                case Kind::Mul : return a * b;
                case Kind::Mod : return std::fmod(a, b);
                // 
                case Kind::BitOr     : return Int(a) | Int(b);
                case Kind::BitXor    : return Int(a) ^ Int(b);
                case Kind::BitAnd    : return Int(a) & Int(b);
                case Kind::BitLShift : return Int(a) << Int(b);
                case Kind::BitRShift : return Int(a) >> Int(b);
                // 
                case Kind::Eq         : return a == b;
                case Kind::Uneq       : return a != b;
                case Kind::Less       : return a < b;
                case Kind::LessEqual  : return a <= b;
                case Kind::Great      : return a > b;
                case Kind::GreatEqual : return a >= b;
                // 
                case Kind::Or  : return a || b;
                case Kind::And : return a && b;
                // 
                default:
                    return {};
            }
        }
        return {};
    }

    // 

    using ExpressionName = ASTNode::ExpressionName;

    // 

    using Expression = ASTNode::Expression;

    const Expression::Data& Expression::Get() const {
        return m_data;
    }

    bool Expression::IsGrouped() const {
        return m_isGrouped;
    }

    Expression::Expression(const Data& data, bool isGrouped):
        m_data(data),
        m_isGrouped(isGrouped)
    {}

    std::optional<Expression::LValue> Expression::ToLValue() const {
        if(auto name = std::get_if<ExpressionName>(&m_data))
            return *name;
        
        if(auto unaryOp = std::get_if<ExpressionUnaryOperation>(&m_data))
            if(unaryOp->GetKind() == ExpressionUnaryOperation::Kind::PointerDereference)
                return unaryOp->GetOperand();

        if(auto binOp = std::get_if<ExpressionBinaryOperation>(&m_data))
            if(binOp->GetKind() == ExpressionBinaryOperation::Kind::StructMemberAccess)
                return binOp->GetOperands();

        return {};
    }

    std::string Expression::Stringify(std::size_t indent) const {
        std::string str = "Expr";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        auto optNumValue = TryGetNumberValue();
        str +=
            GetIndentString(indent + 1)
            + "[InterpretedValue]: "
            + (optNumValue ? std::to_string(optNumValue.value()) : "none")
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "IsGrouped: "
            + std::to_string(m_isGrouped)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + std::visit(overloaded{
                [&](const ExpressionLiteral         & literal ) -> std::string { return literal .Stringify(indent + 1); },
                [&](const ExpressionFunctionCall    & funcCall) -> std::string { return funcCall.Stringify(indent + 1); },
                [&](const ExpressionBlock           & block   ) -> std::string { return block   .Stringify(indent + 1); },
                [&](const ExpressionIf              & ifExpr  ) -> std::string { return ifExpr  .Stringify(indent + 1); },
                [&](const ExpressionLoop            & loop    ) -> std::string { return loop    .Stringify(indent + 1); },
                [&](const ExpressionUnaryOperation  & unaryOp ) -> std::string { return unaryOp .Stringify(indent + 1); },
                [&](const ExpressionBinaryOperation & binOp   ) -> std::string { return binOp   .Stringify(indent + 1); },
                [&](const ExpressionName            & name    ) -> std::string { return "ExprName(" + std::string(name) + ')'; }
            }, m_data);

        return str;
    }

    std::string Expression::StringifyPretty() const {
        std::string str;
        if(m_isGrouped)
            str += '(';
        str += std::visit(overloaded{
            [](const ExpressionLiteral         & literal ){ return literal .StringifyPretty(); },
            [](const ExpressionFunctionCall    & funcCall){ return funcCall.StringifyPretty(); },
            [](const ExpressionBlock           & block   ){ return block   .StringifyPretty(); },
            [](const ExpressionIf              & ifExpr  ){ return ifExpr  .StringifyPretty(); },
            [](const ExpressionLoop            & loop    ){ return loop    .StringifyPretty(); },
            [](const ExpressionUnaryOperation  & unaryOp ){ return unaryOp .StringifyPretty(); },
            [](const ExpressionBinaryOperation & binOp   ){ return binOp   .StringifyPretty(); },
            [](const ExpressionName            & name    ){ return std::string(name); }
        }, m_data);
        if(m_isGrouped)
            str += ')';
        return str;
    }

    std::optional<ExpressionLiteral::Float> Expression::TryGetNumberValue() const {
        if(auto literal = std::get_if<ExpressionLiteral>(&m_data))
            return literal->TryGetNumberValue();
        if(auto unaryOp = std::get_if<ExpressionUnaryOperation>(&m_data))
            return unaryOp->TryGetNumberValue();
        if(auto binOp = std::get_if<ExpressionBinaryOperation>(&m_data))
            return binOp->TryGetNumberValue();
        return {};
    }

    std::string Expression::StringifyLValue(const LValue& lvalue, std::size_t indent) {
        return std::visit(overloaded{
            [&](const ExpressionName& name) {
                return std::string(name);
            },
            [&](const PointerDereference& operand) {
                return ExpressionUnaryOperation::Stringify(
                    ExpressionUnaryOperation::Kind::PointerDereference,
                    operand, indent
                );
            },
            [&](const StructMemberAccess& operands) {
                return ExpressionBinaryOperation::Stringify(
                    ExpressionBinaryOperation::Kind::StructMemberAccess,
                    operands, indent
                );
            }
        }, lvalue);
    }

    std::string Expression::StringifyLValuePretty(const LValue& lvalue) {
        return std::visit(overloaded{
            [&](const ExpressionName& name) {
                return std::string(name);
            },
            [&](const PointerDereference& operand) {
                return ExpressionUnaryOperation::StringifyPretty(
                    ExpressionUnaryOperation::Kind::PointerDereference,
                    operand
                );
            },
            [&](const StructMemberAccess& operands) {
                return ExpressionBinaryOperation::StringifyPretty(
                    ExpressionBinaryOperation::Kind::StructMemberAccess,
                    operands
                );
            }
        }, lvalue);
    }

    /*
     *
     * Statement
     *
     */

    using StatementExpression = ASTNode::Expression;

    // 

    using StatementBinaryOperation = ASTNode::StatementBinaryOperation;

    StatementBinaryOperation::StatementBinaryOperation(Kind kind, const Operands& operands):
        m_kind(kind),
        m_operands(operands)
    {}

    StatementBinaryOperation::Kind StatementBinaryOperation::GetKind() const {
        return m_kind;
    }

    const StatementBinaryOperation::Operands& StatementBinaryOperation::GetOperands() const {
        return m_operands;
    }

    std::string StatementBinaryOperation::StringifyKind(Kind kind) {
        static const std::unordered_map<Kind, const char *> nameMap = {
            RY_ASTNODE__STMTBINOP_KINDS(RY_ASTNODE__STMTBINOP_KINDS_E_NAME_MAP)
        };
        return nameMap.at(kind);
    }

    std::string StatementBinaryOperation::StringifyKindPretty(Kind kind) {
        auto optTokenNumericKind = Token::GetIntToNumericKind(int(kind));
        assert(optTokenNumericKind);
        return Token::StringifyNumericKind(optTokenNumericKind.value());
    }

    std::optional<StatementBinaryOperation::Kind> StatementBinaryOperation::GetTokenKindToBinaryKind(const Token::Kind& tokenKind) {
        static const std::set<Token::NumericKind> tokenKinds {
            RY_ASTNODE__STMTBINOP_KINDS(RY_ASTNODE__STMTBINOP_KINDS_E_VALUES)
        };
        auto optNumericKind = Token::GetKindToNumericKind(tokenKind);
        if(!optNumericKind.has_value())
            return {};
        auto numericKind = optNumericKind.value();
        if(tokenKinds.contains(numericKind))
            return Kind(Token::GetNumericKindToInt(numericKind));
        return {};
    }

    std::string StatementBinaryOperation::StringifyPretty() const {
        std::string str;

        str += Expression::StringifyLValue(m_operands.first);
        str += ' ' + StringifyKindPretty(m_kind) + ' ';
        str += m_operands.second.StringifyPretty();

        return str;
    }

    std::string StatementBinaryOperation::Stringify(std::size_t indent) const {
        std::string str = "StmtBinOp";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + StringifyKind(m_kind)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "FirstOperand: "
            + Expression::StringifyLValue(m_operands.first, indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "SecondOperand: "
            + m_operands.second.Stringify(indent + 1);

        return str;
    }

    // 

    using StatementTypedVariableDefinition = ASTNode::StatementTypedVariableDefinition;

    StatementTypedVariableDefinition::StatementTypedVariableDefinition(
        const VarName& varName,
        const VarType& varType,
        const VarValue& varValue
    ):
        m_varName(varName),
        m_varType(varType),
        m_varValue(varValue)
    {}

    const StatementTypedVariableDefinition::VarName  & StatementTypedVariableDefinition::GetName () const { return m_varName; }
    const StatementTypedVariableDefinition::VarType  & StatementTypedVariableDefinition::GetType () const { return m_varType; }
    const StatementTypedVariableDefinition::VarValue & StatementTypedVariableDefinition::GetValue() const { return m_varValue; }

    std::string StatementTypedVariableDefinition::StringifyPretty() const {
        std::string str;

        str += m_varName;
        str += ' ';
        str += m_varType.StringifyPretty();
        if(m_varValue) {
            str += " = ";
            str += m_varValue->StringifyPretty();
        }

        return str;
    }

    std::string StatementTypedVariableDefinition::Stringify(std::size_t indent) const {
        std::string str = "StmtTypedVarDef";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Name: "
            + std::string(m_varName)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Type: "
            + m_varType.Stringify(indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Value: "
            + (m_varValue ? m_varValue->Stringify(indent + 1) : "none");

        return str;
    }
    
    using StatementUntypedVariableDefinition = ASTNode::StatementUntypedVariableDefinition;

    StatementUntypedVariableDefinition::StatementUntypedVariableDefinition(
        const VarName& varName,
        const VarValue& varValue
    ):
        m_varName(varName),
        m_varValue(varValue)
    {}

    const StatementUntypedVariableDefinition::VarName  & StatementUntypedVariableDefinition::GetName () const { return m_varName; }
    const StatementUntypedVariableDefinition::VarValue & StatementUntypedVariableDefinition::GetValue() const { return m_varValue; }

    std::string StatementUntypedVariableDefinition::StringifyPretty() const {
        return std::string(m_varName) + " := " + m_varValue.StringifyPretty();
    }

    std::string StatementUntypedVariableDefinition::Stringify(std::size_t indent) const {
        std::string str = "StmtUntypedVarDef";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Name: "
            + std::string(m_varName)
            + '\n';

        str += 
            GetIndentString(indent + 1)
            + "Value: "
            + m_varValue.Stringify(indent + 1);

        return str;
    }

    // 

    using StatementAssignment = ASTNode::StatementAssignment;

    StatementAssignment::StatementAssignment(const LValue& lvalue, const RValue& rvalue):
        m_lvalue(lvalue),
        m_rvalue(rvalue)
    {}

    const StatementAssignment::LValue& StatementAssignment::GetLValue() const {
        return m_lvalue;
    }

    const StatementAssignment::RValue& StatementAssignment::GetRValue() const {
        return m_rvalue;
    }

    std::string StatementAssignment::StringifyPretty() const {
        return Expression::StringifyLValuePretty(m_lvalue) + " = " + m_rvalue.StringifyPretty();
    }

    std::string StatementAssignment::Stringify(std::size_t indent) const {
        std::string str = "StmtAssign";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "LValue: "
            + Expression::StringifyLValue(m_lvalue, indent + 1)
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "RValue: "
            + m_rvalue.Stringify(indent + 1);

        return str;
    }

    // 

    using StatementContinue = ASTNode::StatementContinue;

    std::string StatementContinue::StringifyPretty() {
        return "continue";
    }

    std::string StatementContinue::Stringify(std::size_t indent) {
        return "StmtContinue";
    }

    // 

    using StatementBreak = ASTNode::StatementBreak;

    StatementBreak::StatementBreak(const Label& label, const Value& value):
        m_label(label),
        m_value(value)
    {}

    const StatementBreak::Label& StatementBreak::GetLabel() const {
        return m_label;
    }

    const StatementBreak::Value& StatementBreak::GetValue() const {
        return m_value;
    }

    std::string StatementBreak::StringifyPretty() const {
        std::string str = "break";
        if(m_label) {
            str += " \"";
            str += m_label.value();
            str += '"';
        }
        if(m_value) {
            str += ' ';
            str += m_value->StringifyPretty();
        }
        return str;
    }

    std::string StatementBreak::Stringify(std::size_t indent) const {
        std::string str = "StmtBreak";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "[Pretty]: "
            + StringifyPretty()
            + '\n';

        str +=
            GetIndentString(indent + 1)
            + "Label: "
            + m_label.value_or("none")
            + '\n';

        str += 
            GetIndentString(indent + 1)
            + "Value: "
            + (m_value ? m_value->Stringify(indent + 1) : "none");

        return str;
    }

    // 

    using Statement = ASTNode::Statement;

    Statement::Statement(const Data& data):
        m_data(data)
    {}

    const Statement::Data& Statement::Get() const {
        return m_data;
    }

    std::string Statement::Stringify(std::size_t indent) const {
        std::string str = "Statement";
        str += '\n';

        str +=
            GetIndentString(indent + 1)
            + "Kind: "
            + std::visit(overloaded{
                [&](const StatementExpression& expr) {
                    return expr.Stringify(indent + 1);
                },
                [&](const StatementBinaryOperation& binOp) {
                    return binOp.Stringify(indent + 1);
                },
                [&](const StatementVariableDefinition& varDef) {
                    return std::visit(overloaded{
                        [&](const StatementTypedVariableDefinition& typedVarDef){ return typedVarDef.Stringify(indent + 1); },
                        [&](const StatementUntypedVariableDefinition& untypedVarDef){ return untypedVarDef.Stringify(indent + 1); }
                    }, varDef);
                },
                [&](const StatementAssignment& assign) {
                    return assign.Stringify(indent + 1);
                },
                [&](const StatementContinue& stmt) {
                    return stmt.Stringify(indent + 1);
                },
                [&](const StatementBreak& stmtBreak) {
                    return stmtBreak.Stringify(indent + 1);
                }
            }, m_data);

        return str;
    }

    std::string Statement::StringifyPretty() const {
        return std::visit(overloaded{
            [](const StatementExpression& expr) {
                return expr.StringifyPretty();
            },
            [](const StatementBinaryOperation& binOp) {
                return binOp.StringifyPretty();
            },
            [](const StatementVariableDefinition& varDef) {
                return std::visit(overloaded{
                    [](const StatementTypedVariableDefinition& typedVarDef){ return typedVarDef.StringifyPretty(); },
                    [](const StatementUntypedVariableDefinition& untypedVarDef){ return untypedVarDef.StringifyPretty(); }
                }, varDef);
            },
            [](const StatementAssignment& assign) {
                return assign.StringifyPretty();
            },
            [](const StatementContinue& stmt) {
                return stmt.StringifyPretty();
            },
            [](const StatementBreak& stmtBreak) {
                return stmtBreak.StringifyPretty();
            }
        }, m_data);
    }

    /*
     *
     * AST Node
     *
     */

    ASTNode::ASTNode(const Data& data):
        m_data(data)
    {}

    const ASTNode::Data& ASTNode::Get() const {
        return m_data;
    }

    std::string ASTNode::Stringify(std::size_t indent) const {
        return std::visit(overloaded{
            [&](const Type& type)       { return type.Stringify(indent); },
            [&](const Statement& stmt)  { return stmt.Stringify(indent); },
            [&](const Expression& expr) { return expr.Stringify(indent); }
        }, m_data);
    }

    std::string ASTNode::StringifyPretty() const {
        return std::visit(overloaded{
            [&](const Type& type)       { return type.StringifyPretty(); },
            [&](const Statement& stmt)  { return stmt.StringifyPretty(); },
            [&](const Expression& expr) { return expr.StringifyPretty(); }
        }, m_data);
    }

}