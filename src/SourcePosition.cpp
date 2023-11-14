#include "SourcePosition.hpp"

namespace ry {
    
    SourcePosition::SourcePosition(std::size_t startLine, std::size_t startColumn):
        startLine(startLine),
        startColumn(startColumn),
        endLine(startLine),
        endColumn(startColumn)
    {}

    SourcePosition::SourcePosition(std::size_t startLine, std::size_t startColumn, std::size_t endLine, std::size_t endColumn):
        startLine(startLine),
        startColumn(startColumn),
        endLine(endLine),
        endColumn(endColumn)
    {}
    
}
