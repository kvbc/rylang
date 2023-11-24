<center><h1>Language Specification</h1></center>

---

<!--

Note to self: press CTRL K + Z to leave zen mode

Other languages:
- Go    https://go.dev/ref/spec
- Zig   https://ziglang.org/documentation/master
- Odin  https://odin-lang.org/docs/overview
- Rust  https://doc.rust-lang.org/reference/index.html
- Haxe  https://haxe.org/manual/introduction.html
- D     https://dlang.org/spec/spec.html
- GML   https://manual.yoyogames.com/GameMaker_Language/GameMaker_Language_Index.htm
- Lua   https://www.lua.org/manual/5.4/

Useful resources:
- rustc dev guide - https://rustc-dev-guide.rust-lang.org/part-2-intro.html

Consider:
- static
- extern
- type constraints - see [notes/playground/main.rs](../notes/playground/main.rs)
- closures
- variadic funtion arguments (struct fields) - might not need it with macros
- we dont need a slice type - see [notes/slice.rs](../notes/slice.rs)
- bit sets (odinlang)
- `any` type
- tests

TODO:
- Types
- type constraints
- enums
- unions

-->

<style>
    ol { counter-reset: item }
	ol li { display: block }
	ol li:before { content: counters(item, ".") " "; counter-increment: item }
</style>

<body>
<div style="width:300px; position:fixed; top:0; font-size:12px;">
	<ul>
        <li>
            <a href="#implementation">Implementation</a>
            <ol>
                <li><a href="#error-handling">Error Handling</a></li>
                <li>
                    <a href="#compilation">Compilation</a>
                    <ol>
                        <li>
                            <a href="#lexical-analysis">Lexical Analysis</a>
                            <ol>
                                <li><a href="#tokens">Tokens</a></li>
                            </ol>
                        </li>
                        <li><a href="#syntax-analysis">Syntax Analysis</a></li>
                        <li><a href="#semantic-analysis">Semantic Analysis</a></li>
                        <li><a href="#type-checking">Type Checking</a></li>
                        <li><a href="#compile-time-expression-evaluation">Compile-time Expression Evaluation (CTEE)</a></li>
                        <li><a href="#transpilation">Transpilation</a></li>
                    </ol>
                </li>
            </ol>
        </li>
        <li>
            <a href="#design">Design</a>
            <ol>
                <li>
                    <a href="#introduction">Introduction</a>
                    <ol>
                        <li><a href="#notation">Notation</a></li>
                    </ol>
                </li>
                <li>
                    <a href="#grammar">Grammar</a>
                    <ol>
                        <li><a href="#source-code">Source Code</a></li>
                        <li><a href="#names">Names</a></li>
                        <li><a href="#comments">Comments</a></li>
                        <li><a href="#escape-sequences">Escape Sequences</a></li>
                        <li>
                            <a href="#literals">Literals</a>
                            <ol>
                                <li><a href="#integer-literals">Integer Literals</a></li>
                                <li><a href="#float-literals">Float Literals</a></li>
                                <li><a href="#string-literals">String Literals</a></li>
                                <li><a href="#char-literals">Character Literals</a></li>
                                <li><a href="#bool-literals">Boolean Literals</a></li>
                                <li><a href="#null-literals">Null Literal</a></li>
                            </ol>
                        </li>
                        <li><a href="#keywords">Keywords</a></li>
                        <li><a href="#operators">Operators</a></li>
                    </ol>
                </li>
                <li>
                    <a href="#syntax-n-semantics">Syntax & Semantics</a>
                    <ol>
                        <li>
                            <a href="#types">Types</a>
                            <ol>
                                <li>
                                    <a href="#type-attribs">Type Attributes</a>
                                    <ol>
                                        <li><a href="#mutable">Mutable</a></li>
                                        <li><a href="#optional">Optional</a></li>
                                    </ol>
                                </li>
                                <li><a href="#primitive-types">Primitive Types</a></li>
                                <li><a href="#function-type">Function Type</a></li>
                                <li><a href="#struct-type">Struct Type</a></li>
                                <li><a href="#pointer-type">Pointer Type</a></li>
                            </ol>
                        </li>
                        <li>
                            <a href="#parsing-operators">Operators</a>
                            <ol>
                                <li><a href="#arithmetic-operators">Arithmetic Operators</a></li>
                                <li><a href="#bitwise-operators">Bitwise Operators</a></li>
                                <li><a href="#comparison-operators">Comparison Operators</a></li>
                                <li><a href="#logical-operators">Logical Operators</a></li>
                                <li><a href="#other-operators">Other Operators</a></li>
                                <li><a href="#operator-precedence">Operator Precedence</a></li>
                                <li><a href="#operator-associativity">Operator Associativity</a></li>
                            </ol>
                        </li>
                        <li>
                            <a href="#expressions">Expressions</a>
                            <ol>
                                <li>
                                    <a href="#l-values">L-Values</a>
                                    <ol>
                                        <li><a href="#pointer-dereferencing">Pointer Dereferencing</a></li>
                                        <li><a href="#struct-member-access">Struct Member Access</a></li>
                                    </ol>
                                    <li><a href="#function-call">Function Call</a></li>
                                    <li><a href="#address-of">Address Of</a></li>
                                    <li><a href="#type-cast">Type Cast</a></li>
                                    <li><a href="#block">Block</a></li>
                                    <li><a href="#if-else">If / Else</a></li>
                                    <li><a href="#loop">Loop</a></li>
                                    <li><a href="#struct-literals">Struct Literals</a></li>
                                    <li><a href="#compile-time-expressions">Compile-time Expressions</a></li>
                                </li>
                            </ol>
                        </li>
                        <li>
                            <a href="#statements">Statements</a>
                            <ol>
                                <li><a href="#variable-definition">Variable Definition</a></li>
                                <li><a href="#assignment">Assignment</a></li>
                                <li><a href="#continue-statement">Continue Statement</a></li>
                                <li><a href="#break-statement">Break Statement</a></li>
                            </ol>
                        </li>
                        <li><a href="#metadata">Metadata</a></li>
                    </ol>
                </li>
            </ol>
        </li>
	<ul>
