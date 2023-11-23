# Language Specification

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
<div style="width:300px; position: fixed; font-size: 12px;">
	<ol>
		<li>
			<a href="#introduction">Introduction</a>
			<ol>
				<li><a href="#notation">Notation</a></li>
			</ol>
		</li>
		<li>
			<a href="#lexical-analysis">Lexical Analysis</a>
			<ol>
				<li><a href="#source-code">Source Code</a></li>
				<li><a href="#names">Names</a></li>
				<li><a href="#comments">Comments</a></li>
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
				<li><a href="#tokens">Tokens</a></li>
			</ol>
		</li>
		<li>
			<a href="#syntax-analysis">Syntax Analysis, Semantic Analysis, Typing</a>
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
	<ol>
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

# 1. Introduction {#introduction}

## 1.1. Notation {#notation}

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

# 2. Lexical Analysis {#lexical-analysis}

Lexical analysis is the process of grouping source code characters into tokens.

## 2.1. Source Code {#source-code}

**Grammar**

```ebnf
<new_line>   = \n | \r | \r\n (* LF | CR | CRLF *)
<src_char>   = (* Any ASCII character in range of <1, 127> and not <new_line> *)
<whitespace> = ' ' | \t | \v | \f | <new_line>
<eos>        = (* end of source code *)
```

**Warnings**

-   Mixed new-line encodings

**Errors**

-   NUL character (0) in source code

## 2.2. Names {#names}

**Grammar**

```ebnf
<name>                = (<name_start_char> {<name_char>}) ~ <keyword>
    <name_start_char> = _ | a-z | A-Z
    <name_char>       = <name_start_char> | 0-9
```

**References**

-   [\<keyword\>](#keywords)

## 2.3. Comments {#comments}

**Grammar**

```ebnf
<comment> = // {<src_char>} (<new_line> | <eos>)
          | /* ({<src_char>} ~ */) */
```

**References**

-   [\<src_char\>, \<new_line\>, \<eos\>](#source-code)

**Errors**

-   Unterminated multi-line comment

## 2.4. Literals {#literals}

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

-   [\<int_lit\>](#integer-literals)
-   [\<float_lit\>](#float-literals)
-   [\<string_lit\>](#string-literals)
-   [\<char_lit\>](#char-literals)
-   [\<struct_lit\>](#struct-literals)

### 1.4.1. Integer Literals {#integer-literals}

**Grammar**

```ebnf
<int_lit> = 0b 0|1 {{_} 0|1 {_}}
          | 0o 0-7 {{_} 0-7 {_}}
          |    0-9 {{_} 0-9 {_}}
          | 0x <hex_digit> {{_} <hex_digit> {_}}
<hex_digit> = (0-9) | (a-f) | (A-F)
```

**Errors**

TODO

<!--
| Tag        | Error                        | Syntax                                                                                                                                                                                                           | Comment |
| ---------- | ---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- |
| \<int_lit> | ❗ Malformed integer literal | `0(b\|o)(<src_char> ~ 0-9)` <br> `0x(<src_char> ~ <hex_digit>)` <br> <br> `(<bin_int_lit> \| <oct_int_lit>) <alpha>` <br> `(<hex_int_lit> (<alpha> ~ <hex_digit>))` <br> <li> where `<alpha> = a-z \| A-Z` </li> |         |
| \<int_lit> | ❗ Invalid digit             | `(0b \| <bin_int_lit>) 0-9` or <br> `(0o \| <oct_int_lit>) 0-9` or <br> `(0x \| <hex_int_lit>) 0-9`                                                                                                              |
-->

### 1.4.2. Float Literals {#float-literals}

**Grammar**

```ebnf
<float_lit> = <int_lit> . <int_lit> [<float_exp>]
            | <int_lit> <float_exp>
<float_exp> = e|E [+|-] <int_lit>
```

**References**

-   [\<int_lit\>](#integer-literals)

**Errors**

TODO

<!--
| Tag          | Error                       | Syntax                                     |
| ------------ | --------------------------- | ------------------------------------------ |
| \<float_lit> | ❗ Unfinished float literal | `<dec_int_lit> . ~<dec_int_lit>`           |
| \<float_lit> | ❗ Unfinished exponent      | `<dec_int_lit> e\|E [+\|-] ~<dec_int_lit>` |
-->

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

### 1.4.5. Boolean Literals {#bool-literals}

| Tag         | Syntax           |
| ----------- | ---------------- |
| \<bool_lit> | `true  \| false` |

### 1.4.6. Null Literal {#null-literal}

| Tag         | Syntax |
| ----------- | ------ |
| \<null_lit> | `null` |

## 1.5. Keywords {#keywords}

| Tag                 | Syntax                        |
| ------------------- | ----------------------------- |
| \<keyword>          | `<primitive>` or any of below |
| &emsp; \<primitive> | See [Primitives](#primitives) |

```
do
if else
loop continue break
false true null
not or and
as comp
```

## 1.6. Operators {#lexical-operators}

Refer to [Operators](#operators)

## 1.7. Tokens {#tokens}

Tokens represent:

-   [Names](#names)
-   [Keywords](#keywords)
-   [Operators](#operators)
-   [Literals](#literals)
-   Characters: `( ) ; [ ] & * ? ~ , { }`

# 2. Parsing and Semantic Analysis {#parsing-and-semantic-analysis}

## 2.1. Types {#types}

| Tag                       | Syntax                                                                                                     |
| ------------------------- | ---------------------------------------------------------------------------------------------------------- |
| \<type>                   | `[<mut_type_attrib>] [<opt_type_attrib>] (<primitive_type> \| <func_type> \| <struct_type> \| <ptr_type>)` |
| &emsp; \<mut_type_attrib> | See [Type Mutability](#type-mutability)                                                                    |
| &emsp; \<opt_type_attrib> | See [Optional Types](#optional-types)                                                                      |
| &emsp; \<primitive_type>  | See [Primitive Types](#primitive-types)                                                                    |
| &emsp; \<func_type>       | See [Function Type](#function-type)                                                                        |
| &emsp; \<struct_type>     | See [Struct Type](#struct-type)                                                                            |
| &emsp; \<ptr_type>        | See [Pointer Types](#pointer-types)                                                                        |

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

| Tag                    | Syntax                                                                                                                                                                                                                                                                                                                                                 |
| ---------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| \<struct_type>         | `'[' [{<struct_field> ,\|;} <struct_field>] ']'`                                                                                                                                                                                                                                                                                                       |
| &emsp; \<struct_field> | `<name>{,<name>} <type> [= <comp_expr>]` where `<comp_expr>` coerces into `<type>` <br> `<type> [* <comp_expr>] [= <comp_expr>]` where the first `<comp_expr>` is of type `u32` and the second coerces into `<type>` <br> `[<comp_expr> *] <type> [= <comp_expr>]` where the first `<comp_expr>` is of type `u32` and the second coerces into `<type>` |
| &emsp; \<name>         | See [Names](#names)                                                                                                                                                                                                                                                                                                                                    |
| &emsp; \<type>         | See [Types](#types)                                                                                                                                                                                                                                                                                                                                    |
| &emsp; \<comp_expr>    | See [Compile-time expressions](#compile-time-expressions)                                                                                                                                                                                                                                                                                              |

**Examples**

```rust
pos [x i32, y i32 = 9] = [3, 5];

list [i32 * 3] = [1, 2, 3];

tuple [i32, bool, f32] = [1, true, 2.3];
```

### 2.1.4. Pointer Types {#pointer-types}

| Tag         | Syntax     |
| ----------- | ---------- |
| \<ptr_type> | `* <type>` |

### 2.1.5. Type Attributes {#type-attribs}

#### 2.1.5.1. Type Mutability {#type-mutability}

| Tag                | Syntax |
| ------------------ | ------ |
| \<mut_type_attrib> | `~`    |

#### 2.1.5.2. Optional Types {#optional-types}

| Tag                | Syntax |
| ------------------ | ------ |
| \<opt_type_attrib> | `?`    |

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
