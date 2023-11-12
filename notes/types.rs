Person = [
    firstName, lastName *!char;
    middleName ?*!char = null;
];

// 

std = {
    option[T type] => [
        value T;
        ok bool;
    ] = [
        value = ? // what should value be initialized to, if pointers can not be NULL ?
        ok = false
    ];
};

Person = [
    firstName, lastName *!char;
    middleName std:option[*!char];
];

// 

main[] => [] = (import "std"):print["Hello, World!"];

// 

main[] => [] = {
    x := 3;
    x = 5; // error! x is immutable by default

    y $*i32 = 6;
    y = 1;
};

// 

Vec2 = [x, y i32];

pos Vec2 = [1, 2];