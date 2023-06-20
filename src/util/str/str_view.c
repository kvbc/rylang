#include "str_view.h"
#include "alloc_str.h"
#include "dyn_str.h"

// 
// 
// 

void ryUSTR_StringView_init_buflen( struct ryUSTR_StringView * out_view, const u8 * buf, usize len ) {
    out_view->_buf = buf;
    out_view->_len = len;
}

void ryUSTR_StringView_init_buf( struct ryUSTR_StringView * out_view, const u8 * buf ) {
    ryUSTR_StringView_init_buflen(out_view, buf, strlen(buf));
}

void ryUSTR_StringView_init_view( struct ryUSTR_StringView * out_view, struct ryUSTR_StringView * view ) {
    ryUSTR_StringView_init_buflen(
        out_view,
        ryUSTR_StringView_getbuf(view),
        ryUSTR_StringView_getlen(view)
    );
}

void ryUSTR_StringView_init_alloc( struct ryUSTR_StringView * out_view, struct ryUSTR_AllocStr * str ) {
    ryUSTR_StringView_init_buflen(
        out_view,
        ryUSTR_AllocStr_getbuf(str),
        ryUSTR_AllocStr_getlen(str)
    );
}

void ryUSTR_StringView_init_dyn( struct ryUSTR_StringView * out_view, struct ryUSTR_DynStr * str ) {
    if( ryUSTR_DynStr_is_alloc(str) )
        ryUSTR_StringView_init_alloc(out_view, ryUSTR_DynStr_get_alloc(str));
    else
        ryUSTR_StringView_init_view(out_view, ryUSTR_DynStr_get_view(str));
}

// 
// Getters
// 

usize ryUSTR_StringView_getlen( const struct ryUSTR_StringView * view ) {
    return view->_len;
}

const u8 * const ryUSTR_StringView_getbuf( const struct ryUSTR_StringView * view ) {
    return view->_buf;
}