#pragma once

#include "Lexer.hpp"

#include <memory>
#include <optional>
#include <string_view>
#include <variant>

namespace ry {
    
    class ASTNode {
    private:
        using Name = std::string_view;

    public:

        /*
         *
         * Type
         *
         */

        class Type;

        enum class TypePrimitive {
            Char,
            I8, I16, I32, I64, I128,
            U8, U16, U32, U64, U128,
            F32, F64,
            Bool  
        };

        // 

        class TypeStruct {
        public:
            class Field {
            public:
                using FieldType = std::shared_ptr<Type>;
                using Names = std::optional<std::vector<Name>>;

                Field(const FieldType& type);
                Field(const FieldType& type, const Names& names);

                const FieldType & GetType  () const;
                const Names     & GetNames () const;

            private:
                Names m_names;
                FieldType m_type;
            };
            using Fields = std::optional<std::vector<Field>>;

            TypeStruct();
            TypeStruct(const Fields& fields);

            const Fields& GetFields() const;

        private:
            Fields m_fields;
        };

        // 

        class TypeFunction {
        public:
            using ArgumentsType = TypeStruct;
            using ReturnType = std::shared_ptr<Type>;

            TypeFunction(const ArgumentsType& argumentsType, const ReturnType& returnType);

            const ArgumentsType & GetArgumentsType () const;
            const ReturnType    & GetReturnType    () const;

        private:
            ArgumentsType m_argumentsType;
            ReturnType m_returnType;
        };

        // 

        using TypePointer = std::shared_ptr<Type>;

        // 

        class Type {
        public:
            struct Attribs {
                bool isMutable = false;
                bool isOptional = false;
            };
            using Data = std::variant<TypePrimitive, TypeFunction, TypeStruct, TypePointer>;

            Type(const Data& data, const std::optional<Attribs>& attribs);

            const Attribs& GetAttribs() const;
            const Data& GetData() const;

        private:
            const Attribs m_attribs;
            Data m_data;
        };

        /*
         *
         * Expression
         *
         */

        class Statement;
        class Expression;

        class ExpressionLiteral {
        public:
            using Int = Lexer::intlit_t;
            using Float = Lexer::floatlit_t;
            using String = std::string_view;
            using Char = char;
            using Bool = bool;
            class Struct {
            public:
                class Field {
                public:
                    using FieldName = std::optional<Name>;
                    using Value = std::shared_ptr<Expression>;

                    Field(const Value& value);
                    Field(const Value& value, const FieldName& name);

                    const FieldName & GetName  () const;
                    const Value     & GetValue () const;

                private:
                    FieldName m_name;
                    Value m_value;
                };
                using Fields = std::vector<Field>;

                Struct(const Fields& fields);

                const Fields& GetFields() const;

            private:
                Fields m_fields;
            };

            using Data = std::optional<std::variant<Int, Float, String, Char, Bool, Struct>>;

            ExpressionLiteral();
            ExpressionLiteral(const Data& data);

            const Data& GetData() const;

        private:
            Data m_data;
        };

        // 

        class ExpressionFunctionCall {
        public:
            using Function = std::shared_ptr<Expression>;
            using Parameters = ExpressionLiteral::Struct;

            ExpressionFunctionCall(const Function& function, const Parameters& parameters);

            const Function   & GetFunction   () const;
            const Parameters & GetParameters () const;

        private:
            Function m_function;
            Parameters m_parameters;
        };

        // 

        class ExpressionBlock {
        public:
            using Label = std::optional<ExpressionLiteral::String>;
            using Statements = std::vector<Statement>;

            ExpressionBlock(const Label& label, const Statements& statements);

            const Label      & GetLabel      () const;
            const Statements & GetStatements () const;

        private:
            Label m_label;
            Statements m_statements;
        };

        // 

        class ExpressionIf {
        public:
            using Condition = std::shared_ptr<Expression>;
            using SuccessExpression = std::shared_ptr<Expression>;
            using FailExpression = std::optional<std::shared_ptr<Expression>>;

            ExpressionIf(
                const Condition& condition,
                const SuccessExpression& successExpression,
                const FailExpression& failExpression
            );

            const Condition         & GetCondition         () const;
            const SuccessExpression & GetSuccessExpression () const;
            const FailExpression    & GetFailExpression    () const;

        private:
            Condition m_condition;
            SuccessExpression m_successExpression;
            FailExpression m_failExpression;
        };

        // 

        class ExpressionLoop {
        public:
            using InitStatement = std::optional<std::shared_ptr<Statement>>;
            using Condition = std::optional<std::shared_ptr<Expression>>;
            using PostStatement = InitStatement;

            ExpressionLoop(const InitStatement& initStatement);
            ExpressionLoop(const InitStatement& initStatement, const Condition& condition);
            ExpressionLoop(const InitStatement& initStatement, const Condition& condition, const PostStatement& postStatement);

            const InitStatement & GetInitStatement () const;
            const Condition     & GetCondition     () const;
            const PostStatement & GetPostStatement () const;

        private:
            InitStatement m_initStatement;
            Condition m_condition;
            PostStatement m_postStatement;
        };

        // 

        class ExpressionUnaryOperation {
        public:
            using Operand = std::shared_ptr<Expression>;

            enum class Kind {
                ArithmeticNegation,
                BitwiseNegation,
                LogicalNegation,
                AddressOf,
                Comp,
                PointerDereference
            };

            ExpressionUnaryOperation(Kind kind, const Operand& operand);

            Kind GetKind() const;
            const Operand& GetOperand() const;

        private:
            Kind m_kind;
            Operand m_operand;
        };

        // 
        
        class ExpressionBinaryOperation {
        public:
            using Operand = ExpressionUnaryOperation::Operand;
            using Operands = std::pair<Operand, Operand>;

            enum class Kind {
                Add, Sub, Mul, Power, Div, Mod,
                BitOr, BitXor, BitAnd, BitLShift, BitRShift,
                Eq, Uneq, Less, LessEqual, Great, GreatEqual,
                Or, And,
                TypeCast, StructMemberAccess
            };

            ExpressionBinaryOperation(Kind kind, const Operand& firstOperand, const Operand& secondOperand);

            Kind GetKind() const;
            const Operands& GetOperands() const;

        private:
            Kind m_kind;
            Operands m_operands;
        };

        // 

        using ExpressionName = std::string_view;

        // 

        class Expression {
        public:
            class LValue {
            public:
                using Name               = ExpressionName;
                using PointerDereference = ExpressionUnaryOperation;
                using StructMemberAccess = ExpressionBinaryOperation;

                using Data = std::variant<Name, PointerDereference, StructMemberAccess>;

                LValue(const Data& data);

                const Data& GetData() const;

            private:
                Data m_data;
            };

            using Data = std::variant<
                ExpressionLiteral,
                ExpressionFunctionCall,
                ExpressionBlock,
                ExpressionIf,
                ExpressionLoop,
                ExpressionUnaryOperation,
                ExpressionBinaryOperation,
                ExpressionName
            >;

            Expression(const Data& data);

            const Data& GetData() const;
            
            bool IsLValue() const;
            LValue ToLValue() const;

        private:
            Data m_data;
        };

        /*
         *
         * Statement
         *
         */

        using StatementExpression = Expression;

        // 

        class StatementBinaryOperation {
        public:
            using Operands = std::pair<Expression::LValue, Expression>;

            enum class Kind {
                AddEq, SubEq, MulEq, PowerEq, DivEq, ModEq,
                BitOrEq, BitXorEq, BitAndEq, BitLShiftEq, BitRShiftEq
            };

            StatementBinaryOperation(Kind kind, const Operands& operands);

            Kind GetKind() const;
            const Operands & GetOperands() const;

        private:
            Kind m_kind;
            Operands m_operands;
        };

        // 

        class StatementVariableDefinition {
        public:
            using VarName  = ExpressionName;
            using VarType  = std::optional<std::shared_ptr<Type>>;
            using VarValue = std::optional<std::shared_ptr<Expression>>;

            StatementVariableDefinition(const VarName& varName, const VarType& varType); // an optional type
            StatementVariableDefinition(const VarName& varName, const VarValue& varValue); // infer type
            StatementVariableDefinition(const VarName& varName, const VarType& varType, const VarValue& varValue);

            const VarName  & GetName () const;
            const VarType  & GetType () const;
            const VarValue & GetValue() const;

        private:
            VarName m_varName;
            VarType m_varType;
            VarValue m_varValue;
        };

        // 

        class StatementAssignment {
        public:
            using LValue = Expression::LValue;
            using RValue = Expression;

            StatementAssignment(const LValue& lvalue, const RValue& rvalue);

            const LValue & GetLValue() const;
            const RValue & GetRValue() const;

        private:
            LValue m_lvalue;
            RValue m_rvalue;
        };

        // 

        using StatementContinue = struct {};

        // 

        class StatementBreak {
        public:
            using Label = ExpressionBlock::Label;
            using Value = Expression;

            StatementBreak(const Label& label, const Value& value);

            const Label& GetLabel() const;
            const Value& GetValue() const;

        private:
            Label m_label;
            Value m_value;
        };

        // 

        class Statement {
        public:
            using Data = std::variant<
                StatementExpression,
                StatementBinaryOperation,
                StatementVariableDefinition,
                StatementAssignment,
                StatementContinue,
                StatementBreak
            >;

            Statement(const Data& data);

            const Data& GetData() const;
    
        private:
            Data m_data;
        };

        /*
         *
         * AST Node
         *
         */
        
    public:
        using Data = std::variant<Type, Expression, Statement>;

        ASTNode(const Data& data);

        const Data& GetData() const;

    private:
        Data m_data;
    };

}
