#include "lex_strings.h"
#include "../util/arr_view.h"
#include "../util/dyn_arr.h"
#include "../util/str.h"

// 
// [Private]
// 

struct Strings_DynHash {
    usize hash;
    const struct ryU_DynArr * dyn;
};

static struct Strings_DynHash * Strings_get_dynhash( const struct ryL_Strings * strs, usize hash, const struct ryU_DynArr * dyn ) {
    RY_ASSERT(strs);
    if( hash == 0 )
        RY_ASSERT(dyn);
    else
        RY_ASSERT(dyn == NULL);

    usize len = ryU_Arr_get_len(&strs->_dyn_hashes);
    for( usize i = 0; i < len; i++ ) {
        struct Strings_DynHash * dynhash = (struct Strings_DynHash *)ryU_Arr_get(&strs->_dyn_hashes, i);
        if( dynhash->hash == hash || dynhash->dyn == dyn )
            return dynhash;
    }

    return NULL;
}

// 
// [Public]
// 

void ryL_Strings_init( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);
    ryU_Arr_init(&strs->_dyn_hashes, sizeof(struct Strings_DynHash));
    ryU_Arr_init(&strs->_strs,       sizeof(struct ryU_Arr));
    ryU_Arr_init(&strs->_views,      sizeof(struct ryU_ArrView));
    ryU_Arr_init(&strs->_dyns,       sizeof(struct ryU_DynArr));
}

void ryL_Strings_free( struct ryL_Strings * strs ) {
    RY_ASSERT(strs);

    {
        usize strs_len = ryU_Arr_get_len(&strs->_strs);
        for( usize i = 0; i < strs_len; i++ )
            ryU_Arr_free((struct ryU_Arr *)ryU_Arr_get(&strs->_strs, i));
    } {
        usize dyns_len = ryU_Arr_get_len(&strs->_dyns);
        for( usize i = 0; i < dyns_len; i++ )
            ryU_DynArr_free((struct ryU_DynArr *)ryU_Arr_get(&strs->_dyns, i));
    }

    ryU_Arr_free(&strs->_dyn_hashes);
    ryU_Arr_free(&strs->_strs);
    ryU_Arr_free(&strs->_dyns);
    ryU_Arr_free(&strs->_views);
}

// getters

const struct ryU_DynArr * ryL_Strings_get_dyn( struct ryL_Strings * strs, usize hash ) {
    struct Strings_DynHash * dynhash = Strings_get_dynhash(strs, hash, NULL);
    if( dynhash == NULL )
        return NULL;
    return dynhash->dyn;
}

usize ryL_Strings_get_hash( struct ryL_Strings * strs, const struct ryU_DynArr * dyn ) {
    struct Strings_DynHash * dynhash = Strings_get_dynhash(strs, 0, dyn);
    if( dynhash == NULL ) {
        dynhash = (struct Strings_DynHash *)ryU_Arr_next(&strs->_dyn_hashes);
        dynhash->dyn = dyn;
        dynhash->hash = ryU_cstr_hash(
            (const u8 *)ryU_DynArr_get_buf(dyn),
            ryU_DynArr_get_len(dyn)
        );
    }
    return dynhash->hash;
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

// pop

void ryL_Strings_pop_str( struct ryL_Strings * strs ) {
    struct ryU_Arr * str = (struct ryU_Arr *)ryU_Arr_pop(&strs->_strs);
    ryU_Arr_free(str);
}

void ryL_Strings_pop_view( struct ryL_Strings * strs ) {
    ryU_Arr_pop(&strs->_views);
}

void ryL_Strings_pop_dyn( struct ryL_Strings * strs ) {
    struct ryU_DynArr * dyn = (struct ryU_DynArr *)ryU_Arr_pop(&strs->_dyns);
    ryU_DynArr_free(dyn);
}