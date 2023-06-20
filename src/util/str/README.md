| Module    | Struct     | Description                                     |
| --------- | ---------- | ----------------------------------------------- |
| cstr      | -          | Utility functions for C-style strings (buffers) |
| str_view  | `StrView`  | String View                                     |
| alloc_str | `AllocStr` | Allocated String                                |
| dyn_str   | `DynStr`   | Either an `AllocStr` or `DynStr`                |

```mermaid
graph TD
    cstr --> AllocStr & StrView
    AllocStr & StrView <--> DynStr
    subgraph " "
        AllocStr <---> |conversion| StrView
    end
```

---

**AllocStr**
[alloc_str.h](alloc_str.h)

```mermaid
---
title: Initializing an AllocStr
---
graph LR
    cstr -- init_movebuf / init_movebuflen ----> A[AllocStr]
    cstr -- init_copybuf / init_copybuflen ----> A[AllocStr]

    B[AllocStr] -- init_movealloc ----> C[AllocStr]
    B[AllocStr] -- init_copyalloc ----> C[AllocStr]

    StrView -- init_copyview ----> D[AllocStr]
    StrView x--x |CAN'T MOVE| D[AllocStr]

    DynStr -- init_copydyn ----> AllocStr
    DynStr x--x |CAN'T MOVE| AllocStr
```

---

**StrView**
[str_view.h](str_view.h)

As this is a "view", all `init` functions move the data and do not copy it.

```mermaid
---
title: Initializing a StrView
---
graph LR
    cstr -- init_buf / init_buflen ----> A[StrView]

    B[StrView] ----> |init_view| C[StrView]

    AllocStr ----> |init_alloc| D[StrView]

    DynStr -- init_dyn ----> StrView
```
