Result ${
    @comp Tok ast:$Node;
    @comp Terr ast:$Node
} => ast:$Node & is_struct = {
    break ${
        _v @union ${
            @pub ok Tok;
            @pub err Terr;
        }
        _ok bool;

        @pub ok${ v Tok } => self = {
            break {
                _ok = true;
                _v = { ok = v };
            };
        }

        @pub err${ v Terr } => self = {
            break = {
                _ok = false;
                _v = { err = v };
            };
        }

        @pub is_ok${ res self } => bool = {
            break res._ok;
        }

        @pub get_ok${ res self } => Tok = {
            assert{res:is_ok{}};
            break res._v.ok;
        }

        @pub get_err${ res self } => Terr = {
            assert{!res:is_ok{}};
            break res._v.err;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

divf${ a f32; b f32 } => :res = {
    res var = Result{f32; i32};
    if(b == 0)
        return res:err{69};
    return res:ok{a / b};
}

main${} => ${} = {
    a f32 = 3;
    b f32 = 5;
    res Result{f32; i32} = divf{a; b};
    if(res:is_ok{})
        print{"{} / {} = {}"; a; b; res:get_ok{}};
    else
        print{"{} / {} = ERR {}"; a; b; res:get_err{}};
}