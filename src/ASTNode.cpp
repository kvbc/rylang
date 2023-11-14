#include "ASTNode.hpp"

#include <optional>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <assert.h>

namespace ry {
    
    /*
     *
     * Type
     *
     */

    using TypeStruct = ASTNode::TypeStruct;
    using Field = TypeStruct::Field;
    using Fields = TypeStruct::Fields;

    Field::Field(const Field::FieldType& type):
        m_type(type)
    {}

    Field::Field(const Field::FieldType& type, const Field::Names& names):
        m_type(type),
        m_names(names)
    {}

    TypeStruct::TypeStruct() {}

    TypeStruct::TypeStruct(const Fields& fields):
        m_fields(fields)
    {}

    const Fields& TypeStruct::GetFields() const {
        return m_fields;
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

    // 

    using Type = ASTNode::Type;

    Type::Type(const Data& data, const std::optional<Attribs>& attribs):
        m_attribs(attribs.value_or(m_attribs)),
        m_data(data)
    {}

    const Type::Attribs& Type::GetAttribs() const {
        return m_attribs;
    }

    const Type::Data& Type::GetData() const {
        return m_data;
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

    StructLit::Struct(const Fields& fields):
        m_fields(fields)
    {}

    const StructLit::Fields& StructLit::GetFields() const {
        return m_fields;
    }

    ExpressionLiteral::ExpressionLiteral() {}

    ExpressionLiteral::ExpressionLiteral(const Data& data):
        m_data(data)
    {}

    const ExpressionLiteral::Data& ExpressionLiteral::GetData() const {
        return m_data;
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

    // 

    using ExpressionBlock = ASTNode::ExpressionBlock;

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

    // 

    using ExpressionLoop = ASTNode::ExpressionLoop;

    ExpressionLoop::ExpressionLoop(const InitStatement& initStatement):
        m_initStatement(initStatement)
    {}

    ExpressionLoop::ExpressionLoop(
        const InitStatement& initStatement,
        const Condition& condition
    ):
        m_initStatement(initStatement),
        m_condition(condition)
    {}

    ExpressionLoop::ExpressionLoop(
        const InitStatement& initStatement,
        const Condition& condition,
        const PostStatement& postStatement
    ):
        m_initStatement(initStatement),
        m_condition(condition),
        m_postStatement(postStatement)
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

    // 

    using ExpressionUnaryOperation = ASTNode::ExpressionUnaryOperation;

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

    // 

    using ExpressionBinaryOperation = ASTNode::ExpressionBinaryOperation;

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

    // 

    using ExpressionName = ASTNode::ExpressionName;

    // 

    using Expression = ASTNode::Expression;
    using LValue = Expression::LValue;

    LValue::LValue(const Data& data):
        m_data(data)
    {}

    const LValue::Data& LValue::GetData() const {
        return m_data;
    }

    Expression::Expression(const Data& data):
        m_data(data)
    {}

    const Expression::Data& Expression::GetData() const {
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

    const Statement::Data& Statement::GetData() const {
        return m_data;
    }

    /*
     *
     * AST Node
     *
     */

    ASTNode::ASTNode(const Data& data):
        m_data(data)
    {}

    const ASTNode::Data& ASTNode::GetData() const {
        return m_data;
    }

}