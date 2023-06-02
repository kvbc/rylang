Note
[x] [zig](https://ziglang.org/documentation/master/)
[x] [odin](https://odin-lang.org/docs/overview/)
[ ] [rust](https://www.rust-lang.org/learn)
[ ] [go](https://go.dev/ref/spec)
[ ] [nim](https://nim-lang.org/docs/manual.html)

Good syntax
- `Variables`
- `Functions`
- `Struct`
- `Enum`
- `Namespace`

Good info
- TBD

Notes
- raw string \`single back ticks\` - `Literals`
- variadic arguments - `<func_type>`
- default argument values - `<func_type>`
- pointer arrays - `Types`
- using / use
- struct literal - `Literals`
- union
- block labels - `Block`
- switch / match
- function parameters immutable - `<func_type>`
- anon struct types - `<struct_type>` (see notes/notes4.rs)
- compile-time expressions - `Expressions`
- named arguments - `() operator`
  ```rust
  $Vector2 = {
    x i32;
    y i32;
  };
  :Vector2 = {
    new (x i32 = 0, y i32)$Vector2 = {
      v $Vector2;
      v.x = x;
      v.y = y;
      break v;
    }
  };
  v $Vector2 = :Vector2:new(y=0);
  ```

Table of Contents
- [Variables](#variables)
- [Functions][#functions]
- [Struct](#struct)
- [Enum](#enum)
- [Namespace](#namespace)
- [Block](#block)
- [Control Flow](#control-flow)

# Variables

A variable is a named container of data.

**Syntax**

Tag | Syntax | Comment
--- | ------ | -------
\<var>         | `<name> <type> = <expr> ;` where `<type>` is not `<struct_type>`                 | Non-struct variable
\<var>         | `<name> <struct_type> = <struct_literal> ;`                                      | Struct literal
\<var>         | `<name> <struct_type> = <expr> ;` where `<expr>` results in type `<struct_type>` | Struct copy
&emsp; \<name>           | See [Names](#names)
&emsp; \<expr>           | See [Expressions](#expressions)
&emsp; \<struct_literal> | See [Literals](#literals)
&emsp; \<type>, <br> &emsp; \<struct_type> | See [Types](#types)

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

# Functions

A function is an ordered sequence of statements that get executed whenever you call it.
  - See [Statements](#statements) for the definition of a statement `<stmt>`.
  - See [Namespace](#namespace) for the definition of a namespace statement `<namespace_stmt>`.

**Syntax**

Tag | Syntax
--- | ------
\<func>       | `<name> <func_type> = <func_block> ;`
\<func_block> | `{ <func_stmt> {<func_stmt>} }`
\<func_stmt>  | `<stmt> \| <namespace_stmt>`
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

# Struct

A *struct* (Structure) is a collection of variables (fields).

**Syntax**

Tag | Syntax
--- | ------
\<struct>        | `$ <name> = <struct_block> ;`
\<struct_block>  | `{ <struct_stmt> {<struct_stmt>} }`
\<struct_stmt>   | `<struct_field> \| <namespace_stmt>`
\<struct_field>  | `<name> <type> [= <compexpr>] ;` where `<type>` isn't the defined struct itself
\<struct_field>  | `<name> <struct_type> [= <struct_literal>] ;` where `<struct_type>` isn't the defined struct itself
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
# Enum

An Enum (Enumeration) is a collection of scoped, named & unique integer values (fields)

**Syntax**

Tag | Syntax
--- | ------
\<enum>        | `# <name> = <enum_block> ;`
\<enum_block>  | `{ <enum_stmt> {<enum_stmt>} }`
\<enum_stmt>   | `<enum_field> \| <namespace_stmt>`
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
  
# Namespace

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
\<namespace_stmt>   | `<func> \| <struct> \| <enum> \| <namespace>`
\<namespace_entity> | `<struct_type> \| <enum_type> \| <name> \| (<name>())` | Struct / Enum / Namespace / Function
\<namespace_access> | `: <namespace_entity> : <namespace_entity> {: <namespace_entity>}`
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

# Block

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
		x isize = for( a usize = 0; a < max; a += 1 ) "a" {
			for( b usize = 0; b < max; b += 1 ) "b" {
				if a + b == a * b {
					break "a" (a + b);
				}
			}
		}
	```

**Examples&**

```rust
for( x usize = 0;; x < 10; x += 1) "x" {
	for( y usize = 0;; y < 10; y += 1) "y" {
		for( z usize = 0;; z < 10; z += 1) "z" {
			if z < 5 { break "x"; }
		}
	}
}
```

# Control Flow

**If / Else**

Tag | Syntax
--- | ------
\<if>   | `if <expr> <block> {<elif>} [<else>]`
\<elif> | `elif <expr> <block>`
\<else> | `else <block>` 

- Examples:
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

**Loop**

Type | Tag | Syntax
---- | --- | ------
\<expr> | \<loop> | `loop ( <var> ; <bool_expr> ; <bool_expr> ; <stmt> )`

- Examples:
  ```rust
  loop( u32 i = 0;; i++ ) {}
  loop( u32 i = 0; i < 10;; i++ ) {}
  loop( u32 i = 0;; i < 10; i++ ) {}
  loop( u32 i = 0; i < 10; i < 10; i++ ) {}
  ```

**Loop -- continue**

Tag | Syntax
--- | ------
\<continue> | `continue ;`

# Modules

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

# Generics

Tag | Syntax | Examples
--- | ------ | --------
\<generics> | `< <name> {,<name>} >` | `<T>`, `<T1, T2>`, `<A, B, C>`

- Generics can only be applied within struct and alias definitions.
  - See [Struct](#struct)
  - See [Alias](#alias)
- Examples
  - Struct
    ```rust
    $Pair<T> = {
      a T;
      b T;
    };
    {
      pair $Pair<i32>;
      pair.a = -10;
      pair.b = 5;
    } {
      pair $Pair<bool>;
      pair.a = false;
      pair.b = true;
    }
    ```
  - Alias
    ```rust
    ^func<Ta, Tb, Tret> = (a Ta, b Tb)Tret;
    ```
    ```rust
    $Pair<T> = {
      a T;
      b T;
    };
    $^IntPair = $Pair<i32>;
    p $^IntPair;
    p.a = 1;
    p.b = 2;
    ```
    
# Comments

```rust
// single-line comment
```
```rust
/*
multi
line
comment
*/
```

# Statements

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

# Expressions

[if / elif / else](#control-flow)
[loop](#control-flow)
[block](#block)

# Operators

**Arithmetic**

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Unary  | `-`   | negation           | `- <expr>`
Binary | `+`   | addition           | `<expr> + <expr>`
Binary | `-`   | subtraction        | `<expr> - <expr>`
Binary | `*`   | multiplication     | `<expr> * <expr>`
Binary | `/`   | division           | `<expr> / <expr>`
Binary | `%`   | modulo (remainder) | `<expr> % <expr>`

Assign

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Binary | `+=`   | addition           | `<name> += <expr> ;`
Binary | `-=`   | subtraction        | `<name> -= <expr> ;`
Binary | `*=`   | multiplication     | `<name> *= <expr> ;`
Binary | `/=`   | division           | `<name> /= <expr> ;`
Binary | `%=`   | modulo (remainder) | `<name> %= <expr> ;`

**Bitwise**

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Unary  | `~`  | bitwise negation    | `~ <expr>`
Binary | `|`  | bitwise OR          | `<expr> | <expr>`
Binary | `^`  | bitwise XOR         | `<expr> ^ <expr>`
Binary | `&`  | bitwise AND         | `<expr> & <expr>`
Binary | `<<` | bitwise left shift  | `<expr> << <expr>`
Binary | `>>` | bitwise right shift | `<expr> >> <expr>`

Assign

Type | Operator | Name | Syntax
---- | -------- | ---- | ------
Binary | `|=`  | bitwise OR          | `<name> |= <expr>`
Binary | `^=`  | bitwise XOR         | `<name> ^= <expr>`
Binary | `&=`  | bitwise AND         | `<name> &= <expr>`
Binary | `<<=` | bitwise left shift  | `<name> <<= <expr>`
Binary | `>>=` | bitwise right shift | `<name> >>= <expr>`

**Comparison**

Type | Operator | Name
---- | -------- | ----
Binary | `==` | equals
Binary | `!=` | not equals
Binary | `< ` | less
Binary | `<=` | less or equal
Binary | `> ` | greater
Binary | `>=` | greater or equal

**Logical**

Type | Operator | Name
---- | -------- | ----
Unary  | `!`  | logical NOT
Binary | `||` | logical OR
Binary | `&&` | logical AND

**Other**

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

# Types

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
