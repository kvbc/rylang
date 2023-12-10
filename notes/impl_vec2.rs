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

//
// okay what if types are comp-time expressions of type (#comp ast.Node & type)
//

Int #comp ast.Type = i32;
x Int = 5;

type := #comp ast.Type;
Int type = i32;
y Int = 6;

inc := $$(x += 3)
x := 5;
$inc;
// x == 8

//
//
//

a := [
    b := [
        c := [
            d := macro(x += 3);
        ]
    ]
]
x := 1;
a.b.c.d;
// x == 4

//
// types implicitly cast to (#comp ast.Type)
// or types are always compile-time expressions of type (#comp ast.Type)
//

use import "ast";

Pair[T1 Type, T2 Type] => Type = [a T1, b T2];
ab Pair[i32, f32] = [1, 2];

//
//
//

pow_x := ast({
    x *= x;
});
main[] => [] = {
    x := 2;
    pow_x;
    // same as
    x := 2;
    {x *= x};
}

//
//
//

use [X = 3];
std.print[X]; // 3