$Array<T> = {
    @private sz u32;
    @private cap u32;
    @private buf *T;
}
:Array = {
    init (self !* @private $Array)void = {
        :std:dbg:assert(*self == ?);
        self.sz = 0;
        self.cap = 0;
        self.buf = nullptr;
    }

    free (self !* @private $Array)void = {
        :std:dbg:assert(*self != ?);
        :std:mem:free(self.buf);
        *self = ?;
    }
}

@main main ()void = {
    arr $Array = ?;

    :Array:init(&arr);
    :Array:free(&arr);
}

////////////////////////////////////////////

$Array<T> = {
    @private sz u32;
    @private cap u32;
    @private buf *T;
}
:Array = {
    @init init (self !* @private $Array)void = {
        self.sz = 0;
        self.cap = 0;
        self.buf = nullptr;
    }

    @free free (self !* @private $Array)void = {
        :std:mem:free(self.buf);
    }
}

@main main ()void = {
    arr $Array;
    :Array:init(&arr);
    :Array:free(&arr);
}