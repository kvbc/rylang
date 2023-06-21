#include "cstr.h"

// djb2 (http://www.cse.yorku.ca/~oz/hash.html)
usize ryUSTR_cstr_hash( const u8 * str, usize len ) {
    usize hash = 5381;
    for( usize i = 0; i < len; i++ )
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}

usize ryUSTR_cstr_len( const u8 * str ) {
    return strlen((const char *)str);
}

const u8 * ryUSTR_cstr( const char * str ) {
    return (const u8 *)str;
}