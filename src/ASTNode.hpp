#pragma once

#include "Token.hpp"
#include "ry.hpp"

#include <memory>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <set>

namespace ry {
    
    class ASTNode {
    private:
        using Name = std::string_view;
        using TK = Token::Type;

    public:
        class Expression;

        /*
         *
         * Type
         *
         */

        class Type;

        enum class TypePrimitive {
            Char = (int)TK::KW_CHAR,

            I8   = (int)TK::KW_I8,
            I16  = (int)TK::KW_I16,
            I32  = (int)TK::KW_I32,
            I64  = (int)TK::KW_I64,
            I128 = (int)TK::KW_I128,

            U8   = (int)TK::KW_U8,
            U16  = (int)TK::KW_U16,
            U32  = (int)TK::KW_U32,
            U64  = (int)TK::KW_U64,
            U128 = (int)TK::KW_U128,

            F32 = (int)TK::KW_F32,
            F64 = (int)TK::KW_F64,

            Bool = (int)TK::KW_BOOL
        };

        // 

        class TypeStruct {
        public:
            using FieldType = std::shared_ptr<Type>;
            using FieldDefaultValue = std::optional<std::shared_ptr<Expression>>;
            class NamedField {
            public:
                using Names = std::vector<Name>;

                NamedField(const Names& names, const FieldType& type, const FieldDefaultValue& defaultValue = {});

                const FieldType         & GetType         () const;
                const Names             & GetNames        () const;
                const FieldDefaultValue & GetDefaultValue () const;

            private:
                Names m_names;
                FieldType m_type;
                FieldDefaultValue m_defaultValue;
            };
            class UnnamedField {
            public:
                using TypeReps = std::optional<std::shared_ptr<Expression>>;

                UnnamedField(const FieldType& type, const TypeReps& typeReps = {}, const FieldDefaultValue& defaultValue = {});

                const FieldType         & GetType         () const;
                const TypeReps          & GetTypeReps     () const;
                const FieldDefaultValue & GetDefaultValue () const;

            private:
                FieldType m_type;
                TypeReps m_typeReps;
                FieldDefaultValue m_defaultValue;
            };

            using Field = std::variant<NamedField, UnnamedField>;
            using Fields = std::vector<Field>;

            TypeStruct();
            TypeStruct(const Fields& fields);

            const Fields& GetFields() const;

            std::string Stringify() const;

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

            std::string Stringify() const;

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

            Type(const Data& data, const std::optional<Attribs>& attribs = {});

            const Attribs& GetAttribs() const;
            const Data& Get() const;

            static const char * StringifyPrimitiveType(TypePrimitive primitiveType);
            std::string Stringify() const;

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

        class ExpressionLiteral {
        public:
            using Int = Token::intlit_t;
            using Float = Token::floatlit_t;
            using String = std::string;
            using Char = char;
            using Bool = bool;
            class Struct {
            public:
                class Field {
                public:
                    using FieldName = std::optional<Name>;
                    using Value = std::shared_ptr<Expression>;

                    Field(const Value& value);
                    Field(const Value& value, const FieldName& name = {});

                    const FieldName & GetName  () const;
                    const Value     & GetValue () const;

                    std::string Stringify() const;

                private:
                    FieldName m_name;
                    Value m_value;
                };
                using Fields = std::vector<Field>;

                Struct(const Fields& fields);

                const Fields& GetFields() const;

                std::string Stringify() const;

            private:
                Fields m_fields;
            };

            using Data = std::optional<std::variant<Int, Float, String, Char, Bool, Struct>>;

            ExpressionLiteral();
            ExpressionLiteral(const Data& data);

            const Data& Get() const;

            std::string Stringify() const;

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

            std::string Stringify() const;

        private:
            Function m_function;
            Parameters m_parameters;
        };

        // 

        class ExpressionBlock {
        public:
            using Label = std::optional<ExpressionLiteral::String>;
            using Statements = std::vector<Statement>;

            ExpressionBlock();
            ExpressionBlock(const Statements& statements);
            ExpressionBlock(const Label& label, const Statements& statements);

            const Label      & GetLabel      () const;
            const Statements & GetStatements () const;

            std::string Stringify() const;

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

            std::string Stringify() const;

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
            using BodyStatement = std::shared_ptr<Statement>;

