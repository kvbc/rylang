#include "alloc_str.h"
#include "dyn_str.h"

// 
// [Public] 
// 

void ryUSTR_AllocStr_free( struct ryUSTR_AllocStr * str ) {
    free(str->_buf);
}

// 
// [Public] init : move
// 

void ryUSTR_AllocStr_init_movebuflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len ) {
    out_str->_buf = buf;
    out_str->_len = len;
}

void ryUSTR_AllocStr_init_movebuf( struct ryUSTR_AllocStr * out_str, u8 * buf ) {
    ryUSTR_AllocStr_init_movebuflen(out_str, buf, strlen(buf));
}
void ryUSTR_AllocStr_init_movealloc( struct ryUSTR_AllocStr * out_str, struct ryUSTR_AllocStr * str ) {
    ryUSTR_AllocStr_init_movebuflen(out_str, str->_buf, str->_len);
    ryUSTR_AllocStr_init_movebuflen(str, NULL, 0);
}

// 
// [Public] init : copy
// 

void ryUSTR_AllocStr_init_copybuflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len ) {
    u8 * buf_cpy = RY_MALLOC(len);
    RY_ASSERT(buf_cpy != NULL);
    RY_MEMCPY(buf_cpy, buf, len);
    ryUSTR_AllocStr_init_movebuflen(out_str, buf_cpy, len);
}

void ryUSTR_AllocStr_init_copybuf( struct ryUSTR_AllocStr * out_str, u8 * buf ) {
    ryUSTR_AllocStr_init_copybuflen(out_str, buf, strlen(buf));
}

void ryUSTR_AllocStr_init_copyview( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_StringView * view ) {
    const usize viewlen = ryUSTR_StringView_getlen(view);
    const u8 * const viewbuf = ryUSTR_StringView_getbuf(view);
    ryUSTR_AllocStr_init_copybuflen(out_str, viewbuf, viewlen);

}
void ryUSTR_AllocStr_init_copyalloc( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_AllocStr * str ) {
    const u8 * buf = ryUSTR_AllocStr_getbuf(str);
    usize len = ryUSTR_AllocStr_getlen(str);
    ryUSTR_AllocStr_init_copybuflen(out_str, buf, len);
}

void ryUSTR_AllocStr_init_copydyn( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_DynStr * str ) {
    if( ryUSTR_DynStr_is_alloc(str) )
        ryUSTR_AllocStr_init_copyalloc(out_str, ryUSTR_DynStr_get_alloc(str));
    else
        ryUSTR_AllocStr_init_copyview(out_str, ryUSTR_DynStr_get_view(str));
}

// 
// [Public] Getters
// 

usize ryUSTR_AllocStr_getlen( const struct ryUSTR_AllocStr * str ) {
    return str->_len;
}

const u8 * const ryUSTR_AllocStr_getbuf( const struct ryUSTR_AllocStr * str ) {
    return str->_buf;
}