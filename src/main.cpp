#include "Lexer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    static constexpr const char * const FILENAME = "test.ry";

    std::string src;
    {
        std::ifstream file(FILENAME);
        std::stringstream ss;
        ss << file.rdbuf();
        src = ss.str();
    }

    ry::Lexer lexer(FILENAME, src);
    
    std::string header(20, '-');
    std::cout << header << " Source" << std::endl;
    std::cout << lexer.GetSource() << std::endl;

    std::cout << header << " Tokens" << std::endl;
    std::vector<ry::Token> tokens = lexer.Lex();
    for(const ry::Token& token : tokens)
        std::cout << token.Stringify() << std::endl;

    std::cout << header << " Info" << std::endl;
    for(const ry::Lexer::Info& info : lexer.GetInfos())
        std::cout << lexer.StringifyInfo(info) << std::endl << std::endl;
}