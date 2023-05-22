#ifndef RY_STR_H
#define RY_STR_H

#include "core.h"
#include "str_view.h"

struct ry_String {
    RY_SIZE_T len;
    char * buf;
    struct ry_StringView view;
};

// 
// 
// 

void ry_String_fromview( struct ry_String * str, const struct ry_StringView * view ) {
    str->buf = RY_MALLOC(view->len + 1);
    RY_MEMCPY(str->buf, view->buf, view->len);
    str->buf[view->len] = '\0';
    str->len = view->len;
    str->view = ry_StringView_new(str->buf, str->len);
}

// 
// 
// 

void ry_String_free( struct ry_String * str ) {
    free(str->buf);
}

#endif // RY_STR_H