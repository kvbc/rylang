#include "dyn_arr.h"

// 
// 
// 

void ryU_DynArr_init_arr ( struct ryU_DynArr * out_arr, struct ryU_Arr * in_arr ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(in_arr);
    out_arr->_is_view = false;
    out_arr->_variant.arr = in_arr;
}

void ryU_DynArr_init_view( struct ryU_DynArr * out_arr, struct ryU_ArrView * view ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(view);
    out_arr->_is_view = true;
    out_arr->_variant.view = view;
}

void ryU_DynArr_free( struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    if( arr->_is_view )
        ryU_ArrView_clear(arr->_variant.view);
    else
        ryU_Arr_free(arr->_variant.arr);
}

// 
// Getters
// 

bool ryU_DynArr_is_view( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    return arr->_is_view;
}

usize ryU_DynArr_get_len( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    if( arr->_is_view )
        return ryU_ArrView_get_len(arr->_variant.view);
    return ryU_Arr_get_len(arr->_variant.arr);
}

const void * ryU_DynArr_get_buf( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    if( arr->_is_view )
        return ryU_ArrView_get_buf(arr->_variant.view);
    return ryU_Arr_get_buf(arr->_variant.arr);
}

usize ryU_DynArr_get_itemsz( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    if( arr->_is_view )
        return ryU_ArrView_get_itemsz(arr->_variant.view);
    return ryU_Arr_get_itemsz(arr->_variant.arr);
}

// 

struct ryU_Arr * ryU_DynArr_get_arr( struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    return (struct ryU_Arr *)ryU_DynArr_cget_arr((const struct ryU_DynArr *)arr); 
}

struct ryU_ArrView * ryU_DynArr_get_view( struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    return (struct ryU_ArrView *)ryU_DynArr_cget_view((const struct ryU_DynArr *)arr); 
}

const struct ryU_Arr * ryU_DynArr_cget_arr( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    RY_ASSERT(arr->_is_view == false);
    return arr->_variant.arr;
}

const struct ryU_ArrView * ryU_DynArr_cget_view( const struct ryU_DynArr * arr ) {
    RY_ASSERT(arr);
    RY_ASSERT(arr->_is_view);
    return arr->_variant.view;
}