#ifndef RY_OUT_H
#define RY_OUT_H

#include "../ry.h"
#include "../util/str/str_view.h"
#include "../util/str/alloc_str.h"
#include "../util/str/dyn_str.h"

void ryO_vprintf( const u8 * fmt, va_list args );
void ryO_printf( const u8 * fmt, ... );

void ryO_print_char( u8 c );

void ryO_print_cstr   ( const u8 * buf );
void ryO_print_cstrlen( const u8 * buf, usize len );
void ryO_print_StrView ( const struct ryUSTR_StrView * view );
void ryO_print_AllocStr( const struct ryUSTR_AllocStr * str );
void ryO_print_DynStr  ( const struct ryUSTR_DynStr * str );

#endif // RY_OUT_H