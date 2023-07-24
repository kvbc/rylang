Result[
    Tok ast:Node;
    Terr ast:Node;
] => ast:Node & is_struct = { break [
    _v @union [
        @pub ok Tok;
        @pub err Terr;
    ];
    _ok bool;

    @pub ok[v Tok] => self = {
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
]}