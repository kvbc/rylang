#ifndef RY_ARRAY_H
#define RY_ARRAY_H

#include "../core.h"
#include <stdint.h>

struct ry_Array {
    size_t len; // items
    size_t cap; // items
    void * data;
    size_t itemsz; // bytes
};

void ry_Array_init (struct ry_Array * arr, size_t itemsz) {
    arr->len = 0;
    arr->cap = 0;
    arr->data = NULL;
    arr->itemsz = itemsz;
}

void ry_Array_free (struct ry_Array * arr) {
    arr->len = 0;
    arr->cap = 0;
    RY_FREE(arr->data);
}

void ry_Array_push (struct ry_Array * arr, const void * data) {
    if( arr->len >= arr->cap ) {
        if( arr->cap == 0 )
            arr->cap = 5;
        arr->cap += arr->cap / 2;
        arr->data = RY_REALLOC(arr->data, arr->cap * arr->itemsz);
    }
    RY_MEMCPY(
        (uint8_t*)arr->data + (arr->len * arr->itemsz),
        data, arr->itemsz
    );
    arr->len++;
}

void * ry_Array_get (struct ry_Array * arr, size_t idx) {
    RY_ASSERT(idx < arr->len);
    return (uint8_t*)arr->data + (idx * arr->itemsz);
}

void ry_Array_clear (struct ry_Array * arr) {
    arr->len = 0;
}

#endif // RY_ARRAY_H