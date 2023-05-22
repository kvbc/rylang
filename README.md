# rylang

```rs
struct Array {
    @private u32 len;    // == ?
    @private u32 cap;    // == ?
    @private i32 * data; // == ?
    bool something; // == ?
    @private bool other;
}

extend Array {
    @internal // can access private stuff
    @static Array new () {
        // bellow error: cannot access ?
        // Array a; // (a == ?)
        Array a = {}; // must init all vars
        a.len = 0;
        a.cap = 0;
        a.data = null;
        a.something = false;
        // warning! unitialized struct member .other
        // a.other = false;
        return a;
    }
    
    @internal
    u32 get_len (Person& self) {
        return self.len;
    }
    
    @internal
    i32 get (Person& self, u32 idx) {
        assert(idx < self.len);
        return self.data[idx];
    }
    
    @internal
    @private void print (Person& self) {
        self.something = true;
        print("array");
    }
    
    void public_print () {
        print("i am not internal!");
    }
    
    @internal
    i32 push (Person& self, u32 val) {
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
    void set_other( Person& self, bool val ) {
        self.other = val;
    }
}

void main () {
    Array arr = Array.new();
    arr.push(0);
    arr.something = false;
    arr.set_other(true);
}
```
