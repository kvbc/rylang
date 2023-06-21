#include "str.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// 
// 
// 

void ryUSTR_format( struct ryUSTR_AllocStr * out_str, const u8 * fmt, ... ) {
    va_list args;
    va_start(args, fmt);

    usize len = vsnprintf(NULL, 0, (const char *)fmt, args);
    u8 * buf = (u8 *)RY_MALLOC(len);
    vsnprintf((char *)buf, len + 1, (const char *)fmt, args);
    ryUSTR_AllocStr_move_buflen(out_str, buf, len);
    
    va_end(args);
}