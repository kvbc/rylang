#ifndef RYU_ARRAY_H
#define RYU_ARRAY_H

#include "../core.h"
#include <stdint.h>

struct ryU_Array {
    usize _len; // items
    usize _cap; // items
    void * _data;
    usize _itemsz; // bytes
};

void ryU_Array_init( struct ryU_Array * out_arr, usize itemsz );
void ryU_Array_free( struct ryU_Array * arr );
void ryU_Array_push( struct ryU_Array * arr, const void * data );
void * ryU_Array_get( const struct ryU_Array * arr, usize idx );
void ryU_Array_clear( struct ryU_Array * arr );
// Getters
usize ryU_Array_getlen( const struct ryU_Array * arr );

#endif // RYU_ARRAY_H