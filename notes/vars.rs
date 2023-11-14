x i32 = 3;
x ?i32;
x := 3;

// 

Vec2 := [x, y i32];

[x, y] Vec2 := [1, 3];

// 

Person = [
    fn string;
    mn ?string;
    ln string;
]
guy Person = [fn = "Mike", ln = "Tyson"];
[firstName = fn, lastName = ln] := guy;
