#ifndef RY_CORE_H
#define RY_CORE_H

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define RY_SIZE_T size_t
#define RY_BOOL int
#define RY_TRUE (1)
#define RY_FALSE (0)

#define RY_MEMCPY(dest, src, sz) (memcpy(dest, src, sz))
#define RY_MALLOC(sz) (malloc(sz))
#define RY_FREE(ptr) (free(ptr))
#define RY_REALLOC(ptr, sz) (realloc(ptr, sz))

#define RY_ASSERT(cond) (assert(cond))

typedef uint8_t ry_char_t;
typedef int64_t ry_numlit_t; // number literal type

#define RY_CHAR_MIN (0)
#define RY_CHAR_MAX (255)

#endif // RY_CORE_H