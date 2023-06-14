// runs at runtime
add1${ a i32; b i32 } => i32 = {
    break a + b;
};

// runs at runtime
add2${ @comp a i32; b i32 } => i32 = {
    break a + b;
};

// runs at comp-time, all parameters marked as @comp
add3${ @comp a i32; @comp b i32 } => i32 = {
    break a + b;
};

////////////////////////////////////////////////////////////////////

main ${} => ${} = {
    c1 i32 = add1{1; 2};
    c2 i32 = add2(69; 3);
    c3 i32 = add3(10, 20);
};