#include "ry.h"
#include "lex/lexer.h"
#include "util/array.h"
#include "util/str/str_view.h"
#include "util/str/dyn_str.h"
#include "util/str/cstr.h"

#include <stdio.h>

int main (void) {
    struct ryUSTR_StrView srcview;
    ryUSTR_StrView_init_buf(&srcview, ryUSTR_cstr(
        "if( x == 3 ) {"
        "   break;"
        "}"
        "\"this is a string\""
        "\"this is a\\nnl \\tstring \\10 nl 2!!! \\n \\75\\0x55\\0124\\65\\0x53 \\0xFE \""
    ));
    struct ryUSTR_DynStr srcdyn;
    ryUSTR_DynStr_init_viewptr(&srcdyn, &srcview);

    struct ryUSTR_StrView idview;
    ryUSTR_StrView_init_buf(&idview, ryUSTR_cstr("main.test"));
    struct ryUSTR_DynStr iddyn;
    ryUSTR_DynStr_init_viewptr(&iddyn, &idview);

    struct ryL_Lexer lex;
    ryL_Lexer_init(&lex, RYL_LEXFLAG_NONE, &iddyn, &srcdyn);

    ryL_Lexer_lex(&lex);
    ryL_Lexer_print_tokens(&lex);
    puts("------------");
    ryL_Lexer_print_infos(&lex);

    ryL_Lexer_free(&lex);
}