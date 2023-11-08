@export alloc <T>(u32 count)*T {
    break C_malloc(sizeof(T) * count);
}

@export free <T>(ptr *T)void {
    C_free(ptr);
}

@export print_num (i32 x)void {
    C_printf("%d\n", x);
}