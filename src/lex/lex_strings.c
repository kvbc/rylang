#include "lex_strings.h"
#include "../util/arr_view.h"
#include "../util/dyn_arr.h"

// 
// [Private]
// 

struct Strings_DynHash {
    usize hash;
    struct ryU_DynArr * dyn;
};

// 
// [Public]
// 

void ryL_Strings_init( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);
    ryU_Arr_init(&strs->_dyn_hashes,  sizeof(struct Strings_DynHash));
    ryU_Arr_init(&strs->_strs,  sizeof(struct ryU_Arr));
    ryU_Arr_init(&strs->_views, sizeof(struct ryU_ArrView));
    ryU_Arr_init(&strs->_dyns,  sizeof(struct ryU_DynArr));
}

void ryL_Strings_free( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);

    {
        usize strs_len = ryU_Arr_get_len(&strs->_strs);
        for( usize i = 0; i < strs_len; i++ )
            ryU_Arr_free(ryU_Arr_get(&strs->_strs, i));
    } {
        usize dyns_len = ryU_Arr_get_len(&strs->_dyns);
        for( usize i = 0; i < dyns_len; i++ )
            ryU_DynArr_free(ryU_Arr_get(&strs->_dyns, i));
    }

    ryU_Arr_free(&strs->_dyn_hashes);
    ryU_Arr_free(&strs->_strs);
    ryU_Arr_free(&strs->_dyns);
    ryU_Arr_free(&strs->_views);
}

// getters

struct ryU_DynArr * ryL_Strings_get_dyn( struct ryL_Strings * strs, usize hash ) {
    usize len = ryU_Arr_get_len(&strs->_dyn_hashes);
    for( usize i = 0; i < len; i++ ) {
        struct Strings_DynHash * dynhash = ryU_Arr_get(&strs->_dyn_hashes, i);
        if( dynhash->hash == hash )
            return dynhash->dyn;
    }
    return NULL;
}

// getters : next

struct ryU_Arr * ryL_Strings_get_next_str( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);
    return (struct ryU_Arr *)ryU_Arr_next(&strs->_strs);
}

struct ryU_ArrView * ryL_Strings_get_next_view( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);
    return (struct ryU_ArrView *)ryU_Arr_next(&strs->_views);
}

struct ryU_DynArr * ryL_Strings_get_next_dyn( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);
    return (struct ryU_DynArr *)ryU_Arr_next(&strs->_dyns);
}