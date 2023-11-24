#macro Vec2 := ast [
    i32 x;
    i32 y;
]

#macro add_a_b := ast a + b;

pos Vec2 = [1, 2]

a := 1;
b := 2;
c := add_a_b