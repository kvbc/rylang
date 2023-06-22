#ifndef RYU_DYN_ARR_H
#define RYU_DYN_ARR_H

#include "../ry.h"
#include "arr.h"
#include "arr_view.h"

struct ryU_Arr;
struct ryU_ArrView;

// 
// 
// 

struct ryU_DynArr {
    union {
        struct ryU_Arr * arr;
        struct ryU_ArrView * view;
    } _variant;
    bool _is_view;
};

void ryU_DynArr_init_arr ( struct ryU_DynArr * out_arr, struct ryU_Arr * in_arr );
void ryU_DynArr_init_view( struct ryU_DynArr * out_arr, struct ryU_ArrView * view );
void ryU_DynArr_free     ( struct ryU_DynArr * arr );

// getters
bool         ryU_DynArr_is_view   ( const struct ryU_DynArr * arr );
usize        ryU_DynArr_get_len   ( const struct ryU_DynArr * arr );
const void * ryU_DynArr_get_buf   ( const struct ryU_DynArr * arr );
usize        ryU_DynArr_get_itemsz( const struct ryU_DynArr * arr );
// 
const struct ryU_Arr     * ryU_DynArr_cget_arr ( const struct ryU_DynArr * arr );
const struct ryU_ArrView * ryU_DynArr_cget_view( const struct ryU_DynArr * arr );
      struct ryU_Arr     * ryU_DynArr_get_arr  ( struct ryU_DynArr * arr );
      struct ryU_ArrView * ryU_DynArr_get_view ( struct ryU_DynArr * arr );

// 
// 
// 

#endif // RYU_DYN_ARR_H