#ifndef RYU_ARR_H
#define RYU_ARR_H

#include "../ry.h"
#include <stdint.h>
#include "arr_view.h"

struct ryU_ArrView;
struct ryU_DynArr;

// 
// 
// 

struct ryU_Arr {
    usize _cap; // items
    usize _len; // items
    void * _buf;
    usize _itemsz; // in bytes
};

void ryU_Arr_init        ( struct ryU_Arr * out_arr, usize itemsz );
void ryU_Arr_free        ( struct ryU_Arr * arr );

void * ryU_Arr_next( struct ryU_Arr * arr );

// push
void ryU_Arr_push     ( struct ryU_Arr * arr, const void * data );
void ryU_Arr_push_buf ( struct ryU_Arr * arr, const void * buf, usize len );
void ryU_Arr_push_arr ( struct ryU_Arr * arr, const struct ryU_Arr * in_arr );
void ryU_Arr_push_view( struct ryU_Arr * arr, const struct ryU_ArrView * view );
void ryU_Arr_push_dyn ( struct ryU_Arr * arr, const struct ryU_DynArr * in_arr );

// modify
void   ryU_Arr_clear( struct ryU_Arr * arr );
void * ryU_Arr_get  ( const struct ryU_Arr * arr, usize idx );

// move into
void ryU_Arr_move_buf( struct ryU_Arr * out_arr, void * buf, usize len );
void ryU_Arr_move_arr( struct ryU_Arr * out_arr, struct ryU_Arr * in_arr );

// copy into
void ryU_Arr_copy_buf ( struct ryU_Arr * out_arr, const void * buf, usize len );
void ryU_Arr_copy_arr ( struct ryU_Arr * out_arr, const struct ryU_Arr * in_arr );
void ryU_Arr_copy_view( struct ryU_Arr * out_arr, const struct ryU_ArrView * view );
void ryU_Arr_copy_dyn ( struct ryU_Arr * out_arr, const struct ryU_DynArr * in_arr );

// getters
usize        ryU_Arr_get_cap    ( const struct ryU_Arr * arr );
usize        ryU_Arr_get_len    ( const struct ryU_Arr * arr );
usize        ryU_Arr_get_itemsz ( const struct ryU_Arr * arr );
const void * ryU_Arr_get_buf    ( const struct ryU_Arr * arr );

#endif // RYU_ARR_H