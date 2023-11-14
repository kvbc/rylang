#pragma once

#include <cstddef>

namespace ry {

    struct SourcePosition {
        SourcePosition(std::size_t startLine, std::size_t startColumn);
        SourcePosition(std::size_t startLine, std::size_t startColumn, std::size_t endLine, std::size_t endColumn);

        std::size_t startLine;
        std::size_t startColumn;
        std::size_t endLine;
        std::size_t endColumn;
    };

};