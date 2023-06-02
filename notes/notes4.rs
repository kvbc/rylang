List (comptime T type) type {
	return ${
		elems[]T;
		len usize;
	};
}

^IntList = List(i32);
list ^IntList = ...

options ${
	ok bool;
	...
} = {
	.ok = false
	...
}

options ${
	ok bool = false;
} = {};

$List<T> = {
	elems []T;
	len usize;

	new ()$List<T> = {
		list $List<T> = {
			.elems = [],
			.len = 0
		};
		break list;
	}
};

list $List<i32> = :$List<i32>:new();

hello <T>(x T)T {
	break x;
}
x i32 = hello<i32>(3);

/*

Generics only on:
- Structs
- Functions
- Aliases

*/
