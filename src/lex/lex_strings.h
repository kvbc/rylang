#ifndef RYL_STRINGS_H
#define RYL_STRINGS_H

#include "../util/arr.h"

struct ryL_Strings {
    struct ryU_Arr _dyn_hashes; // see lexer.c
    struct ryU_Arr _views; // arr of ryU_ArrView<u8>
    struct ryU_Arr _dyns; // arr of ryU_DynArr<u8>
    struct ryU_Arr _strs; // ar of ryU_Arr<u8>
};

void ryL_Strings_init( struct ryL_Strings * strs );
void ryL_Strings_free( struct ryL_Strings * strs );
// getters
const struct ryU_DynArr * ryL_Strings_get_dyn ( struct ryL_Strings * strs, usize hash );
usize                     ryL_Strings_get_hash( struct ryL_Strings * strs, const struct ryU_DynArr * dyn );
// getters : next
struct ryU_Arr     * ryL_Strings_get_next_str ( struct ryL_Strings * strs );
struct ryU_ArrView * ryL_Strings_get_next_view( struct ryL_Strings * strs );
struct ryU_DynArr  * ryL_Strings_get_next_dyn ( struct ryL_Strings * strs );
// pop
void ryL_Strings_pop_str ( struct ryL_Strings * strs );
void ryL_Strings_pop_view( struct ryL_Strings * strs );
void ryL_Strings_pop_dyn ( struct ryL_Strings * strs );

#endif // RYL_STRINGS_H