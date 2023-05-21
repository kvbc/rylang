#ifndef RY_CSTR_H
#define RY_CSTR_H

#include "core.h"

// djb2 (http://www.cse.yorku.ca/~oz/hash.html)
static unsigned long ry_cstr_hash (const char * str, RY_SIZE_T len) {
    unsigned long hash = 5381;
    for( RY_SIZE_T i = 0; i < len; i++ )
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}

#endif // RY_CSTR_H