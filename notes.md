```
u8 x;
const u8 x;
```

```
u8 hello () {
    return 3;
}
```

```
// @private is only checked at compile-time
struct Person {
    @private String name;
    u32 age;
}

extend Person {
    @private // ignores @private in struct
    Person new (String name, u32 age) {
        Person p;
        p.name = name;
        p.age = age;
        return p;
    }

    void kill (Person self) {
        print(self.name + " has died");
    }
}

Person p = Person::new("marcus", 18);
p.kill();
```

# Block

block returns value

```
u8 x = {
    3;
}
```

# Conditionals

```
u8 x = if( true ) {
    3;
} else if( false ) {
    5;
} else {
    8;
}
```

```
u8 x = 3;
u8 y = switch( x ) {
    case 5: 8;
    case 8: 5;
}
```
switch doesnt "pass through"

```
bool x = false;
print(x ? "true" : "false");
```

# Loops

```
for( u8 i = 0; i < 10; i++ ) {
    print(i);
}
```

```
while( true ) {
    print(1);
}
```

```
do {
    print(1);
} while( false );
```

`break`
`continue`

# Vars

```rs
u32 * ptr; // == ?
const (const u32 *) ptr;
!u32* !;
u32 !padding = 69;

(u32*)* ptr2;
u32 ** ptr2;

(u32*)* !ptr;



*u32 !ptr;
uint32_t * const ptr;

!*!(!*!u32) ptr;
const (const uint32_t * const) * const ptr;
```