#pragma once

#include "Lexer.hpp"

#include <memory>
#include <optional>
#include <string_view>

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
                using FieldType = std::unique_ptr<Type>;
                using Names = std::vector<Name>;

                Field(const FieldType& type);
                Field(const FieldType& type, const Names& names);

                const FieldType & GetType  () const;
                const Names     & GetNames () const;

            private:
                Names m_names;
                FieldType m_type;
            };
            using Fields = std::vector<Field>;

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
            using ReturnType = std::unique_ptr<Type>;

            TypeFunction(const ArgumentsType& argumentsType, ReturnType returnType);

            const ArgumentsType & GetArgumentsType () const;
            const ReturnType    & GetReturnType    () const;

        private:
            ArgumentsType argumentsType;
            ReturnType returnType;
        };

        // 

        using TypePointer = std::unique_ptr<Type>;

        // 

        class Type {
        public:
            struct Attribs {
                bool isMutable;
                bool isOptional;
            };

            enum class Kind {
                Primitive,
                Function,
                Struct,
                Pointer
            };

            Type(const TypePrimitive & primitive);
            Type(const TypeFunction  & function);
            Type(const TypeStruct    & structType);
            Type(const TypePointer   & pointer);

            Kind GetKind() const;
            const Attribs& GetAttribs() const;
            
            const TypePrimitive & GetPrimitive () const;
            const TypeFunction  & GetFunction  () const;
            const TypeStruct    & GetStruct    () const;
            const TypePointer   & GetPointer   () const;

        private:
            Kind m_kind;
            Attribs m_attribs;
            union {
                TypePrimitive primitive;
                TypeFunction  function;
                TypeStruct    structType;
                TypePointer   pointer;
            } m_data;
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
            enum class Kind {
                Int, Float, String, Char, Bool, Null
            };

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
                    using Value = std::unique_ptr<Expression>;

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

            private:
                Fields m_fields;
            };

            ExpressionLiteral(const Int    & intv   );
            ExpressionLiteral(const Float  & floatv );
            ExpressionLiteral(const String & stringv);
            ExpressionLiteral(const Char   & charv  );
            ExpressionLiteral(const Bool   & boolv  );
            ExpressionLiteral(const Struct & structv);
            ExpressionLiteral();

            Kind GetKind() const;

            const Int    & GetInt    () const;
            const Float  & GetFloat  () const;
            const String & GetString () const;
            const Char   & GetChar   () const;
            const Bool   & GetBool   () const;
            const Struct & GetStruct () const;

        private:
            Kind m_kind;
            union {
                Int intv;
                Float floatv;
                String stringv;
                Char charv;
                Bool boolv;
                Struct structv;
            } m_data;
        };

        // 

        class ExpressionFunctionCall {
        public:
            using Function = std::unique_ptr<Expression>;
            using Parameters = ExpressionLiteral::Struct;

            ExpressionFunctionCall(Function function, const Parameters& parameters);

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

            ExpressionBlock(Label label, const Statements& statements);

            const Label      & GetLabel      () const;
            const Statements & GetStatements () const;

        private:
            Label m_label;
            Statements m_statements;
        };

        // 

        class ExpressionIf {
        public:
            using Condition = std::unique_ptr<Expression>;
            using SuccessExpression = std::unique_ptr<Expression>;
            using FailExpression = std::optional<std::unique_ptr<Expression>>;

            ExpressionIf(Condition condition, SuccessExpression successExpression, FailExpression failExpression);

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
            using InitStatement = std::optional<std::unique_ptr<Statement>>;
            using Condition = std::optional<std::unique_ptr<Expression>>;
            using PostStatement = InitStatement;

            ExpressionLoop(InitStatement initStatement);
            ExpressionLoop(InitStatement initStatement, Condition condition);
            ExpressionLoop(InitStatement initStatement, Condition condition, PostStatement postStatement);

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
            using Operand = std::unique_ptr<Expression>;

            enum class Kind {
                ArithmeticNegation,
                BitwiseNegation,
                LogicalNegation,
                AddressOf,
                Comp,
                PointerDereference
            };

            ExpressionUnaryOperation(Kind kind, Operand operand);

            Kind GetKind() const;
            const Operand& GetOperand() const;

        private:
            Kind m_kind;
            Operand operand;
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

            ExpressionBinaryOperation(Kind kind, Operand firstOperand, Operand secondOperand);

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

                enum class Kind {
                    Name,
                    PointerDereference,
                    StructMemberAccess
                };

                LValue(const Name               & name);
                LValue(const PointerDereference & pointerDereference);
                LValue(const StructMemberAccess & structMemberAccess);

                Kind GetKind() const;

                const Name               & GetName               () const;
                const PointerDereference & GetPointerDereference () const;
                const StructMemberAccess & GetStructMemberAccess () const;

            private:
                Kind m_kind;
                union {
                    Name               name;
                    PointerDereference pointerDereference;
                    StructMemberAccess structMemberAccess;
                } m_data;
            };

            enum class Kind {
                Literal,
                FunctionCall,
                Block,
                If,
                Loop,
                UnaryOperation,
                BinaryOperation,
                Name
            };

            Expression(const ExpressionLiteral         & literal        );
            Expression(const ExpressionFunctionCall    & functionCall   );
            Expression(const ExpressionBlock           & block          );
            Expression(const ExpressionIf              & ifStatement    );
            Expression(const ExpressionLoop            & loop           );
            Expression(const ExpressionUnaryOperation  & unaryOperation );
            Expression(const ExpressionBinaryOperation & binaryOperation);
            Expression(const ExpressionName            & name           );

            Kind GetKind() const;

            const ExpressionLiteral         & GetLiteral         () const;
            const ExpressionFunctionCall    & GetFunctionCall    () const;
            const ExpressionBlock           & GetBlock           () const;
            const ExpressionIf              & GetIf              () const;
            const ExpressionLoop            & GetLoop            () const;
            const ExpressionUnaryOperation  & GetUnaryOperation  () const;
            const ExpressionBinaryOperation & GetBinaryOperation () const;
            const ExpressionName            & GetName            () const;
            
            bool IsLValue() const;
            LValue ToLValue() const;

        private:
            union {
                ExpressionLiteral         literal;
                ExpressionFunctionCall    functionCall;
                ExpressionBlock           block;
                ExpressionIf              ifStatement;
                ExpressionLoop            loop;
                ExpressionUnaryOperation  unaryOperation;
                ExpressionBinaryOperation binaryOperation;
                ExpressionName            name;
            } m_data;
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
            using VarType  = std::optional<std::unique_ptr<Type>>;
            using VarValue = std::optional<std::unique_ptr<Expression>>;

            StatementVariableDefinition(const VarName& varName, VarType varType); // an optional type
            StatementVariableDefinition(const VarName& varName, VarValue varValue); // infer type
            StatementVariableDefinition(const VarName& varName, VarType varType, VarValue varValue);

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
            enum class Kind {
                Expression,
                BinaryOperation,
                VariableDefinition,
                Assignment,
                Continue,
                Break
            };

            Statement(Kind kind);
            Statement(const StatementExpression         & expression        );
            Statement(const StatementBinaryOperation    & binaryOperation   );
            Statement(const StatementVariableDefinition & variableDefinition);
            Statement(const StatementAssignment         & assignment        );
            Statement(const StatementBreak              & breakStatement    );

            Kind GetKind() const;

            const StatementExpression         & GetExpression        () const;
            const StatementBinaryOperation    & GetBinaryOperation   () const;
            const StatementVariableDefinition & GetVariableDefintion () const;
            const StatementAssignment         & GetAssignment        () const;
            const StatementBreak              & GetBreak             () const;

        private:
            Kind m_kind;
            union {
                StatementExpression         expression;
                StatementBinaryOperation    binaryOperation;
                StatementVariableDefinition variableDefinition;
                StatementAssignment         assignment;
                StatementBreak              breakStatement;
            } m_data;
        };

        /*
         *
         * AST Node
         *
         */
        
    public:
        enum class Kind {
            Type,
            Expression,
            Statement
        };

        ASTNode(const Type       & type      );
        ASTNode(const Expression & expression);
        ASTNode(const Statement  & statement );

        Kind GetKind() const;

        const Type       & GetType       () const;
        const Expression & GetExpression () const;
        const Statement  & GetStatement  () const;

    private:
        Kind m_kind;
        union {
            Type       type;
            Expression expression;
            Statement  statement;
        } m_data;
    };

}
