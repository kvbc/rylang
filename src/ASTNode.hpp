#pragma once

#include "Token.hpp"
#include "ry.hpp"
#include "src/ASTNode.hpp"

#include <memory>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <set>
#include <unordered_map>

namespace ry {
    
    class ASTNode {
    private:
        using Name = std::string_view;
        using TK = Token::Code;

        static std::string GetIndentString(std::size_t indent);

    public:
        class Expression;

        /*
         *
         * Type
         *
         */

        class Type;

    #define RY_ASTNODE__PRIMITIVE_TYPES_E_ENUM(NAME, VALUE) NAME = int(VALUE),
    #define RY_ASTNODE__PRIMITIVE_TYPES_E_MAP(NAME, _) { TypePrimitive::NAME, #NAME } ,
    #define RY_ASTNODE__PRIMITIVE_TYPES_E_VALUES(_, VALUE) Token::NumericKind(VALUE) ,
    #define RY_ASTNODE__PRIMITIVE_TYPES(E) /* E - expand macro */ \
        E(Char, TK::KeywordChar) \
        E(I8, TK::KeywordI8) \
        E(I16, TK::KeywordI16) \
        E(I32, TK::KeywordI32) \
        E(I64, TK::KeywordI64) \
        E(I128, TK::KeywordI128) \
        E(U8, TK::KeywordU8) \
        E(U16, TK::KeywordU16) \
        E(U32, TK::KeywordU32) \
        E(U64, TK::KeywordU64) \
        E(U128, TK::KeywordU128) \
        E(F32, TK::KeywordF32) \
        E(F64, TK::KeywordF64) \
        E(Bool, TK::KeywordBool)

        enum class TypePrimitive {
            RY_ASTNODE__PRIMITIVE_TYPES(RY_ASTNODE__PRIMITIVE_TYPES_E_ENUM)
        };
        static const std::unordered_map<TypePrimitive, const char *> PRIMITIVE_TYPES_STRINGS_MAP;
        static const std::set<Token::NumericKind> PRIMITIVE_TYPES_TOKEN_KINDS;

    #undef RY_ASTNODE__PRIMITIVE_TYPES_E_STR
    #undef RY_ASTNODE__PRIMITIVE_TYPES_E_ENUM

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

        private:
            std::string StringifyField(const Field& field, std::size_t indent) const;
            std::string StringifyFieldPretty(const Field& field) const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

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
            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

            static std::optional<TypePrimitive> GetTokenKindToPrimitiveType(const Token::Kind& tokenKind);

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
            using Int = TokenLiteral::Int;
            using Float = TokenLiteral::Float;
            using String = TokenLiteral::String;
            using Char = TokenLiteral::Char;
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

                    std::string Stringify(std::size_t indent = 0) const;
                    std::string StringifyPretty() const;

                private:
                    FieldName m_name;
                    Value m_value;
                };
                using Fields = std::vector<Field>;

                Struct(const Fields& fields);

                const Fields& GetFields() const;

                std::string Stringify(std::size_t indent = 0) const;
                std::string StringifyPretty() const;

