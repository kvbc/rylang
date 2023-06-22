#include "arr_view.h"
#include "arr.h"
#include "dyn_arr.h"

// 
// 
// 

void ryU_ArrView_init( struct ryU_ArrView * out_view, usize itemsz ) {
    out_view->_buf = NULL;
    out_view->_itemsz = itemsz;
    out_view->_len = 0;
}

// 

void ryU_ArrView_set( struct ryU_ArrView * out_view, const void * buf, usize len ) {
    out_view->_buf = buf;
    out_view->_len = len;
}

void ryU_ArrView_set_view( struct ryU_ArrView * out_view, const struct ryU_ArrView * view ) {
    ryU_ArrView_set(out_view, view->_buf, view->_len);
}

void ryU_ArrView_set_arr( struct ryU_ArrView * out_view, const struct ryU_Arr * arr ) {
    out_view->_buf = ryU_Arr_get_buf(arr);
    out_view->_len = ryU_Arr_get_len(arr);
    out_view->_itemsz = ryU_Arr_get_itemsz(arr);
}

void ryU_ArrView_set_dyn( struct ryU_ArrView * out_view, const struct ryU_DynArr * arr ) {
    if( ryU_DynArr_is_view(arr) )
        ryU_ArrView_set_view(out_view, ryU_DynArr_cget_view(arr));
    else
        ryU_ArrView_set_arr(out_view, ryU_DynArr_cget_arr(arr));
}

// 

void ryU_ArrView_clear( struct ryU_ArrView * view ) {
    view->_buf = NULL;
    view->_len = 0;
    view->_itemsz = 0;
}

// 
// getters
// 

usize ryU_ArrView_get_len( const struct ryU_ArrView * view ) {
    return view->_len;
}

const void * ryU_ArrView_get_buf( const struct ryU_ArrView * view ) {
    return view->_buf;
}

usize ryU_ArrView_get_itemsz( const struct ryU_ArrView * view ) {
    return view->_itemsz;
}