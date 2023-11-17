#include "ASTNode.hpp"

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <assert.h>
#include <utility>
#include <iostream>

namespace ry {
    
    /*
     *
     * Type
     *
     */

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

    std::string TypeStruct::Stringify() const {
        auto stringifyDefaultValue = [&](const FieldDefaultValue& defaultValue) -> std::string {
            if(defaultValue.has_value())
                return " = " + defaultValue.value()->Stringify();
            return "";
        };

        auto stringifyTypeReps = [&](const UnnamedField::TypeReps& typeReps) -> std::string {
            if(typeReps.has_value())
                return " times " + typeReps.value()->Stringify();
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

        auto stringifyField = [&](const Field& field) -> std::string {
            if(auto * unnamedField = std::get_if<UnnamedField>(&field))
                return
                    unnamedField->GetType()->Stringify()
                    + stringifyTypeReps(unnamedField->GetTypeReps())
                    + stringifyDefaultValue(unnamedField->GetDefaultValue());
            if(auto * namedField = std::get_if<NamedField>(&field))
                return
                    stringifyNames(namedField->GetNames())
                    + namedField->GetType()->Stringify()
                    + stringifyDefaultValue(namedField->GetDefaultValue());
            assert(false);
            return "???";
        };

        std::string str = "[";
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            str += stringifyField(*it);
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

    std::string TypeFunction::Stringify() const {
        return m_argumentsType.Stringify() + " => " + m_returnType->Stringify();
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
        return PRIMITIVE_TYPES_STRINGS[int(primitiveType)];
    }

    std::string Type::Stringify() const {
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
                return str + "*" + (*pointer).Stringify();
            },
            [&](const TypeFunction& function) -> std::string {
                return str + function.Stringify();
            },
            [&](const TypeStruct& structType) -> std::string {
                return str + structType.Stringify();
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

    std::string StructLitField::Stringify() const {
        std::string str;
        if(m_name.has_value()) {
            str += m_name.value();
            str += " = ";
        }
        str += m_value->Stringify();
        return str;
    }

    StructLit::Struct(const Fields& fields):
        m_fields(fields)
    {}

    const StructLit::Fields& StructLit::GetFields() const {
        return m_fields;
    }

    std::string StructLit::Stringify() const {
        std::string str = "[";
        for(auto it = m_fields.cbegin(); it != m_fields.cend(); it++) {
            str += it->Stringify();
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

    std::string ExpressionLiteral::Stringify() const {
        if(!m_data.has_value())
            return "null";
        return std::visit(overloaded{
            [&](Int intValue)              -> std::string { return TokenLiteral::StringifyValue(intValue); },
            [&](Float floatValue)          -> std::string { return TokenLiteral::StringifyValue(floatValue); },
            [&](const String& stringValue) -> std::string { return TokenLiteral::StringifyValue(stringValue); },
            [&](Char charValue)            -> std::string { return TokenLiteral::StringifyValue(charValue); },
            [&](Bool boolValue)            -> std::string { return std::to_string(boolValue); },
            [&](const Struct& structValue) -> std::string { return structValue.Stringify(); }
        }, m_data.value());
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

    std::string ExpressionFunctionCall::Stringify() const {
        return m_function->Stringify() + m_parameters.Stringify();
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

    std::string ExpressionBlock::Stringify() const {
        std::string str;

        if(m_label.has_value())
            str += ExpressionLiteral(m_label).Stringify();

        str += '{';

        for(auto it = m_statements.cbegin(); it != m_statements.cend(); it++) {
            str += it->Stringify();
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
    
    std::string ExpressionIf::Stringify() const {
        std::string str = "if ";
        str += m_condition->Stringify();
        str += " do ";
        str += m_successExpression->Stringify();
        if(m_failExpression.has_value()) {
            str += " else ";
            str += m_failExpression.value()->Stringify();
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

    std::string ExpressionLoop::Stringify() const {
        std::string str = "loop ";
        if(m_initStatement.has_value()) {
            str += m_initStatement.value()->Stringify();
            if(m_condition.has_value()) {
                str += "; ";
                str += m_condition.value()->Stringify();
                if(m_postStatement.has_value()) {
                    str += "; ";
                    str += m_postStatement.value()->Stringify();
                }
            }
            str += " do ";
        }
        str += m_bodyStatement->Stringify();
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
            return Kind(Token::GetNumericKindToInt(numericKind));
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

    const char * ExpressionUnaryOperation::StringifyKind(Kind kind) {
        return "???";
    }

    std::string ExpressionUnaryOperation::Stringify() const {
        return StringifyKind(m_kind) + m_operand->Stringify();
    }

    // 

    using ExpressionBinaryOperation = ASTNode::ExpressionBinaryOperation;

    const std::set<Token::NumericKind> ExpressionBinaryOperation::TOKEN_KINDS {
        RY_ASTNODE__BINOP_KINDS(RY_ASTNODE__BINOP_KINDS_E_VALUES)
    };
    std::optional<ExpressionBinaryOperation::Kind> ExpressionBinaryOperation::GetTokenKindToBinaryKind(const Token::Kind& tokenKind) {
        auto optNumericKind = Token::GetKindToNumericKind(tokenKind);
        if(!optNumericKind.has_value())
            return {};
        auto numericKind = optNumericKind.value();
        if(TOKEN_KINDS.contains(numericKind))
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

    const char * ExpressionBinaryOperation::StringifyKind(Kind kind) {
        return "???";
    }

    std::string ExpressionBinaryOperation::Stringify() const {
        return m_operands.first->Stringify() + ' ' + StringifyKind(m_kind) + ' ' + m_operands.second->Stringify();
    }

    // 

    using ExpressionName = ASTNode::ExpressionName;

    // 

    using Expression = ASTNode::Expression;
    using LValue = Expression::LValue;

    LValue::LValue(const Data& data):
        m_data(data)
    {}

    const LValue::Data& LValue::Get() const {
        return m_data;
    }

    Expression::Expression(const Data& data):
        m_data(data)
    {}

    const Expression::Data& Expression::Get() const {
        return m_data;
    }

    bool Expression::IsLValue() const {
        return std::holds_alternative<LValue::Name>(m_data)
            || std::holds_alternative<LValue::PointerDereference>(m_data)
            || std::holds_alternative<LValue::StructMemberAccess>(m_data);
    }

    LValue Expression::ToLValue() const {
        if(const LValue::Name * ptr = std::get_if<LValue::Name>(&m_data))
            return LValue(*ptr);
        if(const LValue::PointerDereference * ptr = std::get_if<LValue::PointerDereference>(&m_data))
            return LValue(*ptr);
        if(const LValue::StructMemberAccess * ptr = std::get_if<LValue::StructMemberAccess>(&m_data))
            return LValue(*ptr);
        throw std::invalid_argument("expression is not an lvalue");
    }

    std::string Expression::Stringify() const {
        return std::visit(overloaded{
            [](const ExpressionLiteral         & literal ){ return literal .Stringify(); },
            [](const ExpressionFunctionCall    & funcCall){ return funcCall.Stringify(); },
            [](const ExpressionBlock           & block   ){ return block   .Stringify(); },
            [](const ExpressionIf              & ifExpr  ){ return ifExpr  .Stringify(); },
            [](const ExpressionLoop            & loop    ){ return loop    .Stringify(); },
            [](const ExpressionUnaryOperation  & unaryOp ){ return unaryOp .Stringify(); },
            [](const ExpressionBinaryOperation & binOp   ){ return binOp   .Stringify(); },
            [](const ExpressionName            & name    ){ return std::string(name); }
        }, m_data);
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

    // 

    using StatementVariableDefinition = ASTNode::StatementVariableDefinition;

    StatementVariableDefinition::StatementVariableDefinition(
        const VarName& varName,
        const VarType& varType
    ):
        m_varName(varName),
        m_varType(varType)
    {}

    StatementVariableDefinition::StatementVariableDefinition(
        const VarName& varName,
        const VarValue& varValue
    ):
        m_varName(varName),
        m_varValue(varValue)
    {}

    StatementVariableDefinition::StatementVariableDefinition(
        const VarName& varName,
        const VarType& varType,
        const VarValue& varValue
    ):
        m_varName(varName),
        m_varType(varType),
        m_varValue(varValue)
    {}

    const StatementVariableDefinition::VarName& StatementVariableDefinition::GetName() const {
        return m_varName;
    }

    const StatementVariableDefinition::VarType& StatementVariableDefinition::GetType() const {
        return m_varType;
    }

    const StatementVariableDefinition::VarValue& StatementVariableDefinition::GetValue() const {
        return m_varValue;
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

    // 

    using StatementContinue = ASTNode::StatementContinue;

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

    // 

    using Statement = ASTNode::Statement;

    Statement::Statement(const Data& data):
        m_data(data)
    {}

    const Statement::Data& Statement::Get() const {
        return m_data;
    }

    std::string Statement::Stringify() const {
        return "???";
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

}