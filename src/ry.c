#include "core.h"
#include "lexer.h"
#include "array.h"

#include <stdio.h>

int main (void) {
    const char * src = "+=+-true-+asdasd-----so-how-are-you-guys-feeling-in-return-from-the-old-days?";
    RY_SIZE_T srclen = strlen(src);

    struct ry_Lexer lex;
    ry_Lexer_init(&lex, src, srclen);

    ry_Lexer_run(&lex);
    for( RY_SIZE_T i = 0; i < lex.tokens.len; i++ ) {
        struct ry_LexerToken * token = (struct ry_LexerToken *)ry_Array_get(&lex.tokens, i);
        ry_LexerToken_print(token);
        printf("\n");
    }

    ry_Lexer_free(&lex);
}