            ExpressionLoop(
                const InitStatement& initStatement,
                const Condition& condition,
                const PostStatement& postStatement,
                const BodyStatement& bodyStatement
            );

            const InitStatement & GetInitStatement () const;
            const Condition     & GetCondition     () const;
            const PostStatement & GetPostStatement () const;
            const BodyStatement & GetBodyStatement () const;

            std::string Stringify() const;

        private:
            InitStatement m_initStatement;
            Condition m_condition;
            PostStatement m_postStatement;
            BodyStatement m_bodyStatement;
        };

        // 

        class ExpressionUnaryOperation {
        private:
            using TK = Token::Type;

        public:
            using Operand = std::shared_ptr<Expression>;

        #define RY_ASTNODE__UNARYOP_KINDS(E) /* E - expand macro */ \
            E(ArithmeticNegation, TK::OP_NEG)     \
            E(BitwiseNegation,    TK::OP_BIT_NEG) \
            E(LogicalNegation,    TK::OP_NOT)     \
            E(AddressOf,          TK::OP_ADDRESS) \
            E(Comp,               TK::OP_COMP)    \
            E(PointerDereference, TK::OP_PTR_DEREF)

            enum class Kind {
                RY_ASTNODE__UNARYOP_KINDS(RY_EXPAND_ENUM_VALUES)
            };

            static Token::Type GetKindToTokenType(Kind kind);
            static std::optional<Kind> GetTokenTypeToKind(Token::Type tokenType);

            ExpressionUnaryOperation(Kind kind, const Operand& operand);

            Kind GetKind() const;
            const Operand& GetOperand() const;

            static const char * StringifyKind(Kind kind);
            std::string Stringify() const;

        private:
            static const std::set<Token::Type> KINDS;

            Kind m_kind;
            Operand m_operand;
        };

        // 
        
        class ExpressionBinaryOperation {
        private:
            using TK = Token::Type;

        public:
            using Operand = ExpressionUnaryOperation::Operand;
            using Operands = std::pair<Operand, Operand>;

        #define RY_ASTNODE__BINOP_KINDS(E) /* E - expand macro */ \
            E(Add, TK::OP_ADD) \
            E(Sub, TK::OP_SUB) \
            E(Mul, TK::OP_MUL) \
            E(Div, TK::OP_DIV) \
            E(Mod, TK::OP_MOD) \
            \
            E(BitOr,     TK::OP_BIT_OR)  \
            E(BitXor,    TK::OP_BIT_XOR) \
            E(BitAnd,    TK::OP_BIT_AND) \
            E(BitLShift, TK::OP_BIT_LSHIFT) \
            E(BitRShift, TK::OP_BIT_RSHIFT) \
            \
            E(Eq,         TK::OP_EQ)   \
            E(Uneq,       TK::OP_UNEQ) \
            E(Less,       TK::OP_LESS) \
            E(LessEqual,  TK::OP_LESS_EQ)  \
            E(Great,      TK::OP_GREAT)    \
            E(GreatEqual, TK::OP_GREAT_EQ) \
            \
            E(Or,  TK::OP_OR)  \
            E(And, TK::OP_AND) \
            \
            E(TypeCast,           TK::OP_TYPE_CAST) \
            E(StructMemberAccess, TK::OP_STRUCT_ACCESS)

            enum class Kind {
                RY_ASTNODE__BINOP_KINDS(RY_EXPAND_ENUM_VALUES)
            };

            static Token::Type GetKindToTokenType(Kind kind);
            static std::optional<Kind> GetTokenTypeToKind(Token::Type tokenType);

            ExpressionBinaryOperation(Kind kind, const Operand& firstOperand, const Operand& secondOperand);

            Kind GetKind() const;
            const Operands& GetOperands() const;

            static const char * StringifyKind(Kind kind);
            std::string Stringify() const;

        private:
            static const std::set<Token::Type> KINDS;

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

                const Data& Get() const;

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

            const Data& Get() const;
            
            bool IsLValue() const;
            LValue ToLValue() const;

            std::string Stringify() const;

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

            const Data& Get() const;

            std::string Stringify() const;
    
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

        const Data& Get() const;

    private:
        Data m_data;
    };

}