            private:
                Fields m_fields;
            };

            using Data = std::optional<std::variant<Int, Float, String, Char, Bool, Struct>>;

            ExpressionLiteral();
            ExpressionLiteral(const Data& data);

            const Data& Get() const;

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

            std::optional<Float> TryGetNumberValue() const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

        private:
            InitStatement m_initStatement;
            Condition m_condition;
            PostStatement m_postStatement;
            BodyStatement m_bodyStatement;
        };

        // 

        class ExpressionUnaryOperation {
        public:
            using Operand = std::shared_ptr<Expression>;

        #define RY_ASTNODE__UNARYOP_KINDS_E_ENUM(NAME, VALUE) NAME = int(VALUE),
        #define RY_ASTNODE__UNARYOP_KINDS_E_VALUES(NAME, VALUE) Token::NumericKind(VALUE),
        #define RY_ASTNODE__UNARYOP_KINDS_E_NAME_MAP(NAME, VALUE) { Kind::NAME, #NAME } ,
        #define RY_ASTNODE__UNARYOP_KINDS(E) /* E - expand macro */ \
            E(ArithmeticNegation, '-') \
            E(BitwiseNegation,    '~') \
            E(LogicalNegation,    TK::KeywordNot) \
            E(AddressOf,          '&') \
            E(Comp,               TK::KeywordComp) \
            E(PointerDereference, '*')

            enum class Kind {
                RY_ASTNODE__UNARYOP_KINDS(RY_ASTNODE__UNARYOP_KINDS_E_ENUM)
            };

            static std::optional<Kind> GetTokenKindToUnaryKind(const Token::Kind& tokenKind);

            ExpressionUnaryOperation(Kind kind, const Operand& operand);

            Kind GetKind() const;
            const Operand& GetOperand() const;

            static std::string StringifyKind(Kind kind);
            static std::string StringifyKindPretty(Kind kind);
            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;
            static std::string Stringify(Kind kind, const Operand& operand, std::size_t indent = 0);
            static std::string StringifyPretty(Kind kind, const Operand& operand);

            std::optional<ExpressionLiteral::Float> TryGetNumberValue() const;

        private:
            static const std::set<Token::NumericKind> TOKEN_KINDS;

            Kind m_kind;
            Operand m_operand;
        };

        // 
        
        class ExpressionBinaryOperation {
        public:
            using Operand = ExpressionUnaryOperation::Operand;
            using Operands = std::pair<Operand, Operand>;

        #define RY_ASTNODE__BINOP_KINDS_E_ENUM(NAME, VALUE, _) NAME = int(VALUE),
        #define RY_ASTNODE__BINOP_KINDS_E_VALUES(NAME, VALUE, _) Token::NumericKind(VALUE),
        #define RY_ASTNODE__BINOP_KINDS_E_PRIORITY(NAME, _, PRIORITY) { Kind::NAME, PRIORITY } ,
        #define RY_ASTNODE__BINOP_KINDS_E_NAME_MAP(NAME, VALUE, _) { Kind::NAME, #NAME } ,
        #define RY_ASTNODE__BINOP_KINDS(E) /* E - expand macro */ \
            E(Add, '+', 8) \
            E(Sub, '-', 8) \
            E(Mul, '*', 9) \
            E(Div, '/', 9) \
            E(Mod, '%', 9) \
            \
            E(BitOr,     '|', 5)  \
            E(BitXor,    '^', 4) \
            E(BitAnd,    '&', 6) \
            E(BitLShift, TK::BitLShift, 7) \
            E(BitRShift, TK::BitRShift, 7) \
            \
            E(Eq,         TK::Equal, 3)   \
            E(Uneq,       TK::Unequal, 3) \
            E(Less,       '<', 3) \
            E(LessEqual,  TK::LessEqual, 3)  \
            E(Great,      '>', 3)    \
            E(GreatEqual, TK::GreatEqual, 3) \
            \
            E(Or,  TK::KeywordOr, 1)  \
            E(And, TK::KeywordAnd, 2) \
            \
            E(TypeCast,           TK::KeywordAs, 10) \
            E(StructMemberAccess, '.', 12)

            enum class Kind {
                RY_ASTNODE__BINOP_KINDS(RY_ASTNODE__BINOP_KINDS_E_ENUM)
            };

            static int GetKindPriority(Kind kind);
            static std::optional<Kind> GetTokenKindToBinaryKind(const Token::Kind& tokenKind);

            ExpressionBinaryOperation(Kind kind, const Operand& firstOperand, const Operand& secondOperand);

            Kind GetKind() const;
            const Operands& GetOperands() const;

            static std::string StringifyKind(Kind kind);
            static std::string StringifyKindPretty(Kind kind);
            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;
            static std::string Stringify(Kind kind, const Operands& operands, std::size_t indent = 0);
            static std::string StringifyPretty(Kind kind, const Operands& operands);

            std::optional<ExpressionLiteral::Float> TryGetNumberValue() const;
            static std::optional<ExpressionLiteral::Float> TryGetNumberValue(Kind kind, const Operands& operands);

        private:
            Kind m_kind;
            Operands m_operands;
        };

        // 

        using ExpressionName = std::string_view;

        // 

        class Expression {
        public:
            using PointerDereference = ExpressionUnaryOperation::Operand;
            using StructMemberAccess = ExpressionBinaryOperation::Operands;
            using LValue = std::variant<
                ExpressionName,
                PointerDereference,
                StructMemberAccess
            >;

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

            Expression(const Data& data, bool isGrouped = false);

            const Data& Get() const;
            bool IsGrouped() const;
            
            std::optional<LValue> ToLValue() const;

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;

            std::optional<ExpressionLiteral::Float> TryGetNumberValue() const;

            static std::string StringifyLValue(const LValue& lvalue, std::size_t indent = 0);
            static std::string StringifyLValuePretty(const LValue& lvalue);

        private:
            bool m_isGrouped; // ( ... )
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

        #define RY_ASTNODE__STMTBINOP_KINDS_E_ENUM(NAME, VALUE) NAME = int(VALUE),
        #define RY_ASTNODE__STMTBINOP_KINDS_E_VALUES(_, VALUE) Token::NumericKind(VALUE) ,
        #define RY_ASTNODE__STMTBINOP_KINDS_E_NAME_MAP(NAME, _) { Kind::NAME, #NAME } ,
        #define RY_ASTNODE__STMTBINOP_KINDS(E) \
            E(AddEq, TK::AddEqual) \
            E(SubEq, TK::SubEqual) \
            E(MulEq, TK::MulEqual) \
            E(DivEq, TK::DivEqual) \
            E(ModEq, TK::ModEqual) \
            \
            E(BitOrEq, TK::BitOrEqual) \
            E(BitXorEq, TK::BitXorEqual) \
            E(BitAndEq, TK::BitAndEqual) \
            E(BitLShiftEq, TK::BitLShiftEqual) \
            E(BitRShiftEq, TK::BitRShiftEqual)

            enum class Kind {
                RY_ASTNODE__STMTBINOP_KINDS(RY_ASTNODE__STMTBINOP_KINDS_E_ENUM)
            };

            static std::optional<Kind> GetTokenKindToBinaryKind(const Token::Kind& tokenKind);

            StatementBinaryOperation(Kind kind, const Operands& operands);

            
            Kind GetKind() const;
            const Operands & GetOperands() const;

            static std::string StringifyKind(Kind kind);
            static std::string StringifyKindPretty(Kind kind);
            std::string StringifyPretty() const;
            std::string Stringify(std::size_t indent = 0) const;

        private:
            Kind m_kind;
            Operands m_operands;
        };

        // 

        class StatementTypedVariableDefinition {
        public:
            using VarName  = ExpressionName;
            using VarType  = Type;
            using VarValue = std::optional<Expression>;

            StatementTypedVariableDefinition(const VarName& varName, const VarType& varType, const VarValue& varValue = {});

            const VarName  & GetName () const;
            const VarType  & GetType () const;
            const VarValue & GetValue() const;

            std::string StringifyPretty() const;
            std::string Stringify(std::size_t indent = 0) const;

        private:
            VarName m_varName;
            VarType m_varType;
            VarValue m_varValue;
        };

        class StatementUntypedVariableDefinition {
        public:
            using VarName  = ExpressionName;
            using VarValue = Expression;

            StatementUntypedVariableDefinition(const VarName& varName, const VarValue& varValue);

            const VarName  & GetName () const;
            const VarValue & GetValue() const;

            std::string StringifyPretty() const;
            std::string Stringify(std::size_t indent = 0) const;

        private:
            VarName m_varName;
            VarValue m_varValue;
        };

        using StatementVariableDefinition = std::variant<
            StatementTypedVariableDefinition,
            StatementUntypedVariableDefinition
        >;

        // 

        class StatementAssignment {
        public:
            using LValue = Expression::LValue;
            using RValue = Expression;

            StatementAssignment(const LValue& lvalue, const RValue& rvalue);

            const LValue & GetLValue() const;
            const RValue & GetRValue() const;

            std::string StringifyPretty() const;
            std::string Stringify(std::size_t indent = 0) const;

        private:
            LValue m_lvalue;
            RValue m_rvalue;
        };

        // 

        class StatementContinue {
        public:
            static std::string StringifyPretty();
            static std::string Stringify(std::size_t indent = 0);
        };

        // 

        class StatementBreak {
        public:
            using Label = ExpressionBlock::Label;
            using Value = std::optional<Expression>;

            StatementBreak(const Label& label, const Value& value);

            const Label& GetLabel() const;
            const Value& GetValue() const;

            std::string StringifyPretty() const;
            std::string Stringify(std::size_t indent = 0) const;

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

            std::string Stringify(std::size_t indent = 0) const;
            std::string StringifyPretty() const;
    
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

        std::string Stringify(std::size_t indent = 0) const;
        std::string StringifyPretty() const;

    private:
        Data m_data;
    };

}
