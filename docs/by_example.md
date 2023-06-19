# [[SOMETHING]] By Example

# 1. Structs

Anonymous structs

```rust
pos [
    x i32;
    y i32;
] = [1; 2];
```

Named structs can be defined by creating a variable that's holding an anonymous struct.

```rust
IntPair = [
    a i32;
    b i32;
]
```

<!--

TODO: Enums

-->

Unions are structs marked with the `#union` metadata

```rust
Register = #union [
    a u8;
    b u8;
    c u8;
]
```

Tuples/Arrays are structs with repeated fields

```rust
xyz [i32 * 3] = [1; 2; 3]
x = xyz..0
y = xyz..1
z = xyz..2
```

```rust
xy [i32; i32] = [1; 2];
```
