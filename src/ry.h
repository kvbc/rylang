#ifndef RY_CORE_H
#define RY_CORE_H

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define RY_DEBUG 1 // FIXME

#define RY_MEMCPY(dest, src, sz) (memcpy(dest, src, sz))
#define RY_MALLOC(sz)            (malloc(sz))
#define RY_FREE(ptr)             (free(ptr))
#define RY_REALLOC(ptr, sz)      (realloc(ptr, sz))

#define RY_ASSERT(cond) (assert(cond))

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i64     isize;
// 
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t   usize;
// 
typedef float  f32;
typedef double f64;

#endif // RY_CORE_H