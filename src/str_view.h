#ifndef RY_STR_VIEW_H
#define RY_STR_VIEW_H

#include "core.h"

struct ry_StringView {
    RY_SIZE_T len;
    const char * buf;
};

// 
// 
// 

struct ry_StringView ry_StringView_new( const char * buf, RY_SIZE_T len ) {
    return (struct ry_StringView) {
        .len = len,
        .buf = buf
    };
}

struct ry_StringView ry_StringView_frombuf( const char * buf ) {
    return ry_StringView_new(buf, strlen(buf));
}

#endif // RY_STR_VIEW_H