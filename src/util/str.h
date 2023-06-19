#ifndef RY_STR_H
#define RY_STR_H

#include "../core.h"
#include "str_view.h"

struct ry_String {
    size_t len;
    char * buf;
};

// 
// 
// 

void ry_String_fromview( struct ry_String * str, const struct ry_StringView * view ) {
    str->buf = RY_MALLOC(view->len + 1);
    RY_ASSERT(str->buf != NULL);
    RY_MEMCPY(str->buf, view->buf, view->len);
    str->buf[view->len] = '\0';
    str->len = view->len;
}

// 
// 
// 

void ry_String_free( struct ry_String * str ) {
    free(str->buf);
}

#endif // RY_STR_H