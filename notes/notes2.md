# Features

TODO
- var nullability
- unitialized vars
- explicit scoping of enums?

```rs
struct Vector2 = { i32 x; i32 y; };
struct Vector2 pos;

enum Color = { RED; GREEN; BLUE; };
enum Color col;

alias char = u8;
alias char c;

namespace std = {};
std::func(); // dont like that

/*
struct       $
enum         #
alias        ^
namespace    ::
*/

$Vector2 = { i32 x; i32 y; };
$Vector2 pos;

#Color = { RED; GREEN; BLUE; };
#Color col = RED; // scoping?

^char = u8;
^char c;

::std = {}
::std::func();

```

```rs
<type> <name> ( {<params>} ) <block>

void func( i32 a, i32 b, i32 c ) {
    return a + b + c;
}

/*
^idxof_func = i32(!*!i32 arr, u32 len, i32 num)
^idxof_func idxof = {
    break loop {
        @static u32 i = 0;
        if( arr[i] == num )
            break i;
        if( ++i >= len )
            break -1;
    };
}
*^idxof_func ptr = &idxof;
*/

(i32, i32)i32 add( a, b ) {
    break a + b;
}
*(i32 a, i32 b)i32 ptr = &add;

(!*!i32, u32, i32)i32
idxof (arr, len, num) {
    break loop {
        @static u32 i = 0;
        if( arr[i] == num )
            break i;
        if( ++i >= len )
            break -1;
    };
}

```

```rs
$Vector2 = {
    i32 x;
    i32 y;
};
::Vector2 = {
    (!*$Vector2 self, i32 x, i32 y)void
    init = {
        self.x = x;
        self.y = y;
    }

    (!*$Vector2 self, i32 n)void
    add = {
        self.x += n;
        self.y += n;
    }
}
@main ()void main () {
    $Vector2 v;
    ::Vector2::init(&v, 0, 0);
    ::Vector2::add(&v, 5);

    ^v2 = :Vector2;
    :^v2:add(&v, 5);
}
```

<br>

| | |
|-|-|
\<type>                     | `[!] [* <nested_type>] <basetype>`
&emsp; \<basetype>          | `<primitive> | $ <name> | # <name> | ^ <name>` <!-- $struct, #enum, ^alias -->
&emsp; &emsp; \<primitive>  | `i8 | i16 | i32 | i64 | u8 | u16 | u32 | u64 | f32 | f64 | bool`
&emsp; \<nested_type>       | `<type>` 
&emsp; \<nested_type>       | `( [!] <nested_type> )` 
&emsp; \<nested_type>       | `[!] ( <nested_type> )` 

| | |
|-|-|
\<var>       | `<type> <name> = <expr>`
\<alias>     | `^ <name> = <type>`
\<struct>    | `$ <name> = <block>`
\<enum>      | `# <name> = <block>`
\<namespace> | `:: <name> = <block>`
\<break>     | `break [<expr>]`
\<meta>      | `@<name>`

| | |
|-|-|
\<if>          | `if ( <expr> ) <block> {<elif>} [<else>]`
&emsp; \<elif> | `elif ( <expr> ) <block>`
&emsp; \<else> | `else <block>`
\<loop>        | `loop <block>`
\<block>       | `{ <stmt> {<stmt>} }`

| | |
|-|-|
\<stmt_expr> | `<if> | <loop> | <block>`
\<stmt> | `{<meta>} <stmt_expr> | (<var> | <alias> | <struct> | <enum> | <namespace> | <break>) ;`

Variables

```rs
u8 x = 3;
```

Immutability (const)

```rs
!u8 x = 3;
```

Pointers

```rs
u8 x = 3;
*u8 ptr = &x;        // u8 * ptr
*!u8 ptr = &x;       // const u8 * ptr
!*u8 ptr = &x;       // u8 * const ptr
!*!u8 ptr = &x;      // const u8 * const ptr
!*!(!*!u8) ptr = &x; // const (const * u8 const) * const ptr
```

```rs
u8 x = 3;
*u8 ptr = &x;
ptr++; // error: no pointer math allowed
```

Type alias

```rs
alias char = u8;
alias char x = 'c';
```

Struct

```rs
struct Vector2 = {
    i32 x;
    i32 y;
}
struct Vector2 v;
v.x = 0;
v.y = 0;
```

Enum

```rs
enum Color = {
    RED;
    GREEN;
    BLUE;
}
enum Color c = RED;
```

Namespaces

```rs
namespace App {
    void run() {}
}
App::run();
```

<!--
Use
```rs
namespace App {
    void run() {}
}
if( true ) {
    use App;
    run();
}
```
-->

If / Else

```rs
if( true )
    print("hello");
else
    print("bye");

u8 x = if( true ) {
    break 1;
} else {
    break 0;
}

if( 1 ) {

} elif( 2 ) {

} else {

}
```

Loop

```rs
loop {

}
```

Metadata

```rs
// TODO
```