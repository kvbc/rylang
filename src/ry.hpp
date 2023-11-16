#pragma once

#define RY_WRAP_ENUM(TK) TK ,
#define RY_WRAP_STR(TK) #TK ,

namespace ry {
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}