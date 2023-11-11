main[] => [] = {
    nums = [1; 2; 3];
    // the compiler infers the type of [1;2;3]
    // as an array (struct) !!!
    // of type [i32 * 3];
    nums = [1, 2, 3];
};

//

std = import "std";
math = std:math;

Vec2 = [
    x i32;
    y i32;

    distTo[this !*!Vec2, other !*!Vec2] => i32 = {
        break math:sqrt((this.x - other.x)**2 + (this.y - other.y)**2);
    };
];

main[] => [] = {
    x Vec2 = [1, 2];
    y Vec2 = [3, 5];
    dist i32 = x:distTo(y);
};

// 

List = [i32 * 3];
gList ?*List;

processList[list *List] => [] = {
    gList = list;
};

main[] => [] = {
    list List = [1, 2, 3];
    processList(&list);
};

// 

