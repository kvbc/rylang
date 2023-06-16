# Features

---

Table of Contents

Feature | Exists
------- | ------
1. [Metaprogramming](#metaprogramming)                                         | ✔️
&emsp; 1.1. [Generics](#generics)                                              | ✔️
&emsp; 1.2. [Macros](#macros)                                                  | ✔️
2. [Namespaces](#namespaces)                                                   | ✔️
3. [Types](#types)                                                             | 
&emsp; 3.1. [Type Constraints](#type-constraints)                              | ✔️
&emsp; 3.2. [Structs / Classes](#structs-or-classes)                           | ✔️
&emsp; &emsp; 3.2.1. [Access specifiers](#access-specifiers)                   | ✔️
&emsp; &emsp; 3.2.2. [Inheritance](#inheritance)                               | ❌
&emsp; &emsp; 3.2.3. [Constructors & Destructors](#constructors-n-destructors) | ❌
&emsp; &emsp; 3.2.4. [Methods](#methods)                                       | ✔️
&emsp; &emsp; 3.2.5. [Operator Overloading](#operator-overloading)             | ❌
&emsp; 3.3. [Unions](#unions)                                                  | ✔️
&emsp; 3.4. [Enums](#enums)                                                    | ✔️
&emsp; 3.5. [Arrays](#arrays)                                                  | ✔️
4. [Variables](#variables)                                                     | ✔️
&emsp; 4.1. [Functions](#functions)                                            | ✔️
5. [Language-level Error Handling](#language-level-error-handling)             | ❌
6. [Metadata](#metadata)                                                       | ✔️
7. [Interfaces](#interfaces)                                                   | ✔️
8. [Modules / Imports](#modules-or-imports)                                    | ✔️

---

## 1. Metaprogramming {#metaprogramming}

...

### 1.1. Generics {#generics}

Compile-time duck typing.

```rust
Pair[T ast:Type] => ast:Struct = [
    a T;
    b T;
]
nums Pair[i32] = [1; 2]
```

### 1.2. Macros {#macros}

...

---

## 2. Namespaces {#namespaces}

Namespaces are just field-less structs.

```rust
a var = [
    b var = [
        c var = [
            @static x i32 = 0;
        ]
    ]
]
a:b:c:x = 1;
```

---

## 3. Types {#types}

...

### 3.1. Type Constraints {#type-constraints}

```rust
A var = [
    x i32 = 69;
    get[ me self ] => i32 = me.x;
]
B var = [
    x i32 = 420;
    get[ me self ] => i32 = me.x;
]

has_getter[ T ast:Node & is_struct ] => bool = T:has_method("get", [] => i32);

get[ obj ast:Node & is_expr & is_struct & has_getter ] => i32 = obj:get[];

main[] => [] = {
    a A = [];
    b B = [];
    a:get[] == get[a]; // true
    b:get[] == get[b]; // true
}

```

### 3.2. Structs / Classes {#structs-or-classes}

```rust
Vec2 var = [
    x i32;
    y i32;
]
pos Vec2 = [1; 2]
```

#### 3.2.1. Access specifiers {#access-specifiers}

Every struct field is private by default

```rust
Vec2 var = [
         x i32;
    @pub y i32;
]
pos Vec2 = [1; 2]
pos.x = 2; // ERROR
pos.y = 1;
```

#### 3.2.2. Inheritance {#inheritance}

Inheritance ❌
Composition ✔️

#### 3.2.3. Constructors & Destructors {#constructors-n-destructors}

...

#### 3.2.4. Methods {#methods}

```rust
Summator var = [
    sum i32 = 0;

    get[ me self ] => i32 = me.sum;
    add[ me self; num i32 ] => [] = me.sum += num;
]
```

#### 3.2.5. Operator overloading {#operator-overloading}

...

### 3.3. Unions {#unions}

Unions are special structs.

```rust
Value var = @union [
    int isize;
    float f32;
]
```

### 3.4. Enums {#enums}

Enums are special structs.

```rust
Color var = @enum [
    RED;
    GREEN;
    BLUE;
]
```

### 3.5. Arrays {#arrays}

Arrays are structs.

```rust
main[] => [] = {
    arr [i32 * 3] = [1; 2; 3];
    arr..0 = arr..1; // [2; 2; 3]
}
```

---

## 4. Variables {#variables}

```rust
x u32 = 0;
```

### 4.1. Functions {#functions}

```rust
add[ a i32; b i32 ] => i32 = a + b;
main[] => [] = {
    a i32 = 2;
    b i32 = 3;
    x i32 = add[a; b]
    // x == 5
}
```

---

## 5. Language-level Error Handling {#language-level-error-handling}

...

---

## 6. Metadata {#metadata}

```rust
@static x i32 = 0;
```

---

## 7. Interfaces {#interfaces}

See [Type Constraints](#type-constraints)

---

## 8. Modules / Imports {#modules-or-imports}

...