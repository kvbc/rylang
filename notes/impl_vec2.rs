Vector2 := $[
    x, y #i32;
    
    add[self *#Vector2, other *Vector2] => [] = {
        self.x += other.x;
        self.y += other.y;
    }
]

main[] => [] = {
    pos Vector2 = [1, 2];
    ofs Vector2 = [3, 4];
    pos.add[&ofs];
}