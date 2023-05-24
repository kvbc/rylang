# rylang

# Blocks

```rs
{
    ...
} // standalone
```

```rs
{
    if( true )
        break;
    print("oops");
} // "breakable"
```

```rs
u8 x = {
    break 3;
}; // return
```

```rs
u8 x = {}; // x = ?
```

# Conditionals

```rs
if( true )
    print("true");
else
    print("false")
```

```rs
u8 x = if( true )
    break 1;
else
    break 0;
```

```rs
u8 x = if( true ) {} // x = ?
```

```rs
u8 x = true ? 1 : 0;
```

Loops

```rs
u32 x = loop {
    @static u32 i = 0;
    if( i % 10 == 0 )
        break i;
    i++;
    if( i >= 100 )
        break;   
}
print(x) // 10
```

# Examples

```rs
struct Rect {
    @private i32 x;
    @private i32 y;
    @private u32 w;
    @private u32 h;
}
namespace Rect {
    @internal
    void init( !* struct Rect self, i32 x, i32 y, u32 w, u32 h ) {
        assert(*self == ?);
        self.x = x;
        self.y = y;
        self.w = w;
        self.h = h;
    }
}

@main
void main() {
    struct Rect r; // == ?
    !* struct Rect rptr = &r;
    // error: no pointer math
    // rptr += 2;
    Rect::init(rptr);

    // no error
    @unsafe *! ptr = &r;
    ptr++;
}
```

```rs
struct Array {
    @private u32 len;    // == ?
    @private u32 cap;    // == ?
    @private i32 * data; // == ?
    bool something;      // == ?
    @private bool other; // == ?
}

extend Array {
    void non_internal () {
        print("test");
    }
    
    @internal
    @private void init( Array& self ) {
        self.len = 0;
        self.cap = 0;
        self.data = null;
        self.something = false;
        // warning! unitialized struct member .other
        // a.other = false;
    }
}

extend Array {
    @internal // can access private stuff
    @static Array new () {
        // bellow error: cannot access ?
        // Array a; // (a == ?)
        Array a = {}; // must init all vars
        a.init(); // from prev extend
        a.non_internal();
        return a;
    }
    
    @internal
    void free( Array& self ) {
        // free stuff
        free(self.data);
        
        // if any struct member is not ?
        // and its going out of scope, an error will be printed out
        // telling you to free your memory
        self.len = ?;
        self.cap = ?;
        self.data = ?;
        self.something = ?;
        self.other = ?;
        // all of the above or: self = {?};
    }
    
    @internal
    u32 get_len (Array& self) {
        return self.len;
    }
    
    @internal
    i32 get (Array& self, u32 idx) {
        assert(idx < self.len);
        return self.data[idx];
    }
    
    @internal
    @private void print (Array& self) {
        self.something = true;
        print("array");
    }
    
    void public_print () {
        print("i am not internal!");
    }
    
    @internal
    i32 push (Array& self, u32 val) {
        if( self.len >= self.cap ) {
            if( self.cap == 0 )
                self.cap = 5;
            else
                self.cap += self.cap / 2;
            self.data = realloc(self.data, self.cap * sizeof(*data));
        }
        self.data[self.len] = val;
        self.len++;
    }
    
    // only can set other, not get
    @internal
    void set_other( Array& self, bool val ) {
        self.other = val;
    }
}

void main () {
    Array arr = Array.new();
    arr.push(0);
    arr.something = false;
    arr.set_other(true);
} // arr goes out of scope: error - arr has not been freed
// you have to call arr.free() manually
```
