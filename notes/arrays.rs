Crazy var = [
    i32 * 2;
    f32 * 2;
    u32 * 2;
]
// same as
Crazy var = [
    i32; i32;
    f32; f32;
    u32; u32;
]

main[] => [] = {
    arr [i32 * 3] = [1; 2; 3];
    // same as
    arr [i32;i32;i32] = [1; 2; 3];

    arr.0 = 69;
    arr.1 = 420;
    arr.2 = 2137;

    i u32 = 0;
    arr.(i) = 666;

    x Crazy = [-1; -2; 3.4; 5.6; 7; 8];
}