</div>

<div style="margin-left:300px">

<!--

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

| Chapter                                                    | Syntax | Implemented | Elegant Error Handling | Test Coverage | Comment                                            |
| ---------------------------------------------------------- | :----: | :---------: | :--------------------: | :-----------: | -------------------------------------------------- |
| 0. [Notation](#notation)                                   |  N/A   |     N/A     |          N/A           |      N/A      |
| <br >1. [Lexical Analysis](#lexical-analysis) <br> <br>    |   👇   |     👇      |           👇           |      👇       | **Grouping characters into tokens**                |
| &emsp; 1.1 [Source Code](#source-code)                     |   ✔️   |     N/A     |          N/A           |      N/A      | Encoding, characters, whitespace, newlines, EOF    |
| &emsp; 1.2. [Names](#names)                                |   ✔️   |     ✔️      |           ➖           |      ❌       | Identifiers                                        |
| &emsp; 1.3. [Comments](#comments)                          |   ✔️   |     ✔️      |           ➖           |      ❌       | Single-line, multi-line                            |
| &emsp; 1.4. [Literals](#literals)                          |   ✔️   |     👇      |           👇           |      👇       | Integer, floating-point, string, character, struct |
| &emsp; &emsp; 1.4.1. [Integer Literals](#integer-literals) |   ✔️   |     ✔️      |           ➖           |      ❌       | Binary, octal, decimal                             |
| &emsp; &emsp; 1.4.2. [Float Literals](#float-literals)     |   ✔️   |     ✔️      |           ➖           |      ❌       | Exponents                                          |
| &emsp; &emsp; 1.4.3. [String Literals](#string-literals)   |   ✔️   |     ✔️      |           ➖           |      ❌       | Raw, escapable, single-line, multi-line            |
| &emsp; &emsp; 1.4.4. [Character Literals](#char-literals)  |   ✔️   |     ✔️      |           ➖           |      ❌       | Escapable                                          |
| &emsp; &emsp; 1.4.5. [Boolean Literals](#bool-literals)    |   ✔️   |     ✔️      |          N/A           |      ❌       | true, false                                        |
| &emsp; &emsp; 1.4.6. [Null Literal](#null-literal)         |   ✔️   |     ✔️      |          N/A           |      ❌       | null                                               |
| &emsp; 1.5. [Keywords](#keywords)                          |   ↪️   |     ✔️      |          N/A           |      ❌       | Special, reserved names (identifiers)              |
| &emsp; 1.6. [Operators](#lexical-operators)                |   ↪️   |     ✔️      |          N/A           |      ❌       | Arithmetic, bitwise, comparison, logical, other    |
| &emsp; 1.7. [Tokens](#tokens)                              |   👆   |     👆      |           👆           |      👆       | Names, keywords, operators, literals, characters   |

| Chapter                                                                           | Syntax | Semantics |           Parsing Implemented           | Analysis Implemented | Elegant Error Handling | Test Coverage | Comment                                                       |
| --------------------------------------------------------------------------------- | :----: | :-------: | :-------------------------------------: | :------------------: | :--------------------: | :-----------: | ------------------------------------------------------------- |
| <br> 2. [Parsing and Semantic Analysis](#parsing-and-semantic-analysis) <br> <br> |   👇   |    👇     |                   👇                    |          👇          |           👇           |      👇       | **Grouping tokens into untyped AST nodes and their analysis** |
| &emsp; 2.1. [Types](#types)                                                       |  ✔️👇  |    ❌     |                   👇                    |          ❌          |           👇           |      👇       |                                                               |
| &emsp; &emsp; 2.1.1. [Primitive Types](#primitive-types)                          |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ✔️           |      ❌       |                                                               |
| &emsp; &emsp; 2.1.2. [Function Type](#function-type)                              |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ✔️           |      ❌       |                                                               |
| &emsp; &emsp; 2.1.3. [Struct Type](#struct-type)                                  |   ➖   |    ❌     |                   ✔️                    |          ❌          |           ➖           |      ❌       |                                                               |
| &emsp; &emsp; 2.1.4. [Pointer Types](#pointer-types)                              |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ✔️           |      ❌       |                                                               |
| &emsp; &emsp; 2.1.5. [Type Attributes](#type-attribs)                             |   👇   |    ❌     |                   👇                    |          ❌          |           👇           |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.1.5.1. [Type Mutability](#type-mutability)                 |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ✔️           |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.1.5.2. [Optional Types](#optional-types)                   |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ✔️           |      ❌       |                                                               |
| &emsp; 2.2. [Operators](#operators)                                               |   👇   |    ❌     | ↪️ <br> Expression ✔️ <br> Statement ✔️ |          ❌          |           👇           |      👇       |                                                               |
| &emsp; &emsp; 2.2.1. [Arithmetic Operators](#arithmetic-operators)                |   ✔️   |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.2. [Bitwise Operators](#bitwise-operators)                      |   ✔️   |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.3. [Comparison Operators](#comparison-operators)                |   ✔️   |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.4. [Logical Operators](#logical-operators)                      |   ✔️   |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.5. [Other Operators](#other-operators)                          |   ↪️   |    ❌     |                   ↪️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.6. [Operator Precedence](#operator-precedence)                  |  ✔️👆  |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.2.7. [Operator Associativity](#operator-associativity)            |  ✔️👆  |    ❌     |                   👆                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; 2.3. [Expressions](#expressions)                                           |  ✔️👇  |    ❌     |                  ↪️👇                   |          ❌          |           👇           |      👇       |                                                               |
| &emsp; &emsp; 2.3.1. [L-Values](#l-values)                                        |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.3.1.1. [Pointer Dereference](#pointer-dereference)         |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.3.1.2. [Struct Member Access](#struct-member-access)       |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.2. [Function Call](#function-call)                              |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.3. [Address Of](#address-of)                                    |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.4. [Type Cast](#type-cast)                                      |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.5. [Block](#block)                                              |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.6. [Control Flow](#control-flow)                                |   👇   |    ❌     |                   👇                    |          ❌          |           👇           |      👇       |                                                               |
| &emsp; &emsp; &emsp; 2.3.6.1. [If / Else](#if-else)                               |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; &emsp; 2.3.6.2. [Loop](#loop)                                       |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.7. [Struct Literals](#struct-literals)                          |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.3.8. [Compile-time Expressions](#compile-time-expressions)        |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; 2.4. [Statements](#statements)                                             |  ✔️👇  |    ❌     |                  ↪️👇                   |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.4.1. [Variable Definition](#variable-definition)                  |   ➖   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       | // possibly add struct destructuring                          |
| &emsp; &emsp; 2.4.2. [Assignment](#assignment)                                    |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.4.3. [Continue Statement](#continue-statement)                    |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; &emsp; 2.4.4. [Break Statement](#break-statement)                          |   ✔️   |    ❌     |                   ✔️                    |          ❌          |           ❌           |      ❌       |                                                               |
| &emsp; 2.5. [Metadata](#metadata)                                                 |   ✔️   |    ❌     |                   ❌                    |          ❌          |           ❌           |      ❌       |                                                               |
| <br> 3. [Typing](#typing) <br> <br>                                               |   👇   |    ❌     |                   👇                    |          ❌          |           👇           |      👇       | **"Typing" the untyped AST nodes**                            |
| <br> 4. [Transpilation](#transpilation) <br> <br>                                 |   👇   |    ❌     |                   👇                    |          ❌          |           👇           |      👇       | **Transpiling typed AST nodes into C source code**            |
| <br> 5. [Compile-time Evaluation](#compile-time-evaluation) <br> <br>             |   👇   |    ❌     |                   👇                    |          ❌          |           👇           |      👇       | **JIT compile-time expression evaluation**                    |
| &emsp; 5.1. [Macros](#macros)                                                     |   ❌   |    ❌     |                   ❌                    |          ❌          |           ❌           |      ❌       |                                                               |

-->

# 1. Implementation {#implementation}

## 1.1. Error Handling {#error-handling}

Error messages consist of one or more "source markers".

A source marker describes the following properties:

-   Start line number,
-   Start column number,
-   End line number,
-   End column number

Source markers can NOT overlap.

**Examples**

One source marker spanning over multiple lines

```
 main.ry |
      10 | this := is(
         | ^~~~~~~~~~~
         | Error message
         |
      11 |     "a", "very",
         |     ~~~~~~~~~~~~
      12 |     "long", "function call"
         |     ~~~~~~~~~~~~~~~~~~~~~~~
      13 | )
         | ~
```

Multple source markers

```
 main.ry |
      10 | this := is(
         | ^~~~    ^~
         | Error message
         |
      11 |     "a", "very",
         |     ^~~  ^~~~~~
      12 |     "long", "function call"
         |     ^~~~~~  ^~~~~~~~~~~~~~~
      13 | )
```

## 1.2. Compilation {#compilation}

Compilation is the process of creating an executable from the input source code

### 1.2.1. Lexical Analysis {#lexical-analysis}

Lexical analysis is the process of grouping source code characters into tokens based on specified [grammar](#grammar) rules

#### 1.2.1.1. Tokens {#tokens}

Tokens represent:

-   [Names](#names)
-   [Keywords](#keywords)
-   [Operators](#operators)
-   [Literals](#literals)
-   Individual [source code](#source-code) characters

### 1.2.2. Syntax Analysis {#syntax-analysis}

Syntax analysis is the process of creating an Abstract Syntax Tree (AST) from the input tokens

### 1.2.3. Semantic Analysis {#semantic-analysis}

Semantic analysis is the process of analyzing the previously created Abstract Syntax Tree (AST) by specified semantics rules.

### 1.2.4. Type Checking {#type-checking}

Type checking is the process of analyzing a semantically correct Abstract Syntax Tree (AST), assigning types to AST nodes and validating type relations between them.

### 1.2.5. Compile-time Expression Evaluation (CTEE) #{compile-time-expression-evaluation}

TODO <!-- ... and figure out -->

### 1.2.6. Transpilation {#transpilation}

Transpilation is the process of transforming the Abstract Syntax Tree (AST) into the source code of a lower-level programming language (in this case, C).

That transpiled source code then gets compiled by a third-party compiler, producing the final executable and eliminating the need of writing our own backend.

---

# 2. Design {#design}

## 2.1. Introduction {#introduction}

### 2.1.1. Notation {#notation}

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

## 2.2. Grammar {#grammar}

### 2.2.1. Source Code {#source-code}

**Grammar**

```ebnf
<new_line>     = \n | \r | \r\n (* LF | CR | CRLF *)
<src_char>     = (* Any ASCII character in range of <1, 127> and not <new_line> *)
<whitespace>   = ' ' | \t | \v | \f | <new_line>
<eos>          = (* end of source code *)
<any_src_char> = <src_char> | <new_line> | <eos>
<eol>          = <new_line> | <eos> (* end of line *)
```

**Warnings**

-   Mixed new-line encodings

**Errors**

-   NUL character (0) in source code

### 2.2.2. Names {#names}

**Grammar**

```ebnf
<name>                = (<name_start_char> {<name_char>}) ~ <keyword>
    <name_start_char> = _ | a-z | A-Z
    <name_char>       = <name_start_char> | 0-9
```

**References**

-   [Keywords](#keywords)

### 2.2.3. Comments {#comments}

**Grammar**

```ebnf
<comment> = // {<src_char>} (<new_line> | <eos>)
          | /* ({<src_char>} ~ */) */
```

**Errors**

-   Unterminated multi-line comment

**References**

-   [Source Code](#source-code)

### 2.2.4. Escape Sequences {#escape-sequences}

**Grammar**

```ebnf
<esc_seq_char> = a | b | e | f | n | r | t | v | \
<esc_seq> = \ <esc_seq_char>
          | \ <int_lit> (* which maps to <any_src_char> *)

<string_esc_seq_char> = <esc_seq_char> | " | <new_line>
<string_esc_seq> = \ <string_esc_seq_char>
                 | <esc_seq>

<char_esc_seq_char> = <esc_seq_char> | '
<char_esc_seq> = \ <char_esc_seq_char>
               | <esc_seq>
```

**Errors**

```ebnf
Invalid escape sequence = \ ~<string_esc_seq_char>
                        | \ ~<char_esc_seq_char>

Escape sequence out of bounds = \ <int_lit> (* where <int_lit> != <any_src_char> *)
```

Examples

```ebnf
Invalid escape sequence:
    "\x"
      ^

Escape sequence out of bounds:
    "\200"
      ^~~
```

**References**

-   [Integer Literals](#integer-literals)
-   [Source Code](#source-code)

### 2.2.5. Literals {#literals}

**Grammar**

```ebnf
<literal> = <int_lit>
          | <float_lit>
          | <string_lit>
          | <char_lit>
          | <bool_lit>
          | <null_lit>
          | <struct_lit> (* struct literals are not recognized during the lexing phase *)
```

**References**

-   [Integer Literals](#integer-literals)
-   [Float Literals](#float-literals)
-   [String Literals](#string-literals)
-   [Character Literals](#char-literals)
-   [Struct Literals](#struct-literals)

#### 2.2.5.1. Integer Literals {#integer-literals}

**Grammar**

```ebnf
<int_lit> = <bin_int_lit>
          | <oct_int_lit>
          | <dec_int_lit>
          | <hex_int_lit>
<bin_int_lit> = 0b 0|1 {{_} 0|1 {_}}
<oct_int_lit> = 0o 0-7 {{_} 0-7 {_}}
<dec_int_lit> =    0-9 {{_} 0-9 {_}}
<hex_int_lit> = 0x <hex_digit> {{_} <hex_digit> {_}}
<hex_digit> = (0-9) | (a-f) | (A-F)
```

**Errors**

```ebnf
(* eats all invalid digits *)
Invalid digit(s) in integer literal = (0b | <bin_int_lit>) 2-9
                                    | (0o | <oct_int_lit>) 8-9
                                    | (0x | <hex_int_lit>) (g-z | G-Z)

Unfinished integer literal = 0b (<any_src_char> ~ 0|1)
                           = 0o (<any_src_char> ~ 0-7)
                           = 0x (<any_src_char> ~ <hex_digit>)

(* eats all the trailing characters *)
Malformed integer literal = (<bin_int_lit> | <oct_int_lit> | <dec_int_lit>) (a-z | A-Z)
```

Examples

```ebnf
Invalid digit(s) in integer literal:
    0b2
      ^

    0b151617891
       ^ ^ ^~~

Unfinished integer literal:
    0x;
      ^

Malformed integer literal:
    0o123a
         ^

    0xDEADlist
          ^~~~
```

**References**

-   [Source Code](#source-code)

#### 2.2.5.2. Float Literals {#float-literals}

**Grammar**

```ebnf
<float_lit> = <dec_int_lit> . <dec_int_lit> [<float_exp>]
            | <dec_int_lit> <float_exp>
<float_exp> = e|E [+|-] <dec_int_lit>
```

**Errors**

```ebnf
Unfinished float literal          = <dec_int_lit> . ~<dec_int_lit>
Unfinished float literal exponent = <dec_int_lit> e|E [+|-] ~<dec_int_lit>
```

Examples

```ebnf
Unfinished float literal:
    35.abc
       ^

Unfinished float literal exponent:
    123e;
       ^
```

**References**

-   [Integer Literals](#integer-literals)

#### 2.2.5.3. String Literals {#string-literals}

**Grammar**

````ebnf
<string_lit> = ` {<raw_sl_string_lit_char>} `
             | ``` {<raw_ml_string_lit_char>} ```
             | " {<esc_sl_string_lit_char>} "
             | """ {<esc_ml_string_lit_char>} """

<raw_sl_string_lit_char> = <src_char> ~ `
<esc_sl_string_lit_char> = (<src_char> ~ \ ~ ") | <string_esc_seq>
<raw_ml_string_lit_char> = <raw_sl_string_lit_char>
                         | <new_line>
<esc_ml_string_lit_char> = <esc_sl_string_lit_char>
                         | <new_lne>
````

**Errors**

````ebnf
Unterminated string literal = ` {<raw_sl_string_lit_char>} <eol>
                            | " {<esc_sl_string_lit_char>} <eol>
                            | ``` {<raw_ml_string_lit_char>} <eos>
                            | """ {<esc_ml_string_lit_char>} <eos>

(*
    Assumes that the string was meant to be terminated.
    Closes the literal and eats the terminating characters
*)
Expected termination of
multi-line string literal = ``` {<raw_ml_string_lit_char>} (` | ``)
                          | """ {<esc_ml_string_lit_char>} (" | "")

... = (* see Escape Sequences *)
````

Examples

```ebnf
Unterminated string literal:
    "abc
    ^   ^

Expected termination of
multi-line string literal:
    """
    ^~~
        multi
        line
        string
    ""
    ^~
```

**References**

-   [Source Code](#source-code)
-   [Escape Sequences](#escape-sequences)

#### 2.2.5.4. Character Literals {#char-literals}

**Grammar**

```ebnf
<char_lit> = ' <char_lit_char> '
<char_lit_char> = (<src_char> ~ \ ~ ') | <char_esc_seq>
```

**Errors**

```ebnf
Empty character literal = ' '

Unterminated character literal = ' [<char_lit_char>] (<any_src_char> ~ ')

... = (* see Escape Seqeunces *)
```

Examples

```ebnf
Empty character literal:
    ''
    ^~

Unterminated character literal:
    'c
    ^ ^

    'cx
    ^ ^

    '
    ^
```

**References**

-   [Source Code](#source-code)
-   [Escape Sequences](#escape-sequences)

#### 2.2.5.5. Boolean Literals {#bool-literals}

**Grammar**

```ebnf
<bool_lit> = true | false
```

**References**

-   [Keywords](#keywords)

#### 2.2.5.6. Null Literal {#null-literal}

**Grammar**

```ebnf
<null_lit> = null
```

**References**

-   [Keywords](#keywords)

### 2.2.6. Keywords {#keywords}

**Grammar**

```ebnf
<keyword> = <primitive_type>
          | do
          | if    | else
          | loop  | continue | break
          | false | true     | null
          | not   | or       | and
          | as    | comp     | ast
```

**References**

-   [Primitive Types](#primitive-types)

### 2.2.7. Operators {#lexical-operators}

Refer to [Operators](#parsing-operators)

## 2.3. Syntax & Semantics {#syntax-n-semantic-analysis}

### 2.3.1. Types {#types}

**Syntax**

```ebnf
<base_type> = <primitive_type> | <func_type> | <struct_type> | <ptr_type>
            | '(' <base_type> ')'

<type> = <type_attribs> <base_type>
       | '(' <type> ')'
```

**Syntax Errors**

```ebnf
Unclosed type

... (* see Type Attributes *)
```

Examples

```
Unclosed type:
    (i32
    ^   ^
    
    ?((bool)
      ^    ^
```

**References**

- [Type Attributes](#type-attribs)
- [Primitive Types](#primitive-types)
- [Function Type](#function-type)
- [Struct Type](#struct-type)
- [Pointer Type](#pointer-type)

#### 2.3.1.1. Type Attributes {#type-attribs}

**Syntax**

```ebnf
<_mut> = <mut_type_attrib>
<_opt> = <opt_type_attrib>
<type_attribs> = [<_mut>] [<_opt>]
```

**Syntax Errors**

```ebnf
Duplicate type attribute(s) = <mut> <mut> {<mut>}
                            | <opt> <opt> {<opt>}

Invalid type attribute order = <opt> <mut>
```

Examples

```
Duplicate type attribute(s):
    ~~x := 3;
    ^~
    
    ~~~~x := 5;
    ^~~~~
    
Invalid type attribute order:
    ?~a := b;
     ^
```

##### 2.3.1.1.1. Mutable {#mutable}

**Syntax**

```ebnf
<mut_type_attrib> = ~
```

##### 2.3.1.1.2. Optional {#optional-types}

**Syntax**

```ebnf
<opt_type_attrib> = ?
```

#### 2.3.1.2. Primitive Types {#primitive-types}

**Syntax**

```ebnf
<primitive_type> = char
                 | i8  | i16 | i32 | i64 | i128
                 | u8  | u16 | u32 | u64 | u128
                 | f32 | f64
                 | bool
```

#### 2.3.1.3. Function Type {#function-type}

**Syntax**

```ebnf
<func_type> = <struct_type> => <type>
```

**Syntax Errors**

```ebnf
Function arguments expected
to be of type struct, got ... = (<type> ~ <struct_type>) =>
```

Examples

```
Function arguments expected
to be of type struct, got ...:

    add i32 => i32 = 3;
        ^~~
```

**References**

- [Struct Type](#struct-type)
- [Type](#type)

#### 2.3.1.4. Struct Type {#struct-type}

**Syntax**

```ebnf
<struct_type> = '[' [{<_field> ,|;} <_field> [,|;]] ']'

<_field> = <name> <type> [= <comp_expr>]
         | <type> [* <comp_expr>] [= <comp_expr>]
         | [<comp_expr> *] <type> [= <comp_expr>]
```

**Syntax Errors**

```ebnf
Unclosed struct literal

Missing field separator
```

Examples

```
Unclosed struct literal:

    [ i32
         ^
        
Missing field separator:

    [ i8, i16 i32 ]
             ^
```

**Semantic Errors**

```
Field type repetitions must evaluate to a value higher than 0. Got ...

    [i32 * -1]
           ^~
           
    #comp rep := 2 - 3;
    [i32 * rep]
           ^~~

Duplicate field

    [i32 x, f32 x]
                ^
```

**References**

- [Names](#names)
- [Types](#types)
- [Compile-time expressions](#compile-time-expressions)

#### 2.3.1.5. Pointer Type {#pointer-type}

**Syntax**

```ebnf
<ptr_type> = * <type>
```

---

## 2.2. Operators {#operators}

| Tag       | Syntax    | Comment                                |
| --------- | --------- | -------------------------------------- |
| <expr_op> | See below | Operation resulting in an expression   |
| <stmt_op> | See below | Standalone operation yielding no value |

### 2.2.1. Arithmetic Operators {#arithmetic-operators}

| Tag       | Type   | Operator | Name               | Syntax            |
| --------- | ------ | :------: | ------------------ | ----------------- |
| <expr_op> | Unary  |   `-`    | negation           | `- <expr>`        |
| <expr_op> | Binary |   `+`    | addition           | `<expr> + <expr>` |
| <expr_op> | Binary |   `-`    | subtraction        | `<expr> - <expr>` |
| <expr_op> | Binary |   `*`    | multiplication     | `<expr> * <expr>` |
| <expr_op> | Binary |   `/`    | division           | `<expr> / <expr>` |
| <expr_op> | Binary |   `%`    | modulo (remainder) | `<expr> % <expr>` |

Assignment

| Tag       | Type   | Operator | Name               | Syntax               |
| --------- | ------ | :------: | ------------------ | -------------------- |
| <stmt_op> | Binary |   `+=`   | addition           | `<lvalue> += <expr>` |
| <stmt_op> | Binary |   `-=`   | subtraction        | `<lvalue> -= <expr>` |
| <stmt_op> | Binary |   `*=`   | multiplication     | `<lvalue> *= <expr>` |
| <stmt_op> | Binary |   `/=`   | division           | `<lvalue> /= <expr>` |
| <stmt_op> | Binary |   `%=`   | modulo (remainder) | `<lvalue> %= <expr>` |

### 2.2.2. Bitwise Operators {#bitwise-operators}

| Tag       | Type   | Operator | Name                | Syntax             |
| --------- | ------ | :------: | ------------------- | ------------------ |
| <expr_op> | Unary  |   `~`    | bitwise negation    | `~ <expr>`         |
| <expr_op> | Binary |   `\|`   | bitwise OR          | `<expr> \| <expr>` |
| <expr_op> | Binary |   `^`    | bitwise XOR         | `<expr> ^ <expr>`  |
| <expr_op> | Binary |   `&`    | bitwise AND         | `<expr> & <expr>`  |
| <expr_op> | Binary |   `<<`   | bitwise left shift  | `<expr> << <expr>` |
| <expr_op> | Binary |   `>>`   | bitwise right shift | `<expr> >> <expr>` |

Assignment

| Tag       | Type   | Operator | Name                | Syntax                |
| --------- | ------ | :------: | ------------------- | --------------------- |
| <stmt_op> | Binary |  `\|=`   | bitwise OR          | `<lvalue> \|= <expr>` |
| <stmt_op> | Binary |   `^=`   | bitwise XOR         | `<lvalue> ^= <expr>`  |
| <stmt_op> | Binary |   `&=`   | bitwise AND         | `<lvalue> &= <expr>`  |
| <stmt_op> | Binary |  `>>=`   | bitwise right shift | `<lvalue> >>= <expr>` |
| <stmt_op> | Binary |  `<<=`   | bitwise left shift  | `<lvalue> <<= <expr>` |

### 2.2.3. Comparison Operators {#comparison-operators}

| Tag       | Type   | Operator | Name             | Syntax             |
| --------- | ------ | :------: | ---------------- | ------------------ |
| <expr_op> | Binary |   `==`   | equals           | `<expr> == <expr>` |
| <expr_op> | Binary |   `!=`   | not equals       | `<expr> != <expr>` |
| <expr_op> | Binary |   `< `   | less             | `<expr> < <expr>`  |
| <expr_op> | Binary |   `<=`   | less or equal    | `<expr> <= <expr>` |
| <expr_op> | Binary |   `> `   | greater          | `<expr> > <expr>`  |
| <expr_op> | Binary |   `>=`   | greater or equal | `<expr> >= <expr>` |

### 2.2.4. Logical Operators {#logical-operators}

| Tag       | Type   | Operator | Name        | Syntax              |
| --------- | ------ | :------: | ----------- | ------------------- |
| <expr_op> | Unary  |  `not`   | logical NOT | `not <expr>`        |
| <expr_op> | Binary |   `or`   | logical OR  | `<expr> or <expr>`  |
| <expr_op> | Binary |  `and`   | logical AND | `<expr> and <expr>` |

### 2.2.5. Other Operators {#other-operators}

|  Type  | Operator | Name                 | Syntax                                                    |
| :----: | :------: | -------------------- | --------------------------------------------------------- |
| Unary  |  `comp`  | Compile-time         | See [Compile-time expressions](#compile-time-expressions) |
| Unary  |   `&`    | Address Of           | See [Address Of](#address-of)                             |
| Unary  |   `*`    | Pointer Dereference  | See [Pointer Dereference](#pointer-dereference)           |
| Binary |   `as`   | Type Cast            | See [Type Cast](#type-cast)                               |
| Binary |   `.`    | Struct Member Access | See [Struct Member Access](#struct-member-access)         |
|   -    |   `=>`   | Function Arrow       | See [Function Type](#function-type)                       |
| Binary |   `=`    | Assignment           | See ... TODO                                              |
| Binary |   `:=`   | Variable Definition  | See ... TODO                                              |

### 2.2.6. Operator Precedence {#operator-precedence}

| Level | Operators                       |
| :---: | :------------------------------ |
|   -   | `=>`                            |
|  12   | `.`                             |
|  11   | `-a` `~` `not` `&a` `*a` `comp` |
|  10   | `as`                            |
|   9   | `a*b` `/` `%`                   |
|   8   | `a+b` `a-b`                     |
|   7   | `<<` `>>`                       |
|   6   | `a&b`                           |
|   5   | `\|`                            |
|   4   | `^`                             |
|   3   | `==` `!=` `<` `<=` `>` `>=`     |
|   2   | `and`                           |
|   1   | `or`                            |

### 2.2.7. Operator Associativity {#operator-associativity}

| Associativity | Operators                                                                                      |
| :-----------: | ---------------------------------------------------------------------------------------------- |
|     `<--`     | `-a ~ not &a *a comp`                                                                          |
|     `-->`     | `a+b a-b a*b ** / %` <br> `\| ^ a&b << >>` <br> `== != < <= > >=` <br> `or and` <br> `as . =>` |

## 2.3. Expressions {#expressions}

**Syntax**

| Tag               | Syntax                                                                                                                                               |
| ----------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| \<expr>           | `['(']  <literal> \| <func_call> \| <block> \| <if> \| <loop> \| <expr_op> \| <lvalue> \| <address_of> \| <type_cast> \| <explicit_comp_expr> [')']` |
| &emsp; \<literal> | See [Literals](#literals)                                                                                                                            |
| &emsp; \<block>   | See [Block](#block)                                                                                                                                  |
| &emsp; \<if>      | See [If / Else](#if-else)                                                                                                                            |
| &emsp; \<loop>    | See [Loop](#loop)                                                                                                                                    |
| &emsp; \<expr_op> | See [Operators](#operators)                                                                                                                          |

### 2.3.1. L-Values {#l-values}

| Tag       | Syntax                                            |
| --------- | ------------------------------------------------- |
| \<lvalue> | `<name> \| <ptr_deref> \| <struct_member_access>` |

#### 2.3.1.1. Pointer Dereference {#pointer-dereference}

| Tag          | Syntax                                       |
| ------------ | -------------------------------------------- |
| \<ptr_deref> | `* <expr>` where `<expr>` is of pointer type |

#### 2.3.1.2. Struct Member Access {#struct-member-access}

| Tag                     | Syntax                                                                                                            | Comment |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------- | ------- |
| \<struct_member_access> | `<expr> . (<name> \| <expr>)` where the first `<expr>` is of struct type and the last `<expr>` coerces into `u32` |

### 2.3.2. Function Call {#function-call}

| Tag          | Syntax                | Comment                            |
| ------------ | --------------------- | ---------------------------------- |
| \<func_call> | `<expr> <struct_lit>` | where `<expr>` is of function type |

### 2.3.3. Address Of {#address-of}

| Tag           | Syntax     |
| ------------- | ---------- |
| \<address_of> | `& <expr>` |

### 2.3.4. Type Cast {#type-cast}

| Tag          | Syntax             |
| ------------ | ------------------ |
| \<type_cast> | `<expr> as <type>` |

### 2.3.5. Block {#block}

A block is a collection of statements.

-   See `<stmt>` in [Statements](#statements) for the definition of a _statement_.

A block can be "broken from" using the `break` statement.

**Syntax**

| Tag              | Syntax                                       |
| ---------------- | -------------------------------------------- |
| \<block>         | `[<string_lit>] '{' [{<stmt> ;} <stmt>] '}'` |
| &emsp; \<string> | See [Literals](#literals)                    |
| &emsp; \<stmt>   | See [Statements](#statements)                |
| &emsp; \<expr>   | See [Expressions](#expressions)              |

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

<!--
#### 2.3.4.1. Break

| Tag                  | Syntax                          |
| -------------------- | ------------------------------- |
| \<break>             | `break [<string_lit>] [<expr>]` |
| &emsp; \<string_lit> | See [Literals](#literals)       |
| &emsp; \<expr>       | See [Expressions](#expressions) |
--->

### 2.3.6. Control Flow {#control-flow}

#### 2.3.6.1. If / Else {#if-else}

**Syntax**

| Tag            | Syntax                              |
| -------------- | ----------------------------------- |
| \<if>          | `if <expr> do <stmt> [else <stmt>]` |
| &emsp; \<expr> | See [Expressions](#expressions)     |

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

#### 2.3.6.2. Loop {#loop}

**Syntax**

| Tag            | Syntax                                                                                        | Comment                                                         |
| -------------- | --------------------------------------------------------------------------------------------- | --------------------------------------------------------------- |
| \<loop>        | `loop [<stmt> [;\|, <expr> [;\|, <stmt>]] do] <stmt>` where `<expr>` result in a boolean type | init statement `;` break condition `;` post iteration statement |
| &emsp; \<stmt> | See [Statements](#statements)                                                                 |
| &emsp; \<expr> | See [Expressions](#expressions)                                                               |

<!--
##### 2.3.5.2.1. Continue {#continue}

**Syntax**

| Tag         | Syntax     |
| ----------- | ---------- |
| \<continue> | `continue` |
-->

### 2.3.7. Struct Literals {#struct-literals}

**Syntax**

| Tag                        | Syntax                                                   | Comment |
| -------------------------- | -------------------------------------------------------- | ------- |
| \<struct_lit>              | `'[' [{<struct_lit_field> ;\|,} <struct_lit_field>] ']'` |
| &emsp; \<struct_lit_field> | `[<name> =] <expr>`                                      |
| &emsp; \<name>             | See [Names](#names)                                      |
| &emsp; \<expr>             | See [Expressions](#expressions)                          |

**Interpretation**

TODO

**Examples**

```rust
pos [x i32; y i32] = [3; 5];
```

### 2.3.8. Compile-time expressions {#compile-time-expressions}

**Syntax**

| Tag                   | Syntax        |
| --------------------- | ------------- |
| \<comp_expr>          | `<expr>`      |
| \<explicit_comp_expr> | `comp <expr>` |

## 2.4. Statements {#statements}

**Syntax**

| Tag      | Syntax                                                                                |
| -------- | ------------------------------------------------------------------------------------- |
| `<stmt>` | `<expr> \| <stmt_op> \| <var_def> \| <assignment> \| <continue_stmt> \| <break_stmt>` |

### 2.4.1. Variable Definition {#variable-definition}

**Syntax**

| Tag            | Syntax                                             | Comment         |
| -------------- | -------------------------------------------------- | --------------- |
| \<var_def>     | `<name> <type> [= <expr>]` <br> `<name> := <expr>` | <br> Infer type |
| &emsp; \<name> | See [Names](#names)                                |
| &emsp; \<expr> | See [Expressions](#expressions)                    |
| &emsp; \<type> | See [Types](#types)                                |

### 2.4.2. Assignment {#assignment}

| Tag           | Syntax              | Comment |
| ------------- | ------------------- | ------- |
| \<assignment> | `<lvalue> = <expr>` |         |

### 2.4.3. Continue Statement {#continue-statement}

| Tag              | Syntax     | Comment |
| ---------------- | ---------- | ------- |
| \<continue_stmt> | `continue` |         |

### 2.4.4. Break Statement {#break-statement}

| Tag           | Syntax                          | Comment |
| ------------- | ------------------------------- | ------- |
| \<break_stmt> | `break [<string_lit>] [<expr>]` |         |

## 2.5. Metadata {#metadata}

**Syntax**

| Tag            | Syntax              |
| -------------- | ------------------- |
| \<meta>        | `# <name>`          |
| &emsp; \<name> | See [Names](#names) |

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

# Union

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

# Enum

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

# Use

**Syntax**

| Tag                                                  | Syntax                                             |
| ---------------------------------------------------- | -------------------------------------------------- |
| \<use>                                               | `use (<namespace_entity> \| <namespace_access>) ;` |
| &emsp; \<namespace_entity>, <br> \<namespace_access> | See [Namespace](#namespace)                        |

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

</div>
</body>
