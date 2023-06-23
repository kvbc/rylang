#ifndef RYL_STRINGS_H
#define RYL_STRINGS_H

#include "../util/arr.h"

struct ryU_Strings {
    struct ryU_Arr _dyn_hashes; // see lexer.c
    struct ryU_Arr _views; // arr of ryU_ArrView<u8>
    struct ryU_Arr _dyns; // arr of ryU_DynArr<u8>
    struct ryU_Arr _strs; // ar of ryU_Arr<u8>
};

void ryU_Strings_init( struct ryU_Strings * strs );
void ryU_Strings_free( struct ryU_Strings * strs );
// getters
const struct ryU_DynArr * ryU_Strings_get_dyn ( struct ryU_Strings * strs, usize hash );
usize                     ryU_Strings_get_hash( struct ryU_Strings * strs, const struct ryU_DynArr * dyn );
// getters : next
struct ryU_Arr     * ryU_Strings_push_str ( struct ryU_Strings * strs );
struct ryU_ArrView * ryU_Strings_push_view( struct ryU_Strings * strs );
struct ryU_DynArr  * ryU_Strings_push_dyn ( struct ryU_Strings * strs );
// pop
void ryU_Strings_pop_str ( struct ryU_Strings * strs );
void ryU_Strings_pop_view( struct ryU_Strings * strs );
void ryU_Strings_pop_dyn ( struct ryU_Strings * strs );

#endif // RYL_STRINGS_H