#include "alloc_str.h"
#include "dyn_str.h"
#include "cstr.h"
#include "str_view.h"

// 
// [Static]
// 

static void clear( struct ryUSTR_AllocStr * out_str ) {
    out_str->_buf = NULL;
    out_str->_len = 0;
}

// 
// 
// 

void ryUSTR_AllocStr_init( struct ryUSTR_AllocStr * out_str ) {
    clear(out_str);
}

void ryUSTR_AllocStr_free( struct ryUSTR_AllocStr * str ) {
    RY_FREE(str->_buf);
    clear(str);
}

// 
// move
// 

void ryUSTR_AllocStr_move_buflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len ) {
    ryUSTR_AllocStr_free(out_str);
    out_str->_buf = buf;
    out_str->_len = len;
}

void ryUSTR_AllocStr_move_buf( struct ryUSTR_AllocStr * out_str, u8 * buf ) {
    ryUSTR_AllocStr_move_buflen(out_str, buf, ryUSTR_cstr_len(buf));
}
void ryUSTR_AllocStr_move_alloc( struct ryUSTR_AllocStr * out_str, struct ryUSTR_AllocStr * str ) {
    ryUSTR_AllocStr_move_buflen(out_str, str->_buf, str->_len);
    clear(str);
}

// 
// copy
// 

void ryUSTR_AllocStr_copy_buflen( struct ryUSTR_AllocStr * out_str, const u8 * buf, usize len ) {
    u8 * buf_cpy = (u8 *)RY_MALLOC(len);
    RY_ASSERT(buf_cpy != NULL);
    RY_MEMCPY(buf_cpy, buf, len);
    ryUSTR_AllocStr_move_buflen(out_str, buf_cpy, len);
}

void ryUSTR_AllocStr_copy_buf( struct ryUSTR_AllocStr * out_str, const u8 * buf ) {
    ryUSTR_AllocStr_copy_buflen(out_str, buf, ryUSTR_cstr_len(buf));
}

void ryUSTR_AllocStr_copy_view( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_StrView * view ) {
    const usize viewlen = ryUSTR_StrView_get_len(view);
    const u8 * const viewbuf = ryUSTR_StrView_get_buf(view);
    ryUSTR_AllocStr_copy_buflen(out_str, viewbuf, viewlen);

}
void ryUSTR_AllocStr_copy_alloc( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_AllocStr * str ) {
    const u8 * buf = ryUSTR_AllocStr_get_buf(str);
    usize len = ryUSTR_AllocStr_get_len(str);
    ryUSTR_AllocStr_copy_buflen(out_str, buf, len);
}

void ryUSTR_AllocStr_copy_dyn( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_DynStr * str ) {
    if( ryUSTR_DynStr_is_alloc(str) )
        ryUSTR_AllocStr_copy_alloc(out_str, ryUSTR_DynStr_cget_alloc(str));
    else
        ryUSTR_AllocStr_copy_view(out_str, ryUSTR_DynStr_cget_view(str));
}

// 
// getters
// 

usize ryUSTR_AllocStr_get_len( const struct ryUSTR_AllocStr * str ) {
    return str->_len;
}

const u8 * ryUSTR_AllocStr_get_buf( const struct ryUSTR_AllocStr * str ) {
    return str->_buf;
}