| Module    | Struct     | Description                                     |
| --------- | ---------- | ----------------------------------------------- |
| str       | -          | Global utility functions for all strings        |
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

**AllocStr** \
[alloc_str.h](alloc_str.h)

```mermaid
---
title: Conversions into an AllocStr
---
graph LR
    cstr -- move_buf / move_buflen ----> A[AllocStr]
    cstr -- copy_buf / copy_buflen ----> A[AllocStr]

    B[AllocStr] -- move_alloc ----> C[AllocStr]
    B[AllocStr] -- copy_alloc ----> C[AllocStr]

    StrView -- copy_view ----> D[AllocStr]
    StrView x--x |CAN'T MOVE| D[AllocStr]

    DynStr -- copy_dyn ----> AllocStr
    DynStr x--x |CAN'T MOVE| AllocStr
```

---

**StrView** \
[str_view.h](str_view.h)

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
