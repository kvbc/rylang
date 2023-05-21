#ifndef RY_LEXER_INFO_H
#define RY_LEXER_INFO_H

#include <stdlib.h>

enum ry_LexerInfoCode {
    RY_LEXINFO_EMPTY_INPUT
};

struct ry_LexerInfo {
    enum ry_LexerInfoCode code;
};

const char * ry_LexerInfo_get_desc (struct ry_LexerInfo * info) {
    switch( info->code ) {
        case RY_LEXINFO_EMPTY_INPUT: return "empty input buffer";
    }
    return NULL;
}

#endif // RY_LEXER_INFO_H