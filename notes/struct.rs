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

    addv( self $Vector2, val i32 ) $Vector2 = {
        self.x += val;
        self.y += val;
        break self;
    }

    add( self $Vector2, other $Vector2 ) $Vector2 = {
        self.x += other.x;
        self.y += other.y;
        break self;
    }

    subv( self $Vector2, val i32 ) $Vector2 = {
        self.x -= val;
        self.y -= val;
        break self;
    }

    sub( self $Vector2, other $Vector2 ) $Vector2 = {
        self.x -= other.x;
        self.y -= other.y;
        break self;
    }
}

main() void {
    a $Vector2 = $Vector2:new(1, 1);
    b $Vector2 = $Vector2:new(2, 2);

    a.add(b).addv(3).subv(1) // [5, 5]
    
    $Vector2:subv(
        $Vector2:addv(
            $Vector2:add(a, b),
            3
        ),
        1
    ); // [5, 5]
}