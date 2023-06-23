#include "ry.h"
#include "lex/lexer.h"
#include "util/arr.h"
#include "util/arr_view.h"
#include "util/dyn_arr.h"
#include "util/str.h"

#include <stdio.h>

int main (void) {
    struct ryU_ArrView srcview;
    ryU_ArrView_init(&srcview, sizeof(u8));
    const u8 * src = ryU_cstr(
        "if( x == 3 ) {"
        "   break;"
        "}"
        "\"this is a string\""
        "\"this is a\\nnl \\tstring \\10 nl 2!!! \\n \\75\\0x55\\0124\\65\\0x53 \\0xFE \""
        " 0_  "
    );
    ryU_ArrView_set(&srcview, src, ryU_cstr_len(src));
    struct ryU_DynArr srcdyn;
    ryU_DynArr_init_view(&srcdyn, &srcview);

    struct ryU_ArrView idview;
    ryU_ArrView_init(&idview, sizeof(u8));
    const u8 * id = ryU_cstr("main.test");
    ryU_ArrView_set(&idview, id, ryU_cstr_len(id));
    struct ryU_DynArr iddyn;
    ryU_DynArr_init_view(&iddyn, &idview);

    struct ryL_Lexer lex;
    ryL_Lexer_init(&lex, &iddyn, &srcdyn);

    ryL_Lexer_lex(&lex);

#ifdef RY_DEBUG
    ryL_Lexer_tokens_to_string(&lex, NULL);
    puts("------------");
    ryL_Lexer_msgs_to_string(&lex, NULL);
#endif

    ryL_Lexer_free(&lex);
}