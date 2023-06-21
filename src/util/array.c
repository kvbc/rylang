#include "array.h"

void ryU_Array_init (struct ryU_Array * out_arr, usize itemsz) {
    out_arr->_len = 0;
    out_arr->_cap = 0;
    out_arr->_data = NULL;
    out_arr->_itemsz = itemsz;
}

void ryU_Array_free (struct ryU_Array * arr) {
    arr->_len = 0;
    arr->_cap = 0;
    RY_FREE(arr->_data);
}

void ryU_Array_push( struct ryU_Array * arr, const void * new_data ) {
    if( arr->_len >= arr->_cap ) {
        if( arr->_cap == 0 )
            arr->_cap = 5;
        arr->_cap += arr->_cap / 2;
        arr->_data = RY_REALLOC(arr->_data, arr->_cap * arr->_itemsz);
    }
    RY_MEMCPY(
        (u8*)arr->_data + (arr->_len * arr->_itemsz),
        new_data, arr->_itemsz
    );
    arr->_len++;
}

void * ryU_Array_get( const struct ryU_Array * arr, usize idx ) {
    RY_ASSERT(idx < arr->_len);
    return (u8*)arr->_data + (idx * arr->_itemsz);
}

void ryU_Array_clear (struct ryU_Array * arr) {
    arr->_len = 0;
}

// 
// Getters
// 

usize ryU_Array_get_len( const struct ryU_Array * arr ) {
    return arr->_len;
}

usize ryU_Array_get_cap( const struct ryU_Array * arr ) {
    return arr->_cap;
}

const void * ryU_Array_get_data( const struct ryU_Array * arr ) {
    return arr->_data;
}

usize ryU_Array_get_itemsz( const struct ryU_Array * arr ) {
    return arr->_itemsz;
}