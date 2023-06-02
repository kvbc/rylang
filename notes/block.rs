max usize = 10;
x isize = for( a usize = max; a > 0; a -= 1 ) "a" {
	for (b usize = max; b > 0; b -= 1) "b" {
		if (a + b == a * b) {
			break "a" (a + b);
		}
	}
}
assert(x >= 0);
