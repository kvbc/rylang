:std = {
    @pub :ast = { ... };

    @constraint is_iterable ${
        @comp Tcontainer ast:$Struct;
        @comp Tvalue ast:$Struct
    } => bool = {
        assert Tcontainer:has_method{ "next", ${} => Tvalue };
        assert Tcontainer:has_method{ "eos",  ${} => bool };
        break true;
    };

    @constraint is_cool${ @comp T ast:$Struct } => bool = {
        assert T:has_method{ "cool", ${} => bool }
        break true;
    };

    @pub Array${ @comp T ast:$Type } => ast:$Struct = {
        break ast ${
            data *T;
            len usize;
            cap usize;

            @pub new${ size usize } => $Array = {
                break {
                    data = std:malloc(cap * sizeof T);
                    len = 0;
                    cap = size;
                };
            };

            @pub push${ arr self; val T } => ${} = {
                assert arr.len < arr.cap;
                arr.data[arr.len] = val;
                arr.len += 1;
            };

            @pub iterator${ arr self } => ast:$Struct & is_iterable(T) & is_cool() = {
                break ${
                    i usize = 0;

                    @pub next${ it self } => T = {
                        assert !it:eos();
                        v T = arr.data[it.i];
                        it.i += 1;
                        break v;
                    };

                    @pub eos${ it self } => bool = {
                        break it.i >= arr.len;
                    };

                    @pub cool${ it self } => bool = {
                        break false;
                    };
                };
            };
        };
    };
};

main ${} => ${} = {
    arr var = std:Array(i32):new(10);
    arr:push(2);
    arr:push(1);
    arr:push(3);
    arr:push(7);
    loop{ it var = arr:iterator(); !it:eos();; } {
        std:print(it:next());
    }
};