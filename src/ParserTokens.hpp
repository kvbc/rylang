#pragma once

#define RY_PARSER__KEYWORD_TOKENS_EXPAND_NAMES(_, NAME) NAME ,
#define RY_PARSER__KEYWORD_TOKENS(E) /* E - expand macro */ \
    E("", _FirstKeyword) \
    \
    E("do",       KeywordDo) \
    E("if",       KeywordIf) \
    E("else",     KeywordElse) \
    E("loop",     KeywordLoop) \
    E("continue", KeywordContinue) \
    E("break",    KeywordBreak) \
    E("false",    KeywordFalse) \
    E("true",     KeywordTrue) \
    E("null",     KeywordNull) \
    E("not",      KeywordNot) \
    E("or",       KeywordOr) \
    E("and",      KeywordAnd) \
    E("as",       KeywordAs) \
    E("comp",     KeywordComp) \
    \
    E("", _FirstTypeKeyword) \
    \
    E("char", KeywordChar) \
    E("i8", KeywordI8) \
    E("i16", KeywordI16) \
    E("i32", KeywordI32) \
    E("i64", KeywordI64) \
    E("i128", KeywordI128) \
    E("u8", KeywordU8) \
    E("u16", KeywordU16) \
    E("u32", KeywordU32) \
    E("u64", KeywordU64) \
    E("u128", KeywordU128) \
    E("f32", KeywordF32) \
    E("f64", KeywordF64) \
    E("bool", KeywordBool) \
    \
    E("", _LastTypeKeyword) \
    E("", _LastKeyword) 

#define RY_PARSER_SPECIAL_TOKENS_EXPAND_VALUES(VALUE, _) VALUE ,
#define RY_PARSER_SPECIAL_TOKENS_EXPAND_NAMES(_, NAME) NAME ,
#define RY_PARSER_SPECIAL_TOKENS(E) /* E - expand macro */ \
    E("-=", SubEqual) \
    E("+=", AddEqual) \
    E("/=", DivEqual) \
    E("%=", ModEqual) \
    E("*=", MulEqual) \
    \
    E("|=",  BitOrEqual) \
    E("^=",  BitXorEqual) \
    E("&=",  BitAndEqual) \
    E("<<",  BitLShift) \
    E("<<=", BitLShiftEqual) \
    E(">>",  BitRShift) \
    E(">>=", BitRShiftEqual) \
    \
    E("==", Equal) \
    E("!=", Unequal) \
    E("<=", LessEqual) \
    E(">=", GreatEqual) \
    \
    E("=>", FunctionArrow) \
    E(":=", Define) \
    \
    RY_PARSER__KEYWORD_TOKENS(E) \
    \
    E("", _Last)

