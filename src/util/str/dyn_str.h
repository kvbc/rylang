#ifndef RYUSTR_DYN_STR_H
#define RYUSTR_DYN_STR_H

#include "../../core.h"
#include "alloc_str.h"
#include "str_view.h"

// 
// 
// 

struct ryUSTR_DynStr {
    union {
        struct ryUSTR_AllocStr alloc;
        struct ryUSTR_StringView view;
    } _variant;
    bool _is_alloc;
};

struct ryUSTR_AllocStr * ryUSTR_DynStr_init_alloc( struct ryUSTR_DynStr * out_str );
struct ryUSTR_StringView * ryUSTR_DynStr_init_view( struct ryUSTR_DynStr * out_str );
void ryUSTR_DynStr_free( struct ryUSTR_DynStr * str );
// Getters
bool ryUSTR_DynStr_is_alloc( const struct ryUSTR_DynStr * str );
struct ryUSTR_AllocStr * ryUSTR_DynStr_get_alloc( const struct ryUSTR_DynStr * str );
struct ryUSTR_StringView * ryUSTR_DynStr_get_view( const struct ryUSTR_DynStr * str );

// 
// 
// 

#endif // RYUSTR_DYN_STR_H