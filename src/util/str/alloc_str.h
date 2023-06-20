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

// init : move
void ryUSTR_AllocStr_init_movebuflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len );
void ryUSTR_AllocStr_init_movebuf   ( struct ryUSTR_AllocStr * out_str, u8 * buf );
void ryUSTR_AllocStr_init_movealloc ( struct ryUSTR_AllocStr * out_str, struct ryUSTR_AllocStr * str );
// init : copy
void ryUSTR_AllocStr_init_copybuflen( struct ryUSTR_AllocStr * out_str, u8 * buf, usize len );
void ryUSTR_AllocStr_init_copybuf   ( struct ryUSTR_AllocStr * out_str, u8 * buf );
void ryUSTR_AllocStr_init_copyview  ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_StringView * view );
void ryUSTR_AllocStr_init_copyalloc ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_AllocStr * str );
void ryUSTR_AllocStr_init_copydyn   ( struct ryUSTR_AllocStr * out_str, const struct ryUSTR_DynStr * str );

void ryUSTR_AllocStr_free( struct ryUSTR_AllocStr * str );

// Getters
usize            ryUSTR_AllocStr_getlen( const struct ryUSTR_AllocStr * str );
const u8 * const ryUSTR_AllocStr_getbuf( const struct ryUSTR_AllocStr * str );

// 
// 
// 

#endif // RYUSTR_ALLOC_STR_H