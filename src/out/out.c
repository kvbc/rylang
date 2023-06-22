#include "out.h"
#include <stdio.h>
#include <stdarg.h>
#include "../util/str.h"

void ryO_vprintf( const u8 * fmt, va_list args ) {
    vprintf((const char *)fmt, args);
}
void ryO_printf( const u8 * fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    ryO_vprintf(fmt, args);
    va_end(args);
}

// 

void ryO_print_char( u8 c ) {
    putchar(c);
}

// 

void ryO_print_cstr( const u8 * buf ) {
    ryO_print_cstrlen(buf, ryU_cstr_len(buf));

}
void ryO_print_cstrlen( const u8 * buf, usize len ) {
    fwrite(buf, sizeof(*buf), len, stdout);
}

void ryO_print_strview( const struct ryU_ArrView * view ) {
    RY_ASSERT(ryU_ArrView_get_itemsz(view) == 1);
    ryO_print_cstrlen(
        (const u8 *)ryU_ArrView_get_buf(view),
        ryU_ArrView_get_len(view)
    );
}

void ryO_print_str( const struct ryU_Arr * str ) {
    RY_ASSERT(ryU_Arr_get_itemsz(str) == 1);
    ryO_print_cstrlen(
        (const u8 *)ryU_Arr_get_buf(str),
        ryU_Arr_get_len(str)
    );
}

void ryO_print_dynstr( const struct ryU_DynArr * str ) {
    RY_ASSERT(ryU_DynArr_get_itemsz(str) == 1);
    ryO_print_cstrlen(
        (const u8 *)ryU_DynArr_get_buf(str),
        ryU_DynArr_get_len(str)
    );
}