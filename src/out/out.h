#ifndef RY_OUT_H
#define RY_OUT_H

#include "../ry.h"
#include "../util/arr_view.h"
#include "../util/arr.h"
#include "../util/dyn_arr.h"

void ryO_vprintf( const u8 * fmt, va_list args );
void ryO_printf( const u8 * fmt, ... );

void ryO_print_char( u8 c );

void ryO_print_cstr   ( const u8 * buf );
void ryO_print_cstrlen( const u8 * buf, usize len );
void ryO_print_strview( const struct ryU_ArrView * view );
void ryO_print_str    ( const struct ryU_Arr * str );
void ryO_print_dynstr ( const struct ryU_DynArr * str );

#endif // RY_OUT_H