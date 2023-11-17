#pragma once

#define RY_WRAP_ENUM(TK) TK ,
#define RY_WRAP_STR(TK) #TK ,

#define RY_EXPAND_ENUM_VALUES(NAME, VALUE) \
    NAME = (int)VALUE,

#define RY_EXPAND_VALUES(NAME, VALUE) \
    VALUE,

namespace ry {
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}