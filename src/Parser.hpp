#pragma once

#include "Token.hpp"
#include "ASTNode.hpp"

#include <vector>

namespace ry {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);

    private:
        const std::vector<Token>& m_tokens;
    };

}