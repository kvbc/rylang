#include "str.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// 
// 
// 

void ryUSTR_format( struct ryUSTR_AllocStr * out_str, const char * fmt, ... ) {
    va_list args;
    va_start(args, fmt);

    usize len = vsprintf(NULL, fmt, args);
    char * msg = RY_MALLOC(len + 1);
    vsnprintf(msg, len + 1, fmt, args);
    ryUS
    
    va_end(args);
}