main[] => [] = {
    nums = [1; 2; 3];
    // the compiler infers the type of [1;2;3]
    // as an array (struct) !!!
    // of type [i32 * 3];
    nums = [1, 2, 3];
};