#ifndef RYUSTR_CSTR_H
#define RYUSTR_CSTR_H

#include "../../ry.h"

usize ryUSTR_cstr_hash( const u8 * str, usize len );
usize ryUSTR_cstr_len ( const u8 * str );
const u8 * ryUSTR_cstr( const char * str );

#endif // RYUSTR_CSTR_H