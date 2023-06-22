#include "str.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// 
// 
// 

void ryU_str_format( struct ryU_Arr * out_str, const u8 * fmt, ... ) {
    va_list args;
    va_start(args, fmt);

    usize len = vsnprintf(NULL, 0, (const char *)fmt, args);
    u8 * buf = (u8 *)RY_MALLOC(len);
    vsnprintf((char *)buf, len + 1, (const char *)fmt, args);
    ryU_Arr_move_buf(out_str, buf, len);
    
    va_end(args);
}

// djb2 (http://www.cse.yorku.ca/~oz/hash.html)
usize ryU_cstr_hash( const u8 * str, usize len ) {
    usize hash = 5381;
    for( usize i = 0; i < len; i++ )
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}

usize ryU_cstr_len( const u8 * str ) {
    return strlen((const char *)str);
}

const u8 * ryU_cstr( const char * str ) {
    return (const u8 *)str;
}