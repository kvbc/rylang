#include "cstr.h"

// djb2 (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long ryUSTR_cstr_hash( const u8 * str, usize len ) {
    unsigned long hash = 5381;
    for( usize i = 0; i < len; i++ )
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}