// @ - compexpr to ast
// @@ - ast to compexpr

ast := import "ast";

Vector2 := @@[i32 * 2];
// Vector2 ast.Node := @@[i32 * 2];

xy @Vector2 := [1, 2];

//
//
//

return_if_x_true := @@ if x == true do break;
main[] => [] = {
    x := true;
    @return_if_x_true;
    // ^^^ same as vvv
    if x == true do break;
}

//
//
//

x := 1;

inc_x := {
    x += 1;
}

inc_x[] // how do we know if inc_x is a func?