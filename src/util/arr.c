#include "arr.h"
#include "dyn_arr.h"
#include "arr_view.h"
#include "str.h"
#include <stdio.h>

// 
// 
// 

static inline void * Arr_unsafe_get( const struct ryU_Arr * arr, usize idx ) {
    RY_ASSERT(arr);
    RY_ASSERT(idx <= arr->_len); // <= !
    return (u8*)arr->_buf + (idx * arr->_itemsz);
}

static inline void * Arr_unsafe_next( struct ryU_Arr * arr ) {
    RY_ASSERT(arr);
    RY_ASSERT(arr->_buf);
    RY_ASSERT(arr->_itemsz > 0);
    return Arr_unsafe_get(arr, arr->_len);
}

static inline void * Arr_top( struct ryU_Arr * arr ) {
    RY_ASSERT(arr);
    return Arr_unsafe_get(arr, arr->_len - 1);
}

// 
// 
// 

void ryU_Arr_init (struct ryU_Arr * out_arr, usize itemsz) {
    RY_ASSERT(out_arr);
    RY_ASSERT(itemsz > 0);
    out_arr->_cap = 0;
    out_arr->_itemsz = itemsz;
    out_arr->_buf = NULL;
    out_arr->_len = 0;
}

void ryU_Arr_free (struct ryU_Arr * arr) {
    RY_ASSERT(arr);
    arr->_len = 0;
    arr->_cap = 0;
    arr->_itemsz = 0;
    RY_FREE(arr->_buf);
    arr->_buf = NULL;
}

// 

void * ryU_Arr_next( struct ryU_Arr * arr ) {
    RY_ASSERT(arr);
    if( arr->_len >= arr->_cap )
        ryU_Arr_push(arr, NULL); // make sure there's enough space for the next item (grows if that's not the case)
    else
        arr->_len++;
    return Arr_top(arr);
}

// pop

void * ryU_Arr_pop( struct ryU_Arr * arr ) {
    RY_ASSERT(arr);
    if( arr->_len == 0 )
        return NULL;
    void * top = ryU_Arr_get_top(arr);
    arr->_len--;
    return top;
}

// 
// Push
// 

void ryU_Arr_push( struct ryU_Arr * arr, const void * data ) {
    RY_ASSERT(arr);
    ryU_Arr_push_buf(arr, data, 1);
}

void ryU_Arr_push_buf( struct ryU_Arr * arr, const void * buf, usize len ) {
    RY_ASSERT(arr);
    RY_ASSERT(len > 0);
    if( arr->_len + len >= arr->_cap ) {
        if( arr->_cap == 0 )
            arr->_cap = 5;
        arr->_cap += len;
        arr->_cap += arr->_cap / 2;
        arr->_buf = RY_REALLOC(arr->_buf, arr->_cap * arr->_itemsz);
    }
    if( buf != NULL ) {
        RY_MEMCPY(Arr_unsafe_next(arr), buf, len * arr->_itemsz);
    }
    arr->_len += len;
}

void ryU_Arr_push_arr( struct ryU_Arr * arr, const struct ryU_Arr * in_arr ) {
    RY_ASSERT(arr);
    RY_ASSERT(in_arr);
    ryU_Arr_push_buf(arr, ryU_Arr_get_buf(in_arr), ryU_Arr_get_len(in_arr));
}

void ryU_Arr_push_view( struct ryU_Arr * arr, const struct ryU_ArrView * view ) {
    RY_ASSERT(arr);
    RY_ASSERT(view);
    ryU_Arr_push_buf(arr, ryU_ArrView_get_buf(view), ryU_ArrView_get_len(view));
}

void ryU_Arr_push_dyn( struct ryU_Arr * arr, const struct ryU_DynArr * dyn ) {
    RY_ASSERT(arr);
    RY_ASSERT(dyn);
    if( ryU_DynArr_is_view(dyn) )
        ryU_Arr_push_view(arr, ryU_DynArr_cget_view(dyn));
    else
        ryU_Arr_push_arr(arr, ryU_DynArr_cget_arr(dyn));
}

// 
// Modify
// 

void * ryU_Arr_get( const struct ryU_Arr * arr, usize idx ) {
    RY_ASSERT(arr);
    RY_ASSERT(idx < arr->_len);
    return Arr_unsafe_get(arr, idx);
}

void ryU_Arr_clear (struct ryU_Arr * arr) {
    arr->_len = 0;
}

// 
// Move into
// 

void ryU_Arr_move_buf( struct ryU_Arr * out_arr, void * buf, usize len ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(buf);
    RY_ASSERT(len > 0);
    usize itemsz = out_arr->_itemsz;
    ryU_Arr_free(out_arr);
    ryU_Arr_init(out_arr, itemsz);
    out_arr->_buf = buf;
    out_arr->_len = len;
}

void ryU_Arr_move_arr( struct ryU_Arr * out_arr, struct ryU_Arr * in_arr ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(in_arr);
    ryU_Arr_free(out_arr);
    ryU_Arr_init(out_arr, in_arr->_itemsz);
    ryU_Arr_move_buf(out_arr, in_arr->_buf, in_arr->_len);
    in_arr->_buf = NULL;
    in_arr->_cap = 0;
    in_arr->_itemsz = 0;
    in_arr->_len = 0;
}

// 
// Copy into
// 

void ryU_Arr_copy_buf ( struct ryU_Arr * out_arr, const void * buf, usize len ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(buf);
    RY_ASSERT(len > 0);
    ryU_Arr_clear(out_arr);
    ryU_Arr_push_buf(out_arr, buf, len);
}

void ryU_Arr_copy_arr ( struct ryU_Arr * out_arr, const struct ryU_Arr * in_arr ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(in_arr);
    ryU_Arr_copy_buf(out_arr, ryU_Arr_get_buf(in_arr), ryU_Arr_get_len(in_arr));
}

void ryU_Arr_copy_view( struct ryU_Arr * out_arr, const struct ryU_ArrView * view ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(view);
    ryU_Arr_copy_buf(out_arr, ryU_ArrView_get_buf(view), ryU_ArrView_get_len(view));
}

void ryU_Arr_copy_dyn ( struct ryU_Arr * out_arr, const struct ryU_DynArr * in_arr ) {
    RY_ASSERT(out_arr);
    RY_ASSERT(in_arr);
    if( ryU_DynArr_is_view(in_arr) )
        ryU_Arr_copy_view(out_arr, ryU_DynArr_cget_view(in_arr));
    else
        ryU_Arr_copy_arr(out_arr, ryU_DynArr_cget_arr(in_arr));
}

// 
// Getters
// 

usize ryU_Arr_get_cap( const struct ryU_Arr * arr ) {
    return arr->_cap;
}

usize ryU_Arr_get_len( const struct ryU_Arr * arr ) {
    return arr->_len;
}

usize ryU_Arr_get_itemsz( const struct ryU_Arr * arr ) {
    return arr->_itemsz;
}

const void * ryU_Arr_get_buf( const struct ryU_Arr * arr ) {
    return arr->_buf;
}

void * ryU_Arr_get_top( const struct ryU_Arr * arr ) {
    RY_ASSERT(arr);
    RY_ASSERT(arr->_len > 0);
    return ryU_Arr_get(arr, arr->_len - 1);
}