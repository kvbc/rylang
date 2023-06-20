#include "dyn_str.h"

// 
// 
// 

struct ryUSTR_AllocStr * ryUSTR_DynStr_init_alloc( struct ryUSTR_DynStr * out_str ) {
    out_str->_is_alloc = true;
    return ryUSTR_DynStr_get_alloc(out_str);
}
struct ryUSTR_StrView * ryUSTR_DynStr_init_view( struct ryUSTR_DynStr * out_str ) {
    out_str->_is_alloc = false;
    return ryUSTR_DynStr_get_view(out_str);
}

void ryUSTR_DynStr_free( struct ryUSTR_DynStr * str ) {
    if( str->_is_alloc )
        ryUSTR_AllocStr_free(ryUSTR_DynStr_get_alloc(str));
}

// 
// Getters
// 

bool ryUSTR_DynStr_is_alloc( const struct ryUSTR_DynStr * str ) {
    return str->_is_alloc;
}

struct ryUSTR_AllocStr * ryUSTR_DynStr_get_alloc( const struct ryUSTR_DynStr * str ) {
    RY_ASSERT(str->_is_alloc);
    return &(str->_variant.alloc);
}

struct ryUSTR_StrView * ryUSTR_DynStr_get_view( const struct ryUSTR_DynStr * str ) {
    RY_ASSERT(str->_is_alloc == false);
    return &(str->_variant.view);
}

usize ryUSTR_DynStr_get_len( const struct ryUSTR_DynStr * str ) {
    if( str->_is_alloc )
        return ryUSTR_AllocStr_getlen(ryUSTR_DynStr_get_alloc(str));
    else
        return ryUSTR_StrView_getlen(ryUSTR_DynStr_get_view(str));
}

const u8 * ryUSTR_DynStr_get_buf( const struct ryUSTR_DynStr * str ) {
    if( str->_is_alloc )
        return ryUSTR_AllocStr_getbuf(ryUSTR_DynStr_get_alloc(str));
    else
        return ryUSTR_StrView_getbuf(ryUSTR_DynStr_get_view(str));
}