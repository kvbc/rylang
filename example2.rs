/*

* - u can only &,*

*/

^char = u8;
$String = {
    @private buf *!^char;
    @private len u32;
}
:String = {
    init (
        self !* @private $String,
        buf !*!^char,
        len u32
    )void = {
        self.buf = buf;
        self.len = len;
    }
}

@main main ()void = {
    s $String;
    :String:init(&s, "hello", 5);
}