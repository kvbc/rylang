# Language Specification

Note to self: press `CTRL K` + `Z` to leave zen mode

---

**Other Languages**

-   [ ] [Go](https://go.dev/ref/spec)
-   [x] [Zig](https://ziglang.org/documentation/master/)
-   [x] [Haxe](https://haxe.org/manual/introduction.html)
-   [ ] [Nim](https://nim-lang.org/docs/manual.html)
-   [x] [Odin](https://odin-lang.org/docs/overview/) - no spec
-   [x] [Rust](https://doc.rust-lang.org/reference/index.html)
-   [ ] [D](https://dlang.org/spec/spec.html)
-   [ ] [R](https://cran.r-project.org/doc/manuals/r-release/R-lang.pdf)

---

**Useful**

-   rustc dev guide - https://rustc-dev-guide.rust-lang.org/part-2-intro.html
-   Advanced R - https://adv-r.hadley.nz/

---

**Consider**

-   pointer arrays - `Types`
-   function parameters immutable - `<func_type>`
-   anon struct types - `<struct_type>` (see notes/notes4.rs)
-   type constraints - see [notes/playground/main.rs](../notes/playground/main.rs)
-   static
-   extern
-   struct-based, see [notes/all_struct.rs](../notes/all_struct.rs)
-   closures
-   variadic funtion arguments (struct fields)

---

**Table of Contents**

| Symbol | Meaning                                                                 |
| :----: | :---------------------------------------------------------------------- |
|   ✔️   | Good for the time being `/` Matching with the spec as is                |
|   ❌   | Untouched `/` Unsatisfied with `/` Not matching with the spec           |
|   ➖   | Neither satisfied or unsatisfied with `/` To be improved `/` Unfinished |
|   ↪️   | Dependent on other chapter(s)                                           |
|   👇   | Dependent on next subchapters                                           |
|   👆   | Dependent on previous subchapters                                       |

_(lets just ignore test coverage for now)_

| Chapter                                                                           | Syntax | Implemented | Error Handling | Test Coverage | Description                                                   |
| --------------------------------------------------------------------------------- | :----: | :---------: | :------------: | :-----------: | ------------------------------------------------------------- |
| 0. [Notation](#notation)                                                          |  N/A   |     N/A     |      N/A       |      N/A      |
| <br >1. [Lexical Analysis](#lexical-analysis) <br> <br>                           |   👇   |     👇      |       👇       |      👇       | **Grouping characters into tokens**                           |
| &emsp; 1.1 [Source Code](#source-code)                                            |   ✔️   |     N/A     |      N/A       |      N/A      | Encoding, characters, whitespace, newlines, EOF               |
| &emsp; 1.2. [Names](#names)                                                       |   ✔️   |     ✔️      |       ✔️       |      ❌       | Identifiers                                                   |
| &emsp; 1.3. [Comments](#comments)                                                 |   ✔️   |     ✔️      |       ✔️       |      ❌       | Single-line, multi-line                                       |
| &emsp; 1.4. [Literals](#literals)                                                 |   ✔️   |     👇      |       👇       |      👇       | Integer, floating-point, string, character, struct            |
| &emsp; &emsp; 1.4.1. [Integer Literals](#integer-literals)                        |   ✔️   |     ✔️      |       ✔️       |      ❌       | Binary, octal, decimal                                        |
| &emsp; &emsp; 1.4.2. [Float Literals](#float-literals)                            |   ✔️   |     ✔️      |       ✔️       |      ❌       | Exponents                                                     |
| &emsp; &emsp; 1.4.3. [String Literals](#string-literals)                          |   ✔️   |     ✔️      |       ✔️       |      ❌       | Raw, escapable, single-line, multi-line                       |
| &emsp; &emsp; 1.4.4. [Character Literals](#char-literals)                         |   ✔️   |     ✔️      |       ✔️       |      ❌       | Escapable                                                     |
| &emsp; 1.5. [Keywords](#keywords)                                                 |   ↪️   |     ✔️      |      N/A       |      ❌       | Special, reserved names (identifiers)                         |
| &emsp; 1.6. [Operators](#lexical-operators)                                       |   ↪️   |     ✔️      |      N/A       |      ❌       | Arithmetic, bitwise, comparison, logical, other               |
| &emsp; 1.7. [Tokens](#tokens)                                                     |   👆   |     👆      |       👆       |      👆       | Names, keywords, operators, literals, characters              |
| <br> 2. [Parsing and Semantic Analysis](#parsing-and-semantic-analysis) <br> <br> |   👇   |     👇      |       👇       |      👇       | **Grouping tokens into untyped AST nodes and their analysis** |
| &emsp; 2.1. [Types](#types)                                                       |   ✔️   |     ❌      |       👇       |      👇       |                                                               |
| &emsp; &emsp; 2.1.1. [Primitive Types](#primitive-types)                          |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.1.2. [Function Type](#function-type)                              |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.1.3. [Struct Type](#struct-type)                                  |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.1.4. [Type Attributes](#type-attribs)                             |   👇   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.1.4.1. [Pointer Types](#pointer-types)                     |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.1.4.2. [Type Mutability](#type-mutability)                 |   ➖   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.1.4.3. [Optional Types](#optional-types)                   |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; 2.2. [Operators](#operators)                                               |   👇   |     👇      |       👇       |      👇       |                                                               |
| &emsp; &emsp; 2.2.1. [Arithmetic Operators](#arithmetic-operators)                |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.2.2. [Bitwise Operators](#bitwise-operators)                      |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.2.3. [Comparison Operators](#comparison-operators)                |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.2.4. [Logical Operators](#logical-operators)                      |   ✔️   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.2.5. [Other Operators](#other-operators)                          |   ➖   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.2.6. [Operator Precedence](#operator-precedence)                  |   ➖   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; 2.3. [Expressions](#expressions)                                           |   ❌   |     👇      |       👇       |      👇       |                                                               |
| &emsp; &emsp; 2.3.1. [Block](#block) (& `break`)                                  |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.3.2. [Control Flow](#control-flow)                                |   👇   |     👇      |       👇       |      👇       |                                                               |
| &emsp; &emsp; &emsp; 2.3.2.1. [If / Elif / Else](#if-elif-else)                   |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.3.2.2. [Loop](#loop)                                       |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; &emsp; &emsp; 2.3.2.2.1. [Continue](#continue)                      |   ❌   |     ❌      |       ❌       |      ❌       | // This indent is crazy                                       |
| &emsp; &emsp; 2.3.3. [Compile-time Expressions](#compile-time-expressions)        |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.3.4. [Struct Literals](#parsing-struct-literals)                  |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; 2.4. [Statements](#statements)                                             |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; &emsp; 2.4.1. [Variables](#variables)                                      |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| &emsp; 2.5. [Metadata](#metadata)                                                 |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |
| <br> 3. [Typing](#typing) <br> <br>                                               |   👇   |     👇      |       👇       |      👇       | **"Typing" the untyped AST nodes**                            |
| <br> 4. [Transpilation](#transpilation) <br> <br>                                 |   👇   |     👇      |       👇       |      👇       | **Transpiling typed AST nodes into C source code**            |
| <br> 5. [Compile-time Evaluation](#compile-time-evaluation) <br> <br>             |   👇   |     👇      |       👇       |      👇       | **JIT compile-time expression evaluation**                    |
| &emsp; 5.1. [Macros](#macros)                                                     |   ❌   |     ❌      |       ❌       |      ❌       |                                                               |

---

# 0. Notation {#notation}

The syntax is specified using a variant of Extended Backus-Naur Form (EBNF)

The notation has been extended by an negation or exclusion operator `~`.
Symbols have been wrapped in arrow brackets `<>` and are now called tags.

Example:

```ebnf
<number>       = 0-9;
<alphanumeric> = a-z | A-Z | <number>
<not_number>   = ~ <number>               (* anything but number    *)
<alpha>        = <alphanumeric> ~ <alpha> (* this, excluding number *)
```

# 1. Lexical Analysis {#lexical-analysis}

Lexical analysis is the process of grouping source code characters into tokens.

## 1.1. Source Code {#source-code}

| Tag           | Syntax                                                                                             | Comment                                                                     |
| ------------- | -------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
| \<new_line>   | `\n \| \r \| \r\n` or `LF \| CR \| CRLF`                                                           |                                                                             |
| \<src_char>   | Any valid [ASCII](https://www.asciitable.com/) character in range of <1, 127> and not `<new_line>` | The NUL `\0` character is not considered <br> a valid source code character |
| \<whitespace> | `' ' \| \t \| \v \| \f \| <new_line>`                                                              |                                                                             |
| \<src_code>   | `<src_char> {<src_char>}`                                                                          | One or more source code characters                                          |
| \<eof>        | NUL `\0`, end of the `<src_code>` stream.                                                          | End Of File                                                                 |

## 1.2. Names {#names}

**Syntax**

| Tag                       | Syntax                                                              |
| ------------------------- | ------------------------------------------------------------------- |
| \<name>                   | `<name_start_char> {<name_char>}` where `<name>` is not `<keyword>` |
| &emsp; \<name_start_char> | <code>\_ \| a-z \| A-Z</code>                                       |
| &emsp; \<name_char>       | <code>\<name_start_char> \| 0-9</code>                              |
| &emsp; \<keyword>         | See [Keywords](#keywords)                                           |

**Interpretation**

-   A name cannot start with a digit.
-   Names (identifiers) can only consist of:
    -   underscores `_`,
    -   lowercase and uppercase letters from `aA` to `zZ`,
    -   digits from `0` to `9`

**Examples**

```rust
_var
a_b_c
x__
word
_0
__Big_Word__

1_3      // invalid
2_plus_2 // invalid
```

## 1.3. Comments {#comments}

**Syntax**

| Tag(s)                                      | Syntax                                                                | Comment                                                                                       |
| ------------------------------------------- | --------------------------------------------------------------------- | --------------------------------------------------------------------------------------------- |
| \<comment>                                  | `// {<src_char>} [<new_line>]` where `<src_char>` is not `<new_line>` | Single-line comment                                                                           |
| \<comment>, <br> \<multi_line_comment>      | `/* {<src_char>} */` where `<src_char>` is not `*/`                   | Multi-line comment. <br> Multiple multi-line comments can NOT be nested inside of each other. |
| &emsp; \<src_char>, <br> &emsp; \<new_line> | See [Source Code](#source-code)                                       |

**Errors**

| Tag                   | Error                              | Syntax                  | Comment                    |
| --------------------- | ---------------------------------- | ----------------------- | -------------------------- |
| \<multi_line_comment> | ❗ Unterminated multi-line comment | `/* {<src_char>} <eof>` | Terminating `*/` not found |

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

## 1.4. Literals {#literals}

| Tag                  | Syntax                                                                   | Comment                                                                                                           |
| -------------------- | ------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------- |
| \<literal>           | `<int_lit> \| <float_lit> \| <string_lit> \| <char_lit> \| <struct_lit>` |                                                                                                                   |
| &emsp; \<int_lit>    | See [Integer Literals](#integer-literals)                                |                                                                                                                   |
| &emsp; \<float_lit>  | See [Float Literals](#float-literals)                                    |                                                                                                                   |
| &emsp; \<string_lit> | See [String Literals](#string-literals)                                  |                                                                                                                   |
| &emsp; \<char_lit>   | See [Character Literals](#char-literals)                                 |                                                                                                                   |
| &emsp; \<struct_lit> | See [Struct Literals](#struct-literals)                                  | Struct literals are not part of the lexical analysis process, <br> therefore are not mentioned under this chapter |

### 1.4.1. Integer Literals {#integer-literals}

**Syntax**

| Tag(s)                          | Syntax                                 | Comment                     |
| ------------------------------- | -------------------------------------- | --------------------------- |
| \<bin_int_lit>, <br> \<int_lit> | `0b 0\|1 {{_} 0\|1 {_}}`               | Binary integer literal      |
| \<oct_int_lit>, <br> \<int_lit> | `0o 0-7 {{_} 0-7 {_}}`                 | Octal integer literal       |
| \<dec_int_lit>, <br> \<int_lit> | `0-9 {{_} 0-9 {_}}`                    | Decimal integer literal     |
| \<hex_int_lit>, <br> \<int_lit> | `0x <hex_digit> {{_} <hex_digit> {_}}` | Hexadecimal integer literal |
| &emsp; \<hex_digit>             | `(0 - 9) \| (a - f) \| (A - F)`        | Hexadecimal digit           |

**Errors**

| Tag        | Error                        | Syntax                                                                                                                                                                                                           | Comment |
| ---------- | ---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- |
| \<int_lit> | ❗ Malformed integer literal | `0(b\|o)(<src_char> ~ 0-9)` <br> `0x(<src_char> ~ <hex_digit>)` <br> <br> `(<bin_int_lit> \| <oct_int_lit>) <alpha>` <br> `(<hex_int_lit> (<alpha> ~ <hex_digit>))` <br> <li> where `<alpha> = a-z \| A-Z` </li> |         |
| \<int_lit> | ❗ Invalid digit             | `(0b \| <bin_int_lit>) 0-9` or <br> `(0o \| <oct_int_lit>) 0-9` or <br> `(0x \| <hex_int_lit>) 0-9`                                                                                                              |

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

### 1.4.2. Float Literals {#float-literals}

**Syntax**

| Tag                 | Syntax                                    | Comment  |
| ------------------- | ----------------------------------------- | -------- |
| \<float_lit>        | `<int_lit> . <int_lit> [<float_exp>]`     |
| \<float_lit>        | `<int_lit> <float_exp>`                   |
| &emsp; \<float_exp> | `e\|E [+\|-] <int_lit>`                   | Exponent |
| &emsp; \<int_lit>   | See [Integer Literals](#integer-literals) |

**Errors**

| Tag          | Error                       | Syntax                                     |
| ------------ | --------------------------- | ------------------------------------------ |
| \<float_lit> | ❗ Unfinished float literal | `<dec_int_lit> . ~<dec_int_lit>`           |
| \<float_lit> | ❗ Unfinished exponent      | `<dec_int_lit> e\|E [+\|-] ~<dec_int_lit>` |

**Examples**

TODO

### 1.4.3. String Literals {#string-literals}

**Syntax**

| Tag(s)                                                                | Syntax                                                                                | Comment                              |
| --------------------------------------------------------------------- | ------------------------------------------------------------------------------------- | ------------------------------------ |
| \<string_lit>, <br> \<single_line_string_lit>                         | `` ` {<src_char>} ` ``                                                                | Single-line raw string literal       |
| \<string_lit>, <br> \<multi_line_string_lit>                          | <code>\`\`\` {<src_char> \| <new_line>} \`\`\`</code>                                 | Multi-line raw string literal        |
| \<string_lit>, <br> \<esc_string_lit>, <br> \<single_line_string_lit> | `" {<esc_string_lit_char>} "`                                                         | Single-line escapable string literal |
| \<string_lit>, <br> \<esc_string_lit>, <br> \<multi_line_string_lit>  | `""" {<esc_string_lit_char> \| <new_line>} """`                                       | Multi-line escapable string literal  |
| &emsp; \<esc_string_lit_char>                                         | `(<src_char> ~ \) \| \<string_lit_esc_seq>`                                           |
| \<string_lit_esc_seq>                                                 | <code>a \| b \| e \| f \| n \| r \| t \| v \| ' \| \` \| " \| \ \| <new_line> </code> |
| \<string_lit_esc_seq>                                                 | `<int_lit>` which maps to `<src_char>`                                                |
| &emsp; \<src_char>                                                    | See [Lexical Analysis](#lexical-analysis)                                             |
| &emsp; \<int_lit>                                                     | See [Integer Literals](#integer-literals)                                             |

**Errors**

| Tag                       | Error                                                | Syntax                                                                                                                                                                      |
| ------------------------- | ---------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| \<single_line_string_lit> | ❗ Unexpected new line in single-line string literal | <code>\` {<src_char>} <new_line> \`<code> <br> `" {<esc_string_lit_char>} <new_line> "`                                                                                     |
| \<multi_line_string_lit>  | ❗ Expected termination of multi-line string literal | <code>\`\`\` {<src_char> \| <new_line>} \`\|\`\`\|\<eof></code> <br> `""" {<esc_string_lit_char> \| <new_line>} "\|""\|<eof>`                                               |
| \<single_line_string_lit> | ❗ Unterminated single-line string literal           | <code>\` {<src_char>} \<eof></code> <br> `" {<esc_string_lit_char>} <eof>`                                                                                                  |
| \<esc_string_lit>         | ❗ Invalid escape sequence                           | `<esc_string_lit>` containing `\<string_lit_esc_seq>` <br> where `<string_lit_esc_seq> = ~ <string_lit_esc_seq>`                                                            |
| \<esc_string_lit>         | ❗ Escape sequence out of bounds                     | `<esc_string_lit>` containing `\<string_lit_esc_seq>` <br> where `<string_lit_esc_seq>` is `<int_lit>` <br> and `<string_lit_esc_seq>` does not map to a valid `<src_char>` |

**Examples**

```rust
"one line string"
"multi \n line \n string"
"
    multi
    multi
    line
    string
"
`
    multi
    raw
    string
`
"
    one \
    line \
    multi \
" == "one line multi"
```

### 1.4.4. Character Literals {#char-literals}

**Syntax**

| Tag                          | Syntax                                      |
| ---------------------------- | ------------------------------------------- |
| \<char_lit>                  | `' {<char_lit_char>} '`                     |
| &emsp; \<char_lit_char>      | `(<src_char> ~ \) \| \<string_lit_esc_seq>` |
| &emsp; \<src_char>           | See [Lexical Analysis](#lexical-analysis)   |
| &emsp; \<string_lit_esc_seq> | See [String Literals](#string-literals)     |

**Errors**

| Tag         | Error                             | Syntax                                  | Comment                   |
| ----------- | --------------------------------- | --------------------------------------- | ------------------------- |
| \<char_lit> | ❗ Unterminated character literal | `' {<char_lit_char>} <eof>`             | Terminating `'` not found |
| \<char_lit> | ❗ Invalid escape sequence        | See [String Literals](#string-literals) |
| \<char_lit> | ❗ Escape sequence out of bounds  | See [String Literals](#string-literals) |

**Examples**

TODO

## 1.5. Keywords {#keywords}

| Tag                 | Syntax                        |
| ------------------- | ----------------------------- |
| \<keyword>          | `<primitive>` or any of below |
| &emsp; \<primitive> | See [Primitives](#primitives) |

```
if elif else
loop continue break
false true
not or and
as
```

## 1.6. Operators {#lexical-operators}

Refer to [Operators](#operators)

## 1.7. Tokens {#tokens}

Tokens represent:

-   [Names](#names)
-   [Keywords](#keywords)
-   [Operators](#operators)
-   [Literals](#literals)
-   Characters: `( ) ;`

# 2. Parsing and Semantic Analysis {#parsing-and-semantic-analysis}

## 2.1. Types {#types}

| Tag                              | Syntax                                                                                                                                                                                                     | Comment                                                 |
| -------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------- |
| \<type>                          | `[<type_attribs>] (<primitive_type> \| <func_type> \| <struct_type> \| <comp_expr>)` where `<comp_expr>` evaluates into `<type>`                                                                           |
| &emsp; \<type_attribs>           | `<type_attribs_1> = (<opt_type_attrib> \| <ptr_type_attrib>) [<type_attribs>]` <br> `<type_attribs_2> = <mut_type_attrib> [<type_attribs_1>]` <br> `<type_attribs> = <type_attribs_1> \| <type_attribs_2>` | There can NOT be two adjacent `mutable` type attributes |
| &emsp; &emsp; \<ptr_type_attrib> | See [Pointer Types](#pointer-types)                                                                                                                                                                        |
| &emsp; &emsp; \<mut_type_attrib> | See [Type Mutability](#type-mutability)                                                                                                                                                                    |
| &emsp; &emsp; \<opt_type_attrib> | See [Optional Types](#optional-types)                                                                                                                                                                      |
| &emsp; \<primitive_type>         | See [Primitive Types](#primitive-types)                                                                                                                                                                    |
| &emsp; \<func_type>              | See [Function Type](#function-type)                                                                                                                                                                        |
| &emsp; \<struct_type>            | See [Struct Type](#struct-type)                                                                                                                                                                            |
| &emsp; \<comp_expr>              | See [Compile-time expressions](#compile-time-expressions)                                                                                                                                                  |

### 2.1.1. Primitive Types {#primitive-types}

| Tag               | Type             | Syntax                            |
| ----------------- | ---------------- | --------------------------------- |
| \<primitive_type> | Character        | `char`                            |
| \<primitive_type> | Signed integer   | `i8 \| i16 \| i32 \| i64 \| i128` |
| \<primitive_type> | Unsigned integer | `u8 \| u16 \| u32 \| u64 \| u128` |
| \<primitive_type> | Floating-point   | `f32 \| f64`                      |
| \<primitive_type> | Boolean          | `bool`                            |

### 2.1.2. Function Type {#function-type}

| Tag                   | Syntax                          |
| --------------------- | ------------------------------- |
| \<func_type>          | `<struct_type> => <type>`       |
| &emsp; \<struct_type> | See [Struct Type](#struct-type) |
| &emsp; \<type>        | See [Types](#types)             |

**Examples**

```rust
add[a i32; b i32] => i32 = a + b;
```

### 2.1.3. Struct Type {#struct-type}

| Tag                    | Syntax                                                                                                                                                                                                                  |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| \<struct_type>         | `'[' [{<struct_field> ,\|;} <struct_field>] ']'`                                                                                                                                                                        |
| &emsp; \<struct_field> | `[<name> {,<name>}] <type> [= <comp_expr>]` where `<comp_expr>` coerces into `<type>` <br> `<type> [* <comp_expr>] [= <comp_expr>]` where the first `<comp_expr>` is of type `u32` and the second coerces into `<type>` |
| &emsp; \<name>         | See [Names](#names)                                                                                                                                                                                                     |
| &emsp; \<type>         | See [Types](#types)                                                                                                                                                                                                     |
| &emsp; \<comp_expr>    | See [Compile-time expressions](#compile-time-expressions)                                                                                                                                                               |

**Examples**

```rust
pos [x i32, y i32 = 9] = [3, 5];

list [i32 * 3] = [1, 2, 3];

tuple [i32, bool, f32] = [1, true, 2.3];
```

### 2.1.4. Type Attributes {#type-attribs}

| Tag                       | Syntax                                                        |
| ------------------------- | ------------------------------------------------------------- |
| \<type_attrib>            | `<ptr_type_attrib> \| <mut_type_attrib> \| <opt_type_attrib>` |
| &emsp; \<ptr_type_attrib> | See [Pointer Types](#pointer-types)                           |
| &emsp; \<mut_type_attrib> | See [Type Mutability](#type-mutability)                       |
| &emsp; \<opt_type_attrib> | See [Optional Types](#optional-types)                         |

#### 2.1.4.1. Pointer Types {#pointer-types}

| Tag                | Syntax |
| ------------------ | ------ |
| \<ptr_type_attrib> | `*`    |

#### 2.1.4.2. Type Mutability {#type-mutability}

| Tag                | Syntax | Comment                    |
| ------------------ | ------ | -------------------------- |
| \<mut_type_attrib> | `$`    | This character kinda sucks |

#### 2.1.4.3. Optional Types {#optional-types}

| Tag                | Syntax |
| ------------------ | ------ |
| \<opt_type_attrib> | `?`    |

## 2.2. Operators {#operators}

| Tag       | Syntax    | Comment                                |
| --------- | --------- | -------------------------------------- |
| <expr_op> | See below | Operation resulting in an expression   |
| <stmt_op> | See below | Standalone operation yielding no value |

### 2.2.1. Arithmetic Operators {#arithmetic-operators}

| Tag       | Type   | Operator | Name               | Syntax             | Associativity | Precedence |
| --------- | ------ | :------: | ------------------ | ------------------ | :-----------: | :--------: |
| <expr_op> | Unary  |   `-`    | negation           | `- <expr>`         |     `<--`     |     4      |
| <expr_op> | Binary |   `+`    | addition           | `<expr> + <expr>`  |     `-->`     |     7      |
| <expr_op> | Binary |   `-`    | subtraction        | `<expr> - <expr>`  |     `-->`     |     7      |
| <expr_op> | Binary |   `*`    | multiplication     | `<expr> * <expr>`  |     `-->`     |     6      |
| <expr_op> | Binary |   `**`   | power of           | `<expr> ** <expr>` |     `-->`     |     5      |
| <expr_op> | Binary |   `/`    | division           | `<expr> / <expr>`  |     `-->`     |     6      |
| <expr_op> | Binary |   `%`    | modulo (remainder) | `<expr> % <expr>`  |     `-->`     |     6      |

Assignment

| Tag       | Type   | Operator | Name               | Syntax              | Associativity | Precedence |
| --------- | ------ | :------: | ------------------ | ------------------- | :-----------: | :--------: |
| <stmt_op> | Binary |   `+=`   | addition           | `<name> += <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |   `-=`   | subtraction        | `<name> -= <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |   `*=`   | multiplication     | `<name> *= <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |  `**=`   | power of           | `<name> **= <expr>` |      N/A      |    N/A     |
| <stmt_op> | Binary |   `/=`   | division           | `<name> /= <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |   `%=`   | modulo (remainder) | `<name> %= <expr>`  |      N/A      |    N/A     |

### 2.2.2. Bitwise Operators {#bitwise-operators}

| Tag       | Type   | Operator | Name                | Syntax             | Associativity | Precedence |
| --------- | ------ | :------: | ------------------- | ------------------ | :-----------: | :--------: |
| <expr_op> | Unary  |   `~`    | bitwise negation    | `~ <expr>`         |     `<--`     |     4      |
| <expr_op> | Binary |   `\|`   | bitwise OR          | `<expr> \| <expr>` |     `-->`     |     13     |
| <expr_op> | Binary |   `^`    | bitwise XOR         | `<expr> ^ <expr>`  |     `-->`     |     12     |
| <expr_op> | Binary |   `&`    | bitwise AND         | `<expr> & <expr>`  |     `-->`     |     11     |
| <expr_op> | Binary |   `<<`   | bitwise left shift  | `<expr> << <expr>` |     `-->`     |     8      |
| <expr_op> | Binary |   `>>`   | bitwise right shift | `<expr> >> <expr>` |     `-->`     |     8      |

Assignment

| Tag       | Type   | Operator | Name                | Syntax              | Associativity | Precedence |
| --------- | ------ | :------: | ------------------- | ------------------- | :-----------: | :--------: |
| <stmt_op> | Binary |  `\|=`   | bitwise OR          | `<name> \|= <expr>` |      N/A      |    N/A     |
| <stmt_op> | Binary |   `^=`   | bitwise XOR         | `<name> ^= <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |   `&=`   | bitwise AND         | `<name> &= <expr>`  |      N/A      |    N/A     |
| <stmt_op> | Binary |  `>>=`   | bitwise right shift | `<name> >>= <expr>` |      N/A      |    N/A     |
| <stmt_op> | Binary |  `<<=`   | bitwise left shift  | `<name> <<= <expr>` |      N/A      |    N/A     |

### 2.2.3. Comparison Operators {#comparison-operators}

| Tag       | Type   | Operator | Name             | Syntax             | Associativity | Precedence |
| --------- | ------ | :------: | ---------------- | ------------------ | :-----------: | :--------: |
| <expr_op> | Binary |   `==`   | equals           | `<expr> == <expr>` |     `-->`     |     10     |
| <expr_op> | Binary |   `!=`   | not equals       | `<expr> != <expr>` |     `-->`     |     10     |
| <expr_op> | Binary |   `< `   | less             | `<expr> < <expr>`  |     `-->`     |     9      |
| <expr_op> | Binary |   `<=`   | less or equal    | `<expr> <= <expr>` |     `-->`     |     9      |
| <expr_op> | Binary |   `> `   | greater          | `<expr> > <expr>`  |     `-->`     |     9      |
| <expr_op> | Binary |   `>=`   | greater or equal | `<expr> >= <expr>` |     `-->`     |     9      |

### 2.2.4. Logical Operators {#logical-operators}

| Tag       | Type   | Operator | Name        | Syntax              | Associativity | Precedence |
| --------- | ------ | :------: | ----------- | ------------------- | :-----------: | :--------: |
| <expr_op> | Unary  |  `not`   | logical NOT | `not <expr>`        |     `<--`     |     4      |
| <expr_op> | Binary |   `or`   | logical OR  | `<expr> or <expr>`  |     `-->`     |     15     |
| <expr_op> | Binary |  `and`   | logical AND | `<expr> and <expr>` |     `-->`     |     14     |

### 2.2.5. Other Operators {#other-operators}

| Tag       | Type   | Operator | Name         | Syntax                                                          | Associativity | Precedence | Description                                            |
| --------- | ------ | :------: | ------------ | --------------------------------------------------------------- | :-----------: | :--------: | ------------------------------------------------------ |
| <expr_op> | Unary  |   `&`    | address of   | `& <name>`                                                      |     `<--`     |     3      |                                                        |
| <expr_op> | Binary |   `as`   | as           | `<expr> as <type>`                                              |     `-->`     |     2      | See [Conversion](#types-conversion) in [Types](#types) |
| <expr_op> | Binary |   `.`    | Block access | `<name> . (<name> \| <expr>)` where `<expr>` coerces into `u32` |     `-->`     |     1      |                                                        |
| <stmt_op> | Binary |   `=`    | Assignment   | `<name> = <expr>`                                               |      N/A      |    N/A     |                                                        |
| <stmt_op> | Binary |   `:=`   | Definition   | `<name> := <expr>`                                              |      N/A      |    N/A     |                                                        |

### 2.2.6. Operator Precedence {#operator-precedence}

TODO: Verify

| Level | Operators         |
| :---: | :---------------- |
|   1   | `.`               |
|   2   | `as`              |
|   3   | `&`               |
|   4   | `-a` `~a` `not`   |
|   5   | `**`              |
|   6   | `*` `/` `%`       |
|   7   | `a+b` `a-b`       |
|   8   | `<<` `>>`         |
|   9   | `<` `<=` `>` `>=` |
|  10   | `==` `!=`         |
|  11   | `a&b`             |
|  12   | `^`               |
|  13   | `\|`              |
|  14   | `and`             |
|  15   | `or`              |

## 2.3. Expressions {#expressions}

**Syntax**

| Tag               | Syntax                                                             |
| ----------------- | ------------------------------------------------------------------ |
| \<expr>           | `['(']  <literal> \| <block> \| <if> \| <loop> \| <expr_op> [')']` |
| &emsp; \<literal> | See [Literals](#literals)                                          |
| &emsp; \<block>   | See [Block](#block)                                                |
| &emsp; \<if>      | See [If / Elif / Else](#if-elif-else)                              |
| &emsp; \<loop>    | See [Loop](#loop)                                                  |
| &emsp; \<expr_op> | See [Operators](#operators)                                        |

### 2.3.1. Block {#block}

A block is a collection of statements.

-   See `<stmt>` in [Statements](#statements) for the definition of a _statement_.

A block can be "broken from" using the `break` statement.

**Syntax**

| Tag              | Syntax                           |
| ---------------- | -------------------------------- |
| \<block>         | `[<string>] { <stmt> {<stmt>} }` |
| \<break>         | `break [<string>] [<expr>] ;`    |
| &emsp; \<string> | See [Literals](#literals)        |
| &emsp; \<stmt>   | See [Statements](#statements)    |
| &emsp; \<expr>   | See [Expressions](#expressions)  |

**Context**

TODO

**Interpretation**

-   All blocks can be labeled with a preceding string literal.
-   Break statements can be optionally provided the block label to break from (as a string literal) and an expression to return from a block
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

### 2.3.2. Control Flow {#control-flow}

#### 2.3.2.1. If / Elif / Else {#if-elif-else}

**Syntax**

| Tag             | Syntax                                    |
| --------------- | ----------------------------------------- |
| \<if>           | `if ( <expr> ) <block> {<elif>} [<else>]` |
| \<elif>         | `elif ( <expr> ) <block>`                 |
| \<else>         | `else <block>`                            |
| &emsp; \<expr>  | See [Expressions](#expressions)           |
| &emsp; \<block> | See [Block](#6-block)                     |

**Parentship**

| Tag   | Parent  | Comment                                              |
| ----- | ------- | ---------------------------------------------------- |
| \<if> | \<expr> | See **Interpretation** & [Expressions](#expressions) |

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

#### 2.3.2.2. Loop {#loop}

**Syntax**

| Tag            | Syntax                                                                            | Comment                                                                             |
| -------------- | --------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------- |
| \<loop>        | `loop ( <stmt> ; <expr> ; <expr> ; <stmt> )` where both `<expr>` result in `bool` | start statement `;` start break condition `;` end break condition `;` end statement |
| &emsp; \<stmt> | See [Statements](#statements)                                                     |
| &emsp; \<expr> | See [Expressions](#expressions)                                                   |
| &emsp; `bool`  | See `<primitive>` in [Types](#types)                                              |

**Parentship**

| Tag     | Parent  | Comment                                              |
| ------- | ------- | ---------------------------------------------------- |
| \<loop> | \<expr> | See **Interpretation** & [Expressions](#expressions) |

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

##### 2.3.2.2.1. Continue {#continue}

**Syntax**

| Tag         | Syntax       |
| ----------- | ------------ |
| \<continue> | `continue ;` |

**Context**

TODO

**Interpretation**

TODO

**Examples**

TODO

### 2.3.3. Compile-time expressions {#compile-time-expressions}

**Syntax**

| Tag          | Syntax            |
| ------------ | ----------------- |
| \<comp_expr> | `comp ( <expr> )` |
| \<comp_expr> | `comp <block>`    |

**Context**

-   Metadata `@comp` can be used to mark [struct](#struct) fields and [variables](#variables) as being compile-time known.

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

### 2.3.4. Struct Literals {#struct-literals}

**Syntax**

| Tag                     | Syntax                                  | Comment                                                       |
| ----------------------- | --------------------------------------- | ------------------------------------------------------------- |
| \<struct_literal>       | `[ {<struct_literal_field>} ]`          |
| \<struct_literal_field> | `[<name> =] <expr> [;]` see **Comment** | The semicolon `;` can only be omitted if it's the last field. |
| &emsp; \<name>          | See [Names](#names)                     |
| &emsp; \<expr>          | See [Expressions](#expressions)         |

**Interpretation**

TODO

**Examples**

```rust
pos [x i32; y i32] = [3; 5];
```

## 2.4. Statements {#statements}

**Syntax**

| Tag     | Syntax                                |
| ------- | ------------------------------------- |
| \<stmt> | `<var> \| <expr> \| <namespace_stmt>` |

### 2.4.1. Variables {#variables}

**Syntax**

| Tag            | Syntax                                                          | Comment             |
| -------------- | --------------------------------------------------------------- | ------------------- |
| \<var>         | `<name> <type> = <expr> ;` where `<expr>` coerces into `<type>` | Non-struct variable |
| &emsp; \<name> | See [Names](#names)                                             |
| &emsp; \<expr> | See [Expressions](#expressions)                                 |
| &emsp; \<type> | See [Types](#types)                                             |

-   See `<name>` in [Names](#names) for the rules behind a valid variable name (identifier).
-   See `<expr>` in [Expressions](#expressions) for what a variable can be assigned.
-   The variable's value must be able to coerce into the variable's type.
    -   See [Type Coercion](#type-coercion)
-   All variables must be initialized.
-   Variables of type `<type>` being `<func_type>` are knows as functions.
    -   See `<func_type>` in [Types](#types)
-   Variables of type `<type>` being `<struct_type>` are known as structs.
    -   See `<struct_type>` in [Types](#types)

**Context**

| Tag             | Parent                |
| --------------- | --------------------- |
| \<var>          | `<block>`             |
| &emsp; \<block> | See [Blocks](#blocks) |

-   Variables can only exist inside of blocks.

**Examples**

```rust
x i32 = 3; // int var

add [a i32; b i32]=>i32 = a + b;

Vec2 var = [ // comp-time variable, type alias
    x i32;
    y i32;
]
```

<!--
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
-->

## 2.5. Metadata {#metadata}

**Syntax**

| Tag            | Syntax                                                           |
| -------------- | ---------------------------------------------------------------- |
| \<meta>        | `@ <name>` where `<name>` is one of the listed in **Parentship** |
| &emsp; \<name> | See [Names](#names)                                              |

**Parentship**

| Metadata    | Parent             | Comment                |
| ----------- | ------------------ | ---------------------- |
| @pub        | `<namespace_stmt>` | Public access modifier |
| @constraint | `<func>`           | Contstraint function   |
| @union      | `<struct_type>`    | Union                  |
| @comp       |                    | Compile-time statement |
| @closure    | `<func>`           | Closure                |

**Interpretation**

TODO

**Examples**

TODO

# 3. Typing {#typing}

# 4. Transpilation {#transpilation}

# 5. Compile-time Evaluation {#compile-time-evaluation}

## 5.1. Macros {#macros}

...

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

## 2.3. Struct {#struct}

A _struct_ (Structure) is a collection of variables (fields).

**Syntax**

| Tag                      | Syntax                                                                          | Comment                                                                                                                                                           |
| ------------------------ | ------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| \<struct>                | `$ <name> = <struct_block> ;`                                                   |
| \<struct_block>          | `{ <struct_stmt> {<struct_stmt>} }`                                             |
| \<struct_stmt>           | `<struct_field> \| <namespace_stmt>`                                            |
| \<struct_field>          | `<name> <type> [= <compexpr>] ;` where `<type>` isn't the defined struct itself | <li>The semicolon `;` might be omitted if it's the last field in a struct.</li> <li>Struct fields follow the same rules as regular [variables](#1-variables)</li> |
| &emsp; \<name>           | See [Names](#names)                                                             |
| &emsp; \<var>            | See [Variables](#variables)                                                     |
| &emsp; \<var_type>       | See [Types](#types)                                                             |
| &emsp; \<namespace_stmt> | See [Namespace](#namespace)                                                     |
| &emsp; \<compexpr>       | See [Expressions](#expressions)                                                 |
| &emsp; \<struct_literal> | See [Literals](#literals)                                                       |

**Parentship**

| Tag       | Parent       | Comment                     |
| --------- | ------------ | --------------------------- |
| \<struct> | \<namespace> | See [Namespace](#namespace) |

**Context**

TODO

**Interpretation**

-   A _struct_ is also a namespace.
    -   See [Namespace](#namespace).
    ```rust
      $Vector2 = {
        ^int = i32;
        x ^int;
        y ^int;
      };
    ```
-   Structs can only be defined inside of namespaces.
    -   See [Namespace](#namespace).
-   A _struct_ cannot be empty.
    ```rust
    $Vector2 = {} // ERROR
    ```
-   See `<struct_type>` in [Types](#types) for anonymous structs.
-   A _struct_ field cannot be of the same type as the defined struct.
    ```rust
    $A = {
      a $A; // ERROR
    };
    ```
-   _Struct_ fields follow the same rules as regular variables.
    -   See [Variables](#variables).
-   _Struct_ fields can be assigned default values.
    ```rust
    $Vector2 = {
      x i32 = 0;
      y i32 = 0;
    };
    ```
-   _Struct_ fields can be accessed using the dot `.` operator.
    -   See [Operators](#operators) for the _struct_ field access operator
    ```rust
    pos $Vector2;
    pos.x = 0;
    pos.y = 0;
    x i32 = pos.x;
    ```
-   Functions declared inside of a _struct_, with their first parameter being of the defined _struct_ type, are called _methods_. \
    You can call them on a struct instance using the colon `:` operator.

    -   See [Operators](#operators) for the _struct_ _method_ access operator.

    The first "self" argument is excluded from a _method_ call. \
    This _struct_ _method_ syntax is nothing but syntactic sugar for plain-old _namespace_ access.

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

-   Examples:
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

| Tag                      | Syntax                                                             |
| ------------------------ | ------------------------------------------------------------------ |
| \<enum>                  | `# <name> = <enum_block> ;`                                        |
| \<enum_block>            | `{ <enum_stmt> {<enum_stmt>} }`                                    |
| \<enum_stmt>             | `<enum_field> \| <namespace_stmt>`                                 |
| \<enum_field>            | `<name> [= <compexpr>] ;` where `<compexpr>` results in type `i32` |
| \<enum_access>           | `<enum_type> : <name>`                                             |
| &emsp; \<name>           | See [Names](#names)                                                |
| &emsp; \<namespace_stmt> | See [Namespace](#namespace)                                        |
| &emsp; \<number>         | See [Literals](#literals)                                          |
| &emsp; \<compexpr>       | See [Expressions](#expressions)                                    |

**Parentship**

| Tag     | Parent       | Comment                     |
| ------- | ------------ | --------------------------- |
| \<enum> | \<namespace> | See [Namespace](#namespace) |

**Context**

TODO

-   The first enum field, if not explicitely set, is equal to 0.
-   Each next enum field, if not explicitely set, is 1 higher than the previous value.

**Interpretation**

-   Enum fields are of type `i32`
    -   See `<primitive>` in [Types](#types) for the type of `i32`.
-   Enums can only be defined inside of namespaces.
    -   See [Namespace](#namespace)
        Enums are also namespaces.
    -   See [Namespace](#namespace).
-   Enum fields can be accessed using the colon `:` operator
    -   See [Operators](#operators) for the _enum field access_ operator.
    ```rust
    var #Enum = #Enum:FIELD;
    ```
-   _Enum_ fields can be explicitely set.
    -   The set value must be a _compile-time_ expression resulting in a value of type `i32`.
        -   See [Expressions](#expressions) for the definition of a _compile-time_ expression `compexpr`.
        ```rust
        x i32 = 10;
        #Color = {
            RED = x, // ERROR
            GREEN = 0.5, // ERROR
        };
        ```
    -   The set value must be higher than the previous values.
        ```rust
        #Color = {
            RED, // 0
            GREEN, // 1
            BLUE = 1 // ERROR
        }
        ```
    -   The fields can be set even multiple times
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

-   [Functions](#functions),
-   [Structs](#struct),
-   [Enums](#enum),
-   [Aliases](#alias), and
-   Other namespaces

**Syntax**

| Tag                 | Syntax                                                           | Comment                              |
| ------------------- | ---------------------------------------------------------------- | ------------------------------------ |
| \<namespace>        | `: <name> = <namespace_block> ;`                                 |
| \<namespace>        | `: <name> = <string> ;`                                          | Module import                        |
| \<namespace_block>  | `{ <namespace_stmt> {<namespace_stmt>} }`                        |
| \<namespace_stmt>   | `<func> \| <struct>      \| <enum> \| <namespace>`               |
| \<namespace_entity> | `<struct_type> \| <enum_type>   \| <name> \| <name>()`           | Struct / Enum / Namespace / Function |
| \<namespace_access> | `<namespace_entity> : <namespace_entity> {: <namespace_entity>}` |
| &emsp; \<name>      | See [Names](#names)                                              |
| &emsp; \<string>    | See [Literals](#literals)                                        |
| &emsp; \<func>      | See [Functions](#functions)                                      |
| &emsp; \<struct>    | See [Struct](#struct)                                            |
| &emsp; \<enum>      | See [Enum](#enum)                                                |
| &emsp; \<alias>     | See [Alias](#alias)                                              |

**Parentship**

| Tag          | Parent       | Comment         |
| ------------ | ------------ | --------------- |
| \<namespace> | \<namespace> | See **Context** |

**Context**

-   The global scope is also considered a namespace.

**Interpretation**

-   Namespaces can be imported from other files. This concept forms the basis of [Modules](#modules).
-   [Functions](#functions), [Structs](#struct) and [Enums](#enum) are all namespaces.
-   To access a namespace member, you can use the colon `:` operator
    -   ```rust
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

| Tag                                                  | Syntax                      |
| ---------------------------------------------------- | --------------------------- | ---------------------- |
| \<use>                                               | `use (<namespace_entity>    | <namespace_access>) ;` |
| &emsp; \<namespace_entity>, <br> \<namespace_access> | See [Namespace](#namespace) |

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
