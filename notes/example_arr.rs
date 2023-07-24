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
    @init init <T>(self !* @private $Array<T>)void = {
        self.sz = 0;
        self.buf = :std:mem:malloc(sizeof(T) * 1000);
    }

    push <T>(self !* @private $Array<T>, val T)void = {
        self.buf[self.sz] = val;
        self.sz++;
    }

    @free free <T>(self !* @private $Array<T>)void = {
        :std:mem:free(self.buf);
    }
}

@main main ()void = {
    arr $Array<i32>;
    :Array:init<i32>(&arr);
    :Array:push<i32>(&arr, 10);
    :Array:free<i32>(&arr);
}