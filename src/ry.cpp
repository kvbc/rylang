#include "Lexer.hpp"
#include "Parser.hpp"
#include "ASTNode.hpp"

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

    std::cout << header << " Lexer Info" << std::endl;
    std::cout << lexer.GetInfos().Stringify() << std::endl;

    ry::Parser parser(tokens, lexer.GetInfos());
    std::optional<ry::ASTNode> ast = parser.Parse();

    std::cout << header << " AST" << std::endl;
    if(ast.has_value()) {
        if(auto * expr = std::get_if<ry::ASTNode::Expression>(&ast.value().Get()))
            std::cout << expr->Stringify() << std::endl;
        else if(auto * type = std::get_if<ry::ASTNode::Type>(&ast.value().Get()))
            std::cout << type->Stringify() << std::endl;
    }
    std::cout << std::endl;

    std::cout << header << " Parser Info" << std::endl;
    std::cout << parser.GetInfos().Stringify() << std::endl;
}