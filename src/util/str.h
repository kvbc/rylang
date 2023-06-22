#ifndef RYU_STR_H
#define RYU_STR_H

#include "../ry.h"
#include "arr.h"

void ryU_str_format( struct ryU_Arr * out_str, const u8 * fmt, ... );

// cstr

usize ryU_cstr_hash( const u8 * str, usize len );
usize ryU_cstr_len ( const u8 * str );
const u8 * ryU_cstr( const char * str );

#endif // RYU_STR_H