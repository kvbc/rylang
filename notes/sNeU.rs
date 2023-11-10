// struct, namespace, enum, union

// struct
Vec2 = {
    x i32;
    y i32
};

// namespace
std = {
    add {a i32; b i32} => i32 = {
        break a + b;
    }
};

main {} => {} = {
    c i32 = std:add(1, 2);
    a = {
        break 3;
    }
    mus = {
        x i32 = 5;
    }
}

[a i32; b i32] -> i32

add[a i32; b i32] i32 = {

}