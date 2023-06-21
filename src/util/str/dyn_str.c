#include "dyn_str.h"

// 
// 
// 

struct ryUSTR_AllocStr * ryUSTR_DynStr_init_alloc( struct ryUSTR_DynStr * out_str ) {
    out_str->_is_alloc = true;
    out_str->_is_ptr = false;
    struct ryUSTR_AllocStr * alloc_str = ryUSTR_DynStr_get_alloc(out_str);
    ryUSTR_AllocStr_init(alloc_str);
    return alloc_str;
}

struct ryUSTR_StrView * ryUSTR_DynStr_init_view( struct ryUSTR_DynStr * out_str ) {
    out_str->_is_alloc = false;
    out_str->_is_ptr = false;
    struct ryUSTR_StrView * view = ryUSTR_DynStr_get_view(out_str);
    ryUSTR_StrView_init(view);
    return view;
}

void ryUSTR_DynStr_init_allocptr( struct ryUSTR_DynStr * out_str, struct ryUSTR_AllocStr * alloc_str ) {
    out_str->_is_alloc = true;
    out_str->_is_ptr = true;
    out_str->_variant.alloc_ptr = alloc_str;
}

void ryUSTR_DynStr_init_viewptr( struct ryUSTR_DynStr * out_str, struct ryUSTR_StrView * view ) {
    out_str->_is_alloc = false;
    out_str->_is_ptr = true;
    out_str->_variant.view_ptr = view;
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

struct ryUSTR_AllocStr * ryUSTR_DynStr_get_alloc( struct ryUSTR_DynStr * str ) {
    return (struct ryUSTR_AllocStr *)ryUSTR_DynStr_cget_alloc((const struct ryUSTR_DynStr *)str);
}

struct ryUSTR_StrView * ryUSTR_DynStr_get_view( struct ryUSTR_DynStr * str ) {
    return (struct ryUSTR_StrView *)ryUSTR_DynStr_cget_view((const struct ryUSTR_DynStr *)str);
}

const struct ryUSTR_AllocStr * ryUSTR_DynStr_cget_alloc( const struct ryUSTR_DynStr * str ) {
    RY_ASSERT(str->_is_alloc);
    if( str->_is_ptr )
        return str->_variant.alloc_ptr;
    return &(str->_variant.alloc);
}

const struct ryUSTR_StrView * ryUSTR_DynStr_cget_view( const struct ryUSTR_DynStr * str ) {
    RY_ASSERT(str->_is_alloc == false);
    if( str->_is_ptr )
        return str->_variant.view_ptr;
    return &(str->_variant.view);
}

usize ryUSTR_DynStr_get_len( const struct ryUSTR_DynStr * str ) {
    if( str->_is_alloc )
        return ryUSTR_AllocStr_get_len(ryUSTR_DynStr_cget_alloc(str));
    else
        return ryUSTR_StrView_get_len(ryUSTR_DynStr_cget_view(str));
}

const u8 * ryUSTR_DynStr_get_buf( const struct ryUSTR_DynStr * str ) {
    if( str->_is_alloc )
        return ryUSTR_AllocStr_get_buf(ryUSTR_DynStr_cget_alloc(str));
    else
        return ryUSTR_StrView_get_buf(ryUSTR_DynStr_cget_view(str));
}