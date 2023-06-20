#ifndef RYUSTR_STR_VIEW_H
#define RYUSTR_STR_VIEW_H

#include "../core.h"

// 
// 
// 

struct ryUSTR_StringView {
    usize _len;
    const u8 * _buf;
};

void ryUSTR_StringView_init_buflen( struct ryUSTR_StringView * out_view, const u8 * buf, usize len );
void ryUSTR_StringView_init_buf   ( struct ryUSTR_StringView * out_view, const u8 * buf );
void ryUSTR_StringView_init_view  ( struct ryUSTR_StringView * out_view, struct ryUSTR_StringView * view );
void ryUSTR_StringView_init_alloc ( struct ryUSTR_StringView * out_view, struct ryUSTR_AllocStr * str );
void ryUSTR_StringView_init_dyn   ( struct ryUSTR_StringView * out_view, struct ryUSTR_DynStr * str );

// Getters
usize ryUSTR_StringView_getlen( const struct ryUSTR_StringView * view );
const u8 * const ryUSTR_StringView_getbuf( const struct ryUSTR_StringView * view );

// 
// 
// 

#endif // RYUSTR_STR_VIEW_H