ast := (import "std").ast

#macro A := [x i32]
#macro B := [x i32]

#macro has_field_x[struct ast.Type & is_struct] => bool = struct:has_field[ast i32 x]

add[
    #macro T ast.Type & is_struct,
    a T,
    b T
] => i32 = a.x + b.x

a A := [3];
b B := [6];
c := add[a, b]

//

// idk figure it out