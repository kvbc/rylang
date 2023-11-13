min := 67;
max := 100;
firstEven ?i32 = "even" {
    loop(n := min; n <= max;; n++) {
        if n % 2 == 0 {
            break "even" n;
        }
    }
};