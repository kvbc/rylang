#include "core.h"
#include "lexer.h"
#include "array.h"

#include <stdio.h>

int main (void) {
    struct ry_StringView src = ry_StringView_frombuf("+=+-true-+asdasd-----so-how-are-you-guys-feeling-in-return-from-the-old-days?\"this is\n a string");
    struct ry_StringView id = ry_StringView_frombuf("main.test");

    struct ry_Lexer lex;
    ry_Lexer_init(&lex, &id, &src);

    ry_Lexer_run(&lex);
    for( RY_SIZE_T i = 0; i < lex.tokens.len; i++ ) {
        struct ry_LexerToken * token = (struct ry_LexerToken *)ry_Array_get(&lex.tokens, i);
        ry_LexerToken_print(token);
        printf("\n");
    }
    puts("------------");
    ry_Lexer_print_infos(&lex);

    ry_Lexer_free(&lex);
}