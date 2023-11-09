#include <iostream>
#include "Lexer.hpp"

int main() {
    ry::Lexer lexer(
        "Hello! \n"
        "// Everyone \n"
        "// This is a /*string*/hmm :) \n"
        "if you like it or not as i do lul \n"
        "3 + 5 = 3 += 4 ?? .. huh? \n"
        "3_0_1 0b1___0_1_0 0o__3453_3457_ 0x6AFG\n"
        "123456789123456789123456789 \n"
        "123.123456789123456789 \n"
        "123.123456789123456789e+2 \n"
        "123.123456789123456789e2 \n"
        "123.123e-1 \n"
        "123.123456789123456789e-3 \n"
        " `ok so this work or not?` \n"
        " ``` \n"
        " well it \n"
        " does seem \n"
        " to be \n"
        " working sir \n"
        "``` \n"
        " \" ye it wukrs?? \" \n"
        " \"\"\" \n "
        " well it \n"
        " seeems so \n"
        " \"\"\" \n"
        " \" does\\nit\\nreally\\n?\" \n"

        " \"\"\" \\\n"
        " hello \\\n"
        " everywon \\\n"
        " \"\"\" \n"

        " \"\\078\" \n"

        " 'c' \n"
        " '\\0x26' \n"
    );
    std::cout << lexer.GetSource() << std::endl;
    std::cout << std::string(20, '-') << std::endl;
    std::vector<ry::Token> tokens = lexer.Lex();
    for(const ry::Token& token : tokens)
        std::cout << token.Stringify() << std::endl;
    std::cout << std::string(20, '-') << std::endl;
    for(const ry::Lexer::Info& info : lexer.GetInfos())
        std::cout << info.GetMessage() << std::endl;
}