Vec2 var = [
    x i32;
    y i32;
]

Color var = @enum [
    R;
    G;
    B;
]

// just an empty struct
Namespace var = {
    // ...
}

MyUnion var = @union [

]

Pair[ T1 ast:$Type; T1 ast:$Type ] => ast:Struct = {
    break [
        a T;
        b T;
    ]
}

MyVec2 var = Pair[i32; i32];


add[ a i32; b i32 ] => i32 = a + b;

////////////////////////////////////////////////////////////////////////////////////////// 

std var = [
    std var = [
        Vec2 var = [
            x i32;
            y i32;
        ]
    ]
]

main[] => [] = {
    pos std:math:Vec2 = [6; 7];
}

////////////////////////////////////////////////////////////////////////////////////////// 