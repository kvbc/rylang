#ifndef RYU_ARR_VIEW_H
#define RYU_ARR_VIEW_H

#include "../ry.h"

struct ryU_Arr;
struct ryU_DynArr;

// 
// 
// 

struct ryU_ArrView {
    usize _len; // item count
    const void * _buf;
    usize _itemsz; // in bytes
};

void ryU_ArrView_init( struct ryU_ArrView * out_view, usize itemsz );

// setters
void ryU_ArrView_set     ( struct ryU_ArrView * out_view, const void * buf, usize len );
void ryU_ArrView_set_view( struct ryU_ArrView * out_view, const struct ryU_ArrView * view );
void ryU_ArrView_set_arr ( struct ryU_ArrView * out_view, const struct ryU_Arr * arr );
void ryU_ArrView_set_dyn ( struct ryU_ArrView * out_view, const struct ryU_DynArr * arr );

void ryU_ArrView_clear( struct ryU_ArrView * view );

// getters
usize        ryU_ArrView_get_len   ( const struct ryU_ArrView * view );
const void * ryU_ArrView_get_buf   ( const struct ryU_ArrView * view );
usize        ryU_ArrView_get_itemsz( const struct ryU_ArrView * view );

// 
// 
// 

#endif // RYU_ARR_VIEW_H