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

