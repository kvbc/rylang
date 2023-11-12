// std

std = {
    rand[min i32, max i32] => i32;
    prompt[prompt !*!char] => ?*!char;
    str2num[str !*!char] => i32?;
    print[str !*!char] => [];
}

// guessing game

std = import "std";

add := {
    3;
};

main[] => [] = {
    target := std:rand[1, 100];
    loop {
        guessStr := std:prompt["Guess (1 to 100): "];
        if guessStr == null { continue }
        guessNum := std:str2num[guessStr];
        if guessNum == null { continue }
        if not (guessNum >= 1 and guessNum <= 100) { continue }
        if guessNum < target {
            std:print["Higher!"];
            continue;
        }
        if guessNum == target {
            std:print["Correct!"];
            break;
        }
        if guessNum > target {
            std:print["Lower!"];
            continue;
        }
    }
};

// 

main[] => [] = {
    x i32 = 3;
    x = 5;

    y auto = 5;
    y = 7;

    let z = 5;
    z = 8;

    w := 1;
    w = 2;
}

// 

a := true;
b := 1;
c := 2;
d := a ? b : c;
d := if a {b} else {c};

// 

Vec2 = [x i32; y i32];

ns = {
    func[] => [] => {};
};

pos Vec2 = [1, 2];

ns . x[];
ns : x[];

pos.x;
pos.y;

std.print();

std:print();

std.str(data.num);

std:str(data.num);

// 

