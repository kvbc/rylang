void pure( int a, int b, int c,
           int * ret_a, int * ret_b, int * ret_c ) {
    *ret_a = c;
    *ret_b = b;
    *ret_c = a;
}

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    pure(a, b, c, &a, &b, &c);
}

// 

pure ${a i32; b i32; c i32} => pure:$Ret = {
    $Ret = ${a i32; b i32; c i32};
    break { c; b; a };
};
main ${} => ${} = {
    a i32 = 1;
    b i32 = 2;
    c i32 = 3;
    ret pure:$Ret = pure{a, b, c};
    a = ret.a;
    b = ret.b;
    c = ret.c;
}