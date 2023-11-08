#include <iostream>
#include "Lexer.hpp"

int main() {
    ry::Lexer lexer(
        "Hello! \n"
        "// Everyone \n"
        "// This is a /*string*/hmm :) \n"
        "if you like it or not as i do lul \n"
        "3 + 5 = 3 += 4 ?? .. huh?"
    );
    std::cout << lexer.GetSource() << std::endl;
    std::cout << std::string(20, '-') << std::endl;
    std::vector<ry::Token> tokens = lexer.Lex();
    for(const ry::Token& token : tokens)
        std::cout << token.Stringify() << std::endl;
}