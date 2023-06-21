#include "out.h"
#include <stdio.h>
#include <stdarg.h>
#include "../util/str/cstr.h"

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
    ryO_print_cstrlen(buf, ryUSTR_cstr_len(buf));

}
void ryO_print_cstrlen( const u8 * buf, usize len ) {
    fwrite(buf, sizeof(*buf), len, stdout);
}

void ryO_print_StrView( const struct ryUSTR_StrView * view ) {
    ryO_print_cstrlen(
        ryUSTR_StrView_get_buf(view),
        ryUSTR_StrView_get_len(view)
    );
}
void ryO_print_AllocStr( const struct ryUSTR_AllocStr * str ) {
    ryO_print_cstrlen(
        ryUSTR_AllocStr_get_buf(str),
        ryUSTR_AllocStr_get_len(str)
    );
}
void ryO_print_DynStr( const struct ryUSTR_DynStr * str ) {
    ryO_print_cstrlen(
        ryUSTR_DynStr_get_buf(str),
        ryUSTR_DynStr_get_len(str)
    );
}