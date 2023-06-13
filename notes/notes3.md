Other Languages
- [x] [zig](https://ziglang.org/documentation/master/)
- [ ] [D](https://dlang.org/spec/spec.html)
- [x] [odin](https://odin-lang.org/docs/overview/) - no spec
- [ ] [rust](https://www.rust-lang.org/learn) - no spec
- [ ] [go](https://go.dev/ref/spec)
- [ ] [nim](https://nim-lang.org/docs/manual.html)

---

Useful
- https://rustc-dev-guide.rust-lang.org/part-2-intro.html

---

Consider
- pointer arrays - `Types`
- see `notes/func.rs` for new `<func_type>` ideas.
- function parameters immutable - `<func_type>`
- anon struct types - `<struct_type>` (see notes/notes4.rs)
- see [playground/main.rs](playground/main.rs)

---

Table of Contents

Chapter | Syntax | Info
------- | :----: | ---- 
1. [Lexical Analysis](#lexical-analysis)                          | ✔️
&emsp; 1.1. [Names](#names)                                       | ✔️
&emsp; 1.2. [Comments](#comments)                                 | ✔️
&emsp; 1.3. [Literals](#literals)                                 | ✔️
&emsp; &emsp; 1.3.1. [Integer Literals](#integer-literals)        | ✔️ 
&emsp; &emsp; 1.3.2. [Float Literals](#float-literals)            | ✔️ 
&emsp; &emsp; 1.3.3. [String Literals](#string-literals)          | ✔️ 
&emsp; &emsp; 1.3.4. [Struct Literals](#struct-literals)          | ✔️
&emsp; 1.4. [Semicolons](#semicolons)                             | ✔️
2. [Definitions](#definitions)                                    | ✔️ 
&emsp; 2.1. [Variables](#variables)                               | ✔️ 
&emsp; 2.2. [Functions](#functions)                               | ✔️ 
&emsp; 2.3. [Struct](#struct)                                     | ✔️ 
&emsp; &emsp; 2.3.1. [Union](#union)                              | ✔️ 
&emsp; 2.4. [Enum](#enum)                                         | ✔️ 
&emsp; 2.5. [Namespace](#namespace)                               | ✔️ 
&emsp; &emsp; 2.5.1. [Modules](#modules)                          | ✔️
&emsp; &emsp; 2.5.2. [Use](#use)                                  | ✔️
3. [Operators](#operators)                                        |
&emsp; 3.1. [Arithmetic Operators](#arithmetic-operators)         |
&emsp; 3.2. [Bitwise Operators](#bitwise-operators)               |
&emsp; 3.3. [Comparison Operators](#comparison-operators)         |
&emsp; 3.4. [Logical Operators](#logical-operators)               |
&emsp; 3.5. [Other Operators](#other-operators)                   |
4. [Expressions](#expressions)                                    | ✔️ 
&emsp; 4.1. [Block](#block) (& `break`)                           | ✔️ 
&emsp; 4.2. [Control Flow](#control-flow)                         | ✔️ 
&emsp; &emsp; 4.2.1. [If / Elif / Else](#if-elif-else)            | ✔️ 
&emsp; &emsp; 4.2.2. [Loop](#loop)                                | ✔️ 
&emsp; &emsp; &emsp; 4.2.2.1. [Continue](#continue)               | ✔️
&emsp; 4.3. [Compile-time Expressions](#compile-time-expressions) | ✔️
5. [Statements](#statements)                                      |
6. [Types](#types)                                                |
7. [Macros](#macros)                                              |
8. [Metadata](#metadata)                                          | ✔️

---

Good info
- TBD

---

# 1. Lexical Analysis {#lexical-analysis}

**Syntax**

Tag | Syntax
--- | ------
\<new_line> | `\n | \r | \r\n` or `LF | CR | CRLF`
\<src_char> | Any valid [UTF-8](https://en.wikipedia.org/wiki/UTF-8) character (0 - 255)

**Interpretation**

- The only valid line encodings are:
  - LF
  - CR
  - CRLF
- The source code is written in the UTF-8 encoding.

## 1.1. Names {#names}

**Syntax**

Tag | Syntax
--- | ------
\<name>            | `<name_start_char> {<name_char>}`
\<name_start_char> | `_ | a-z | A-Z`
\<name_char>       | `<name_start_char> | 0-9`

**Interpretation**

- A name cannot start with a digit.
- Names (identifiers) can only consist of:
  - underscores `_`,
  - lowercase and uppercase letters from `aA` to `zZ`,
  - digits from `0` to `9`

**Examples**

```
_var
a_b_c
x__
word
_0
__Big_Word__

1_3      // invalid
2_plus_2 // invalid
```

## 1.2. Comments {#comments}

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<comment> | `// {<src_char>} [<new_line>]` where `<src_char>` is not `<new_line>` | Single-line comment
\<comment> | `/* {<src_char>} */` | Multi-line comment
&emsp; \<src_char>, <br> \<new_line> | See [Lexical Analysis](#lexical-analysis)

**Context**

- Multiple multi-line comments can be nested inside of each other.

**Examples**

```rust
// single-line comment
```
```rust
/*
multi
line
comment
*/

/* or single line */
```

## 1.3. Literals {#literals}

### 1.3.1. Integer Literals {#integer-literals}

**Syntax**

Tag(s) | Syntax | Comment
------ | ------ | -------
\<dec_int>, <br> \<integer> | `0-9 {[_]0-9}` | Decimal integer literal
\<integer> | `0b 0|1 {[_]0|1}` | Binary integer literal
\<integer> | `0x <hex_digit> {[_]<hex_digit>}` | Hexadecimal integer literal
&emsp; \<hex_digit> | `(0 - 9) | (a - z) | (A - Z)` | Hexadecimal digit
\<integer> | `0o 0-7 {[_]0-7}` | Octal integer literal

**Examples**

```rust
// decimal
00100 // == 100
2_0_1 // == 201
69

// bin
0b1111_1111_1111_0000 // == 255.255.255.0
0b0_0_0_1 // == 1
0b110 // == 6

// hex
0x7A
0x6_9
0x0_01_35 // == 0x135

// oct
0o23
0o7_7
0o1_2_3

_420   // INVALID
2137_  // INVALID
_1_    // INVALID
0x_1   // INVALID
0x23_  // INVALID
0x_0_  // INVALID
```

### 1.3.2. Float Literals {#float-literals}

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<float> | `<dec_int> . <dec_int> [<float_exp>]`
\<float> | `<dec_int> <float_exp>`
\<float_exp> | `e +|- <dec_int>` | Exponent
&emsp; \<dec_int> | See [Integer Literals](#integer-literals)

**Examples**

TODO

### 1.3.3. String Literals {#string-literals}

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<string> | `" {<src_char>} "` | Raw string literal
\<string> | `` ` {<src_char> | \<string_esc_seq>} ` `` where `<src_char>` is not `\` | Escapable string literal
\<string_esc_seq> | <code>a \| b \| e \| f \| n \| r \| t \| v \| ` \| \ </code>
\<string_esc_seq> | `<integer>`
&emsp; \<src_char> | See [Lexical Analysis](#lexical-analysis)
&emsp; \<integer>  | See [Integer Literals](#integer-literals)

**Interpretation**

TODO

**Examples**

TODO

### 1.3.4. Struct Literals {#struct-literals}

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<struct_literal> | `{ {<struct_literal_field>} }`
\<struct_literal_field> | `[<name> =] <expr> [;]` see **Comment** | The semicolon `;` can only be omitted if it's the last field.
&emsp; \<name> | See [Names](#names)
&emsp; \<expr> | See [Expressions](#expressions)

**Interpretation**

TODO

**Examples**

TODO

## 1.4. Semicolons {#semicolons}

TODO

# 2. Definitions {#definitions}

## 2.1. Variables {#variables}

A variable is a named container for data.

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<var>         | `<name> <type> = <expr> ;` where `<type>` is neither `<struct_type>` nor `<func_type>` | Non-struct variable
\<var>         | `<name> <struct_type> = <struct_literal> ;`                                            | Struct literal
\<var>         | `<name> <struct_type> = <expr> ;` where `<expr>` results in type `<struct_type>`       | Struct copy
&emsp; \<name>           | See [Names](#names)
&emsp; \<expr>           | See [Expressions](#expressions)
&emsp; \<struct_literal> | See [Literals](#literals)
&emsp; \<type>, <br> &emsp; \<struct_type>, <br> &emsp; \<func_type> | See [Types](#types)

**Parentship**

Tag | Parent | Comment
--- | ------ | -------
\<var> | `<func_block>`, all sub-blocks of `<func_block>` | See **Interpretation**

**Context**

TODO

- Variable shadowing is disallowed.
  ```rust
  main ()void = {
	x u8 = 1;
    {
        x u8 = 2; // ERROR: variable redefinition
    }
    x u8 = 3; // ERROR: variable redefinition
  }
  ```

**Interpretation**

- See `<name>` in [Names](#names) for the rules behind a valid variable name (identifier).
- See `<expr>` in [Expressions](#expressions) for what a non-struct variable can be assigned.
- All variables must be initialized.
- Variables can only exist inside of functions and their sub-blocks - there can be no global variables.
- Variables of type *struct* `<struct_type>` must be initialized either by copy or by using the struct literal.
  - See [Struct](#struct).
  - See `<struct_type>` in [Types](#types).
  - See [Literals](#literals) for the definition of a *struct literal*.
  ```rust
  main ()void = {
    $Vector2 = { x i32; y i32 };
    a $Vector2 = { // literal
        .x = 0,
        .y = 0
    };
    // or
    b $Vector2 = a; // copy
    c $Vector2 = { // copy
        break b;
    };
  };
  ```
  This supposed copy might not always result in an actual copy of the data. \
  The compiler is expected to, where applicable, prefer "moving" the data instead of actually copying it. \
  One such example can be observed below.
  ```rust
  $Vector2 = {
    x i32;
    y i32;
    new( x i32, y i32 ) $Vector2 = {
        v $Vector2 = {
            .x = x,
            .y = y
        };
        break v;
    }
  };
  main() void = {
    pos $Vector2 = :$Vector2:new(0, 0);
  }
  ```
  TODO: Talk about how it's done in C - returning a struct by value in C (implicit return pointer)

**Examples**

```rust
val u8 = 10;
ptr *u8 = &val;
```

```rust
$Vector2 = { x i32; y i32; };
pos $Vector2 = {
    .x = 0,
    .y = 0
};
```

```rust
$A = { x j32; };
$B = { a $A; };
b $B = {
    .a = {
        .x = 3;
    };
};
```

## 2.2. Functions {#functions}

A function is an ordered sequence of statements that get executed whenever you call it.
  - See [Statements](#statements) for the definition of a statement `<stmt>`.
  - See [Namespace](#namespace) for the definition of a namespace statement `<namespace_stmt>`.

**Syntax**

Tag | Syntax
--- | ------
\<func>       | `<name> <func_type> = <func_block> ;`
\<func_block> | `{ <func_stmt> {<func_stmt>} }`
\<func_stmt>  | `<stmt> | <namespace_stmt>`
&emsp; \<func_type>      | See [Types](#types)
&emsp; \<name>           | See [Names](#names)
&emsp; \<stmt>           | See [Statements](#statements)
&emsp; \<namespace_stmt> | See [Namespace](#namespace)

**Parentship**

Tag | Parent | Comment 
--- | ------ | -------
\<func> | \<namespace> | See **Interpretation**

**Context**

TODO

- See [Operators](#operators) for the function call operator `()` and it's specific language features.
- Functions are also namespaces.
  - See [Namespaces](#namespace).
  ```rust
  xadd (a i32, b i32) :xadd():$Result = {
    $Result = {
      val i32;
      ok bool;
    };
    res $Result;
    res.val = a + b;
    res.ok = (a != b);
    break res;
  };
  res :xadd():$Result = xadd(1, 2);
  ```
- Sub-functions are (by default) not closures - they can't access variables of outer scope. \
  You can mark a sub-function as a closure by using the `@closure` metadata, \
  allowing it to access variables of the same scope that the sub-function has been defined in.
  ```rust
  main ()void {
    a i32 = 1;
    b i32 = 2;
    // @closure // fine
    add ()i32 {
        break a + b; // ERROR
    }
    c i32 = add();
  }
  ```
  ```rust
  main ()void = {
    a i32 = 1;
    b i32 = 2;
    @closure add_1 ()i32 = {
        // @closure // fine
        add_2 ()i32 = {
            break a + b; // ERROR: both add_1() and add_2() must be closures
        }
        break add_2();
    }
    c i32 = add_1();
  }
  ```
  ```rust
  main ()void = {
    a i32 = 1;
    b i32 = 2;
    // @closure // fine
    add_1 ()i32 = {
        @closure add_2 ()i32 = {
            break a + b; // ERROR: add_1() must be a closure itself to allow for sub-closures
        }
        break add_2();
    }
    c i32 = add_1();
  }
  ```

**Interpretation**

- See `<name>` in [Names](#names) for the rules behind a valid function name (identifier).
- See `<func_type>` in [Types](#types) for all about function parameters and return semantics.
- See `<stmt>` in [Statements](#statements) and `<namespace_stmt>` in [Namespace](#namespace) for all the statements that can be used inside of a function body.
- Functions can only be defined inside of namespaces.
  - See [Namespaces](#namespace)
  
**Examples**

```rust
add (a i32, b i32)i32 = {
    break a + b;
}
main ()void = {
    a i32 = 1;
    b i32 = 2;
    c i32 = add(a, b);
}
```

## 2.3. Struct {#struct}

A *struct* (Structure) is a collection of variables (fields).

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<struct>        | `$ <name> = <struct_block> ;`
\<struct_block>  | `{ <struct_stmt> {<struct_stmt>} }`
\<struct_stmt>   | `<struct_field> | <namespace_stmt>`
\<struct_field>  | `<name> <type> [= <compexpr>] ;` where `<type>` isn't the defined struct itself | <li>The semicolon `;` might be omitted if it's the last field in a struct.</li> <li>Struct fields follow the same rules as regular [variables](#1-variables)</li>
&emsp; \<name>           | See [Names](#names)
&emsp; \<var>            | See [Variables](#variables)
&emsp; \<var_type>       | See [Types](#types)
&emsp; \<namespace_stmt> | See [Namespace](#namespace)
&emsp; \<compexpr>       | See [Expressions](#expressions)
&emsp; \<struct_literal> | See [Literals](#literals)

**Parentship**

Tag | Parent | Comment
--- | ------ | ------
\<struct> | \<namespace> | See [Namespace](#namespace)

**Context**

TODO

**Interpretation**

- A *struct* is also a namespace.
  - See [Namespace](#namespace).
  ```rust
    $Vector2 = {
      ^int = i32;
      x ^int;
      y ^int;
    };
  ```
- Structs can only be defined inside of namespaces.
  - See [Namespace](#namespace).
- A *struct* cannot be empty.
  ```rust
  $Vector2 = {} // ERROR
  ```
- See `<struct_type>` in [Types](#types) for anonymous structs.
- A *struct* field cannot be of the same type as the defined struct.
  ```rust
  $A = {
    a $A; // ERROR
  };
  ```
- *Struct* fields follow the same rules as regular variables.
  - See [Variables](#variables).
- *Struct* fields can be assigned default values.
  ```rust
  $Vector2 = {
    x i32 = 0;
    y i32 = 0;
  };
  ```
- *Struct* fields can be accessed using the dot `.` operator.
  - See [Operators](#operators) for the *struct* field access operator
  ```rust
  pos $Vector2;
  pos.x = 0;
  pos.y = 0;
  x i32 = pos.x;
  ```
- Functions declared inside of a *struct*, with their first parameter being of the defined *struct* type, are called *methods*. \
  You can call them on a struct instance using the colon `:` operator.
  - See [Operators](#operators) for the *struct* *method* access operator.

  The first "self" argument is excluded from a *method* call. \
  This *struct* *method* syntax is nothing but syntactic sugar for plain-old *namespace* access.
  ```rust
  $Vector2 = {
      x i32;
      y i32;
  
      add( self !*$Vector2, other !*!$Vector2 ) !*$Vector2 = {
          self.x += other.x;
          self.y += other.y;
          break self;
      }
  };
  
  main() void {
      a $Vector2 = { .x = 1; .y = 1; };
      b $Vector2 = { .x = 2; .y = 2; };
  
      a:add(b);
  
      // is the same as
  
      $Vector2:add(&a, b);
  }
  ```
- Examples:
  ```rust
  $Vector2 = {
      x i32;
      y i32;
      new (x i32, y i32) $Vector2 = {
          v $Vector2 = {
            .x = x,
            .y = y
          };
          break v; 
      }
  };
  main ()void = {
    pos $Vector2 = $Vector2:new(0, 0);
  }
  ```

## 2.3.1. Union {#union}

**Syntax**

TODO

**Interpretation**

TODO

**Examples**

```rust
@union $Value = {
  int isize = 0;
  ok bool = false;
  float f32 = 0.0;
};
main ${} -> void = {
  v $Value = {};
  v.int = 10;
  // ...
  v.ok = true;
  // ...
  v.float = 3.141;
};
```

## 2.4. Enum {#enum}

An Enum (Enumeration) is a collection of scoped, named & unique integer values (fields)

**Syntax**

Tag | Syntax
--- | ------
\<enum>        | `# <name> = <enum_block> ;`
\<enum_block>  | `{ <enum_stmt> {<enum_stmt>} }`
\<enum_stmt>   | `<enum_field> | <namespace_stmt>`
\<enum_field>  | `<name> [= <compexpr>] ;` where `<compexpr>` results in type `i32`
\<enum_access> | `<enum_type> : <name>`
&emsp; \<name>           | See [Names](#names)
&emsp; \<namespace_stmt> | See [Namespace](#namespace)
&emsp; \<number>         | See [Literals](#literals)
&emsp; \<compexpr>       | See [Expressions](#expressions)

**Parentship**

Tag | Parent | Comment
--- | ------ | ------
\<enum> | \<namespace> | See [Namespace](#namespace)

**Context**

TODO

- The first enum field, if not explicitely set, is equal to 0.
- Each next enum field, if not explicitely set, is 1 higher than the previous value. 

**Interpretation**

- Enum fields are of type `i32`
  - See `<primitive>` in [Types](#types) for the type of `i32`.
- Enums can only be defined inside of namespaces.
  - See [Namespace](#namespace)
 Enums are also namespaces.
  - See [Namespace](#namespace).
- Enum fields can be accessed using the colon `:` operator
  - See [Operators](#operators) for the *enum field access* operator.
  ```rust
  var #Enum = #Enum:FIELD;
  ```
- *Enum* fields can be explicitely set.
  - The set value must be a *compile-time* expression resulting in a value of type `i32`.
    - See [Expressions](#expressions) for the definition of a *compile-time* expression `compexpr`.
    ```rust
    x i32 = 10;
    #Color = {
        RED = x, // ERROR
        GREEN = 0.5, // ERROR
    };
    ```
  - The set value must be higher than the previous values.
    ```rust
    #Color = {
        RED, // 0
        GREEN, // 1
        BLUE = 1 // ERROR
    }
    ```
  - The fields can be set even multiple times
    ```rust
    #Animal = {
        DOG, // 0
        CAT = 10,
        WOLF, // 11
        LION, // 12
        SQUID = 100, 
        HORSE // 101
    }
    ```
    
**Examples**

```rust
#Color = {
  RED;   // 0
  GREEN; // 1
  BLUE;  // 2
};
c #Color = #Color:RED;
```

## 2.5. Namespace {#namespace}

A namespace is a scoped collection of:
  - [Functions](#functions),
  - [Structs](#struct),
  - [Enums](#enum),
  - [Aliases](#alias), and
  - Other namespaces

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<namespace>        | `: <name> = <namespace_block> ;`
\<namespace>        | `: <name> = <string> ;` | Module import
\<namespace_block>  | `{ <namespace_stmt> {<namespace_stmt>} }`
\<namespace_stmt>   | `<func> | <struct> | <enum> | <namespace>`
\<namespace_entity> | `<struct_type> | <enum_type> | <name> | <name>()` | Struct / Enum / Namespace / Function
\<namespace_access> | `<namespace_entity> : <namespace_entity> {: <namespace_entity>}`
&emsp; \<name>   | See [Names](#names)
&emsp; \<string> | See [Literals](#literals)
&emsp; \<func>   | See [Functions](#functions)
&emsp; \<struct> | See [Struct](#struct)
&emsp; \<enum>   | See [Enum](#enum)
&emsp; \<alias>  | See [Alias](#alias)

**Parentship**

Tag | Parent | Comment
--- | ------ | -------
\<namespace> | \<namespace> | See **Context**

**Context**

- The global scope is also considered a namespace.

**Interpretation**

- Namespaces can be imported from other files. This concept forms the basis of [Modules](#modules).
- [Functions](#functions), [Structs](#struct) and [Enums](#enum) are all namespaces.
- To access a namespace member, you can use the colon `:` operator
  - ```rust
    :math = {
        $Vector2 = { x i32; y i32; };
    };
    pos :math:$Vector2;
    ```

**Examples**

TODO

### 2.5.1. Modules {#modules}

```rust
// main.ry
:util = "util.ry";
:^math = :util:math;
$^Vector2 = :^math:$Vector2;

main ()void = {
  pos $^Vector2;
  pos.x = 0;
  pos.y = 0;
}
```

```rust
// util.ry
@export :math = "util/math.ry";
```

```rust
// util/math.ry
@export $Vector2 = {
  x i32;
  y i32;
}
```

### 2.5.2. Use {#use}

**Syntax**

Tag | Syntax
--- | ------
\<use> | `use (<namespace_entity> | <namespace_access>) ;`
&emsp; \<namespace_entity>, <br> \<namespace_access> | See [Namespace](#namespace)

**Interpretation**

TODO

**Examples**

```rust
:a = {
  :b = {
    :c = {
      $Vector2 = {
        x isize;
        y isize;
      };
    };
  };
};

main ${} -> void = {
  {
    v a:b:c:$Vector2 = {
      x = 1;
      y = 2;
    };
  };
  {
    use a:b:c;
    v $Vector2 = {1; 2};
  };
};
```

# 3. Operators {#operators}

## 3.1. Arithmetic Operators {#arithmetic-operators}

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Unary  | `-`   | negation           | `- <expr>`
Binary | `+`   | addition           | `<expr> + <expr>`
Binary | `-`   | subtraction        | `<expr> - <expr>`
Binary | `*`   | multiplication     | `<expr> * <expr>`
Binary | `/`   | division           | `<expr> / <expr>`
Binary | `%`   | modulo (remainder) | `<expr> % <expr>`

Assignment

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Binary | `+=`   | addition           | `<name> += <expr> ;`
Binary | `-=`   | subtraction        | `<name> -= <expr> ;`
Binary | `*=`   | multiplication     | `<name> *= <expr> ;`
Binary | `/=`   | division           | `<name> /= <expr> ;`
Binary | `%=`   | modulo (remainder) | `<name> %= <expr> ;`

## 3.2. Bitwise Operators {#bitwise-operators}

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Unary  | `~`  | bitwise negation    | `~ <expr>`
Binary | `|`  | bitwise OR          | `<expr> | <expr>`
Binary | `^`  | bitwise XOR         | `<expr> ^ <expr>`
Binary | `&`  | bitwise AND         | `<expr> & <expr>`
Binary | `<<` | bitwise left shift  | `<expr> << <expr>`
Binary | `>>` | bitwise right shift | `<expr> >> <expr>`

Assignment

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Binary | `|=`  | bitwise OR          | `<name> |= <expr>`
Binary | `^=`  | bitwise XOR         | `<name> ^= <expr>`
Binary | `&=`  | bitwise AND         | `<name> &= <expr>`
Binary | `<<=` | bitwise left shift  | `<name> <<= <expr>`
Binary | `>>=` | bitwise right shift | `<name> >>= <expr>`

## 3.3. Comparison Operators {#comparison-operators}

Type | Operator | Name
---- | -------- | ----
Binary | `==` | equals
Binary | `!=` | not equals
Binary | `< ` | less
Binary | `<=` | less or equal
Binary | `> ` | greater
Binary | `>=` | greater or equal

## 3.4. Logical Operators {#logical-operators}

Type | Operator | Name
---- | -------- | ----
Unary  | `!`  | logical NOT
Binary | `||` | logical OR
Binary | `&&` | logical AND

## 3.5. Other Operators {#other-operators}

Ternary

Type | Operator | Tag | Syntax
---- | :------: | --- | ------
Ternary | `?:` | \<ternary> | `( <expr> ) ? ( <expr> ) : ( <expr> )`

Address

Type | Operator | Tag | Syntax
---- | :------: | --- | ------
Unary | `&` | \<address> | `& <name>`

###### as

See [Conversion](#types-conversion) in [Types](#types)

Type | Operator | Tag | Syntax
---- | :------: | --- | ------
Binary | `as` | \<as> | `<expr> as <type>`

**Associativity**
**Precedence**

# 4. Expressions {#expressions}

## 4.1. Block {#block}

A block is a collection of statements.
- See `<stmt>` in [Statements](#statements) for the definition of a *statement*.

A block can be "broke from" using the `break` statement.

**Syntax**

Tag | Syntax
--- | ------
\<block> | `[<string>] { <stmt> {<stmt>} }`
\<break> | `break [<string>] [<expr>] ;`
&emsp; \<string> | See [Literals](#literals)
&emsp; \<stmt> | See [Statements](#statements)
&emsp; \<expr> | See [Expressions](#expressions)

**Context**

TODO

**Interpretation**

- All blocks can be labeled with a preceding string literal.
- Break statements can be optionally provided the block label to break from (as a string literal) and an expression to return from a block
	```rust
    max usize = 10;
    x isize = "x" {
        for( a usize = max;; a > 0; a -= 1 ) {
            for( b usize = max;; b > 0; b -= 1 ) {
                if( a + b == a * b ) {
                    break "x" (a + b);
                }
            }
        }
        break -1;
    }
	```

**Examples**

```rust
for( x usize = 0;; x < 10; x += 1 ) "x" {
	for( y usize = 0;; y < 10; y += 1 ) {
		for( z usize = 0;; z < 10; z += 1) {
			if z < 5 { break "x"; }
		}
	}
}
```

## 4.2. Control Flow {#control-flow}

### 4.2.1. If / Elif/ Else {#if-elif-else}

**Syntax**

Tag | Syntax
--- | ------
\<if>   | `if ( <expr> ) <block> {<elif>} [<else>]`
\<elif> | `elif ( <expr> ) <block>`
\<else> | `else <block>` 
&emsp; \<expr>  | See [Expressions](#expressions)
&emsp; \<block> | See [Block](#6-block)

**Parentship**

Tag | Parent | Comment
--- | ------ | -------
\<if> | \<expr> | See **Interpretation** & [Expressions](#expressions)

**Context**

TODO

**Interpretation**

**Examples**

```rust
a i32 = 1;
b i32 = if( a == 1 ) {
    break 10;
} elif( a == 2 ) {
    break 20;
} elif( a == 3 ) {
    break 30;
} else {
    break 0;
}
```

### 4.2.2. Loop {#loop}

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<loop> | `loop ( <stmt> ; <expr> ; <expr> ; <stmt> )` where both `<expr>` result in `bool` | start statement `;` start break condition `;` end break condition `;` end statement
&emsp; \<stmt> | See [Statements](#statements)
&emsp; \<expr> | See [Expressions](#expressions)
&emsp; `bool`  | See `<primitive>` in [Types](#types)

**Parentship**

Tag | Parent | Comment
--- | ------ | -------
\<loop> | \<expr> | See **Interpretation** & [Expressions](#expressions)

**Context**

TODO

**Interpretation**

TODO

**Examples**

```rust
loop( u32 i = 0;; i++ ) {}
loop( u32 i = 0; i < 10;; i++ ) {}
loop( u32 i = 0;; i < 10; i++ ) {}
loop( u32 i = 0; i < 10; i < 10; i++ ) {}
```

#### 4.2.2.1. Continue {#continue}

**Syntax**

Tag | Syntax
--- | ------
\<continue> | `continue ;`

**Context**

TODO

**Interpretation**

TODO

**Examples**

TODO

## 4.3. Compile-time expressions {#compile-time-expressions}

**Syntax**

Tag | Syntax
--- | ------
\<comp_expr> | `comp ( <expr> )`
\<comp_expr> | `comp <block>`

**Context**

- Metadata `@comp` can be used to mark [struct](#struct) fields and [variables](#variables) as being compile-time known.

**Interpretation**

TODO

**Examples**

```rust
add ${ a f32; b f32 } => f32 = {
    break a + b;
};
sub ${ @comp a f32; b f32 } => f32 = {
    break a - b;
};
mul ${ a f32; @comp b f32 } => f32 = {
    break a * b;
};
div ${ @comp a f32; @comp b f32 } => f32 = {
    break a / b;
};
main ${} => ${} = {
    @comp a f32 = 3;
    @comp b f32 = 5;

    @comp c f32 = add{a; b}; // ERROR
    c f32 = add{a; b}; // ok

    @comp c f32 = sub{a; b}; // ERROR
    c f32 = sub{a; b}; // ok

    @comp c f32 = mul{a; b}; // ERROR
    c f32 = mul{a; b}; // ok

    @comp c f32 = div{a; b}; // ok
    c f32 = div{a; b}; // ok

    break {};
};
```

# 5. Statements {#statements}

[var](#variables)
[function](#functions)
[struct](#struct)
[enum](#enum)
[namespace](#namespace)
[alias](#alias)

[if / elif / else](#control-flow)
[loop](#control-flow)
[block](#block)
[break](#block)
[breakif](#block)

# 6. Types {#types}

**Primitives**

Tag | Syntax
--- | ------
\<primitive> | below

- Integers
  - `i8`, `i16`, `i32`, `i64`, `i128`
  - `u8`, `u16`, `u32`, `u64`, `u128`
- Floating-point: `f32`, `f64`
- Boolean: `bool`
- Character: `char`
- Size: `isize`, `usize`

**Pointers**

**Functions**


  - Functions can take parameters, that get passed by value.
  - Functions can take variadic arguments.
  - Function parameters can take default values

**Immutability** (*const*-ness)

##### Conversion

See [as](#as) in [Operators](#operators)

`3 as f32`

# 7. Macros {#macros}

# 8. Metadata {#metadata}

**Syntax**

Tag | Syntax
--- | ------
\<meta> | `@ <name>`
&emsp; \<name> | See [Names](#names)

**Parentship**

TODO

**Interpretation**

TODO

**Examples**

TODO