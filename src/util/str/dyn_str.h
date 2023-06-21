#ifndef RYUSTR_DYN_STR_H
#define RYUSTR_DYN_STR_H

#include "../../ry.h"
#include "alloc_str.h"
#include "str_view.h"

// 
// 
// 

struct ryUSTR_DynStr {
    union {
        struct ryUSTR_AllocStr * alloc_ptr;
        struct ryUSTR_StrView * view_ptr;
        struct ryUSTR_AllocStr alloc;
        struct ryUSTR_StrView view;
    } _variant;
    bool _is_alloc;
    bool _is_ptr;
};

struct ryUSTR_AllocStr * ryUSTR_DynStr_init_alloc( struct ryUSTR_DynStr * out_str );
struct ryUSTR_StrView * ryUSTR_DynStr_init_view( struct ryUSTR_DynStr * out_str );
void ryUSTR_DynStr_init_allocptr( struct ryUSTR_DynStr * out_str, struct ryUSTR_AllocStr * alloc_str );
void ryUSTR_DynStr_init_viewptr( struct ryUSTR_DynStr * out_str, struct ryUSTR_StrView * view );
void ryUSTR_DynStr_free( struct ryUSTR_DynStr * str );
// Getters
bool ryUSTR_DynStr_is_alloc( const struct ryUSTR_DynStr * str );
const struct ryUSTR_AllocStr * ryUSTR_DynStr_cget_alloc( const struct ryUSTR_DynStr * str );
const struct ryUSTR_StrView  * ryUSTR_DynStr_cget_view ( const struct ryUSTR_DynStr * str );
struct ryUSTR_AllocStr * ryUSTR_DynStr_get_alloc( struct ryUSTR_DynStr * str );
struct ryUSTR_StrView  * ryUSTR_DynStr_get_view ( struct ryUSTR_DynStr * str );
usize ryUSTR_DynStr_get_len( const struct ryUSTR_DynStr * str );
const u8 * ryUSTR_DynStr_get_buf( const struct ryUSTR_DynStr * str );

// 
// 
// 

#endif // RYUSTR_DYN_STR_H