#ifndef RYUSTR_ALLOC_STR_H
#define RYUSTR_ALLOC_STR_H

#include "../core.h"
#include "str_view.h"

// 
// 
// 

struct ryUSTR_AllocStr {
    usize _len;
    u8 * _buf;
};

void ryUSTR_AllocStr_init( struct ryUSTR_AllocStr * out_str );
void ryUSTR_AllocStr_free( struct ryUSTR_AllocStr * str );

// 
// Conversions into
// 

// move into
void ryUSTR_AllocStr_move_buflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len );
void ryUSTR_AllocStr_move_buf   ( struct ryUSTR_AllocStr * out_str, u8 * buf );
void ryUSTR_AllocStr_move_alloc ( struct ryUSTR_AllocStr * out_str, struct ryUSTR_AllocStr * str );
// [!] can't move StrView and DynStr
// copy into
void ryUSTR_AllocStr_copy_buflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len );
void ryUSTR_AllocStr_copy_buf   ( struct ryUSTR_AllocStr * out_str, u8 * buf );
void ryUSTR_AllocStr_copy_view  ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_StringView * view );
void ryUSTR_AllocStr_copy_alloc ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_AllocStr * str );
void ryUSTR_AllocStr_copy_dyn   ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_DynStr * str );

// 
// Getters
// 

usize            ryUSTR_AllocStr_getlen( const struct ryUSTR_AllocStr * str );
const u8 * const ryUSTR_AllocStr_getbuf( const struct ryUSTR_AllocStr * str );

// 
// 
// 

#endif // RYUSTR_ALLOC_STR_H