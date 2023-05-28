Note
- [zig](https://ziglang.org/documentation/master/)
- [rust](https://www.rust-lang.org/learn)
- [go](https://go.dev/learn/)
- [nim](https://nim-lang.org/docs/manual.html)
- [odin](https://odin-lang.org/docs/overview/)

Notes
- raw string \`single back ticks\`
- variadic arguments
- default argument values
- pointer arrays
- named arguments
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
- problem - scoped func struct?
  ```rust
  $Result = {
    val i32;
    ok bool;
  };
  xadd (a i32, b i32)$Result = {
    res $Result;
    res.val = a + b;
    res.ok = (a != b);
    break res;
  }
  res $Result = xadd(1, 2);
  ```
  ```rust
  xadd (a i32, b i32)xadd:$Result = {
    @public $Result = {
      val i32;
      ok bool;
    };
    res $Result;
    res.val = a + b;
    res.ok = (a != b);
    break res;
  }
  res xadd:$Result = xadd(1, 2);
  ```

# Variables

Tag | Syntax
--- | ------
\<var> | `<name> <type> = <expr> ;`
\<var> | `<name> <struct_type> ;`

- All variables must be initialized, except variables of the struct type. \ 
Those can be unitialized, but **all** of their fields **must** be initalized in the same block that the struct has been declared in. See [Struct](#struct)
- Variables can only exist inside of functions and their sub-blocks - there are no global variables.
- Examples:
  ```rust
  len u8 = 10;
  ptr *u8 = &len;
  s $struct;
  ```
  ```rust
  $A = { x j32; };
  $B = { a $A; };
  b $B;
  b.a.x = 3;
  ```

# Functions

Tag | Syntax
--- | ------
\<func> | `<name> <func_type> = <block> ;`

- Examples:
  ```rust
  add (a i32, b i32)i32 = {
    break a + b;
  }
  @main main ()void = {
    a i32 = 1;
    b i32 = 2;
    c i32 = add(a, b);
  }
  ```

# Struct

Tag | Syntax
--- | ------
\<struct>       | `$ <name> [<generics>] = <struct_block> ;`
\<struct_block> | `{ <struct_field> {<struct_field>} }`
\<struct_field> | `<name> <type> ;`

- A *struct* (Structure) is just a collection of variables (fields).
- A *struct* cannot be empty.
- The fields **cannot** be initialized on struct creation.
  ```rust
  $Vector2 = {
    x i32 = 0; // error
    y i32 = 0; // error
  }
  ```
- The fields can be accessed using the dot `.` operator.
  ```rust
  pos $Vector2;
  pos.x = 0;
  pos.y = 0;
  x i32 = pos.x;
  ```
- Examples:
  ```rust
  $Vector2 = {
      x i32;
      y i32;
  };
  :Vector2 = {
      new (x i32, y i32)$Vector2 {
          v $Vector2;
          v.x = x;
          v.y = y;
          break v; 
      }
  }
  @main main ()void = {
    pos $Vector2 = :Vector2:new(0, 0);
  }
  ```

# Enum

Tag | Syntax
--- | ------
\<enum>        | `# <name> = <enum_block> ;`
\<enum_block>  | `{ <enum_field> {<enum_field>} }`
\<enum_field>  | `<name> [= <number>] ,`
\<enum_access> | `# <name> : <name>` 

- *Enum* (Enumeration) is a collection of scoped, named, unique integer values (fields)
- *Enum* values are of type `i32`
- *Enum* values can be accessed using the colon `:` operator
  - ```rust
    var #Enum = #Enum:FIELD;
    ```
- *Enum* values can be explicitely set.
  - The set value must be an integer literal.
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
  - Even multiple times
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
- The first *enum* value, if not explicitely set, is equal to 0.
- Each next *enum* value, if not explicitely set, is 1 higher than the previous value. 
- Examples:
  ```rust
  #Color = {
    RED;   // 0
    GREEN; // 1
    BLUE;  // 2
  };
  c #Color = #Color:RED;
  ```

# Namespace

Tag | Syntax | Comment
--- | ------ | -------
\<namespace>        | `: <name> = <namespace_block> ;`
\<namespace>        | `: <name> = <string> ;` | Module import
\<namespace_block>  | `{ <namespace_field> {<namespace_field>} }`
\<namespace_field>  | `<func> | <struct> | <enum> | <namespace> | <alias>`
\<namespace_access> | `: <name> : <name>`
\<namespace_access> | `: ^ <name> : <name>` | Namespace [alias](#alias)

- A *namespace* is a scoped collection of:
  - Functions
  - Structs
  - Enums
  - Aliases
  - Other namespaces

- To access a *namespace* member, you can use the colon `:` operator
  - ```rust
    :math = {
        $Vector2 = { x i32; y i32; };
    };
    pos :math:$Vector2;
    ```
- See [Alias](#alias) for namespace aliasing.

# Alias

Tag | Syntax | Comment
--- | ------ | -------
\<alias> | `^ <name> [<generics>] = <type> ;` | Type alias
\<alias> | `:^ <name> = :<name> {:<name>} ;` | [Namespace](#namespace) alias
\<alias> | `$^ <name> [<generics>] = <struct_type> ;` | [Struct](#struct) alias
\<alias> | `#^ <name> = <enum_type> ;` | [Enum](#enum) alias

- An *alias* is an different name (alias) for a specified type.
- Examples:
  ```rust
  ^char = u8;
  ^int = i32;

  x ^int = 10;
  c ^char = 'c';
  ```
  Namespace alias
  ```rust
  :a = {
    :b = {
        :c = {
            $Vector2 = { x i32; y i32; };
        }
    }
  };

  pos1 :a:b:c:$Vector2;

  ^abc = :a:b:c;
  pos2 :^abc:$Vector2;
  ```

# Control Flow

**If / Else**

Tag | Syntax | Comment
--- | ------ | -------
\<if>   | `if ( <expr> ) <block> {<elif>} [<else>]`
\<elif> | `elif ( <expr> ) <block>`
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

Tag | Syntax | Comment
--- | ------ | -------
\<loop> | `loop <block>`

- Examples:
  ```rust
  loop {
    @static i u32 = 0;
    :std:print(i);
    breakif( i++ >= 10 );
  }
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

# Block

Tag | Syntax
--- | ------
\<block> | `{ <stmt> {<stmt>} }`

**break**

Tag | Syntax
--- | ------
\<break> | `break [<expr>] ;`

**breakif**

Tag | Syntax
--- | ------
\<breakif> | `breakif ( <expr> ) [<expr>] ;`

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

**Immutability** (*const*-ness)

##### Conversion

See [as](#as) in [Operators](#operators)

`3 as f32`