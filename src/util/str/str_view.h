#ifndef RYUSTR_STR_VIEW_H
#define RYUSTR_STR_VIEW_H

#include "../core.h"

// 
// 
// 

struct ryUSTR_StrView {
    usize _len;
    const u8 * _buf;
};

void ryUSTR_StrView_init_buflen( struct ryUSTR_StrView * out_view, const u8 * buf, usize len );
void ryUSTR_StrView_init_buf   ( struct ryUSTR_StrView * out_view, const u8 * buf );
void ryUSTR_StrView_init_view  ( struct ryUSTR_StrView * out_view, struct ryUSTR_StrView * view );
void ryUSTR_StrView_init_alloc ( struct ryUSTR_StrView * out_view, struct ryUSTR_AllocStr * str );
void ryUSTR_StrView_init_dyn   ( struct ryUSTR_StrView * out_view, struct ryUSTR_DynStr * str );

// Getters
usize ryUSTR_StrView_getlen( const struct ryUSTR_StrView * view );
const u8 * const ryUSTR_StrView_getbuf( const struct ryUSTR_StrView * view );

// 
// 
// 

#endif // RYUSTR_STR_VIEW_H