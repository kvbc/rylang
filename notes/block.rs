max usize = 10;
x isize = "x" {
    for( a usize = max;; a > 0; a -= 1 ) {
        for( b usize = max;; b > 0; b -= 1 ) {
            if( a + b == a * b ) {
                break "x" (a + b);
            }
        }
    }
    break "x" -1;
}