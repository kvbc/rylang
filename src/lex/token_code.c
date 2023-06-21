#include "token_code.h"

// 
// [Public]
// 

inline bool ryL_TokenCode_is_keyword( enum ryL_TokenCode code ) {
    return (code > TK__KW_FIRST) && (code < TK__KW_LAST);
}

inline enum ryL_TokenCode ryL_TokenCode_from_char( u8 c ) {
    RY_ASSERT(c >= RYL_CHAR_MIN && c <= RYL_CHAR_MAX);
    return (enum ryL_TokenCode)c;
}