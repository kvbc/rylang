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
Namespace var = [
    // ...
]

MyUnion var = @union [

]

Pair[ T1 ast:$Type; T1 ast:$Type ] => ast:Struct = {
    break [
        a T;
        b T;
    ]
}

MyVec2 var = Pair[i32; i32]