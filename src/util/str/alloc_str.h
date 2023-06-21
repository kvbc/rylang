#ifndef RYUSTR_ALLOC_STR_H
#define RYUSTR_ALLOC_STR_H

#include "../../ry.h"

struct ryUSTR_StrView;
struct ryUSTR_DynStr;

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
void ryUSTR_AllocStr_copy_buflen( struct ryUSTR_AllocStr * out_str, const u8 * buf, usize len );
void ryUSTR_AllocStr_copy_buf   ( struct ryUSTR_AllocStr * out_str, const u8 * buf );
void ryUSTR_AllocStr_copy_view  ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_StrView * view );
void ryUSTR_AllocStr_copy_alloc ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_AllocStr * str );
void ryUSTR_AllocStr_copy_dyn   ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_DynStr * str );

// 
// Getters
// 

usize      ryUSTR_AllocStr_get_len( const struct ryUSTR_AllocStr * str );
const u8 * ryUSTR_AllocStr_get_buf( const struct ryUSTR_AllocStr * str );

// 
// 
// 

#endif // RYUSTR_ALLOC_STR_H