#include "token_code.h"

// 
// [Public]
// 

inline bool ryL_TokenCode_is_keyword( enum ryL_TokenCode code ) {
    return (code > TK__KW_FIRST) && (code < TK__KW_LAST);
}