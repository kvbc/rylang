$Vector2 = {
    x i32;
    y i32;

    add( self $Vector2, other $Vector2 ) $Vector2 = {
        self.x += other.x;
        self.y += other.y;
        break self;
    }
};

main() void {
    a $Vector2 = { .x = 1; .y = 1; };
    b $Vector2 = { .x = 2; .y = 2; };

    a.add(b);

    // is the same as

    $Vector2:add(a, b);
}

