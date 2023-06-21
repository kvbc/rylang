#ifndef RYUSTR_STR_VIEW_H
#define RYUSTR_STR_VIEW_H

#include "../../ry.h"

struct ryUSTR_AllocStr;
struct ryUSTR_DynStr;

// 
// 
// 

struct ryUSTR_StrView {
    usize _len;
    const u8 * _buf;
};

void ryUSTR_StrView_init       ( struct ryUSTR_StrView * out_view );
void ryUSTR_StrView_init_buflen( struct ryUSTR_StrView * out_view, const u8 * buf, usize len );
void ryUSTR_StrView_init_buf   ( struct ryUSTR_StrView * out_view, const u8 * buf );
void ryUSTR_StrView_init_view  ( struct ryUSTR_StrView * out_view, struct ryUSTR_StrView * view );
void ryUSTR_StrView_init_alloc ( struct ryUSTR_StrView * out_view, struct ryUSTR_AllocStr * str );
void ryUSTR_StrView_init_dyn   ( struct ryUSTR_StrView * out_view, struct ryUSTR_DynStr * str );

// Getters
usize ryUSTR_StrView_get_len( const struct ryUSTR_StrView * view );
const u8 * ryUSTR_StrView_get_buf( const struct ryUSTR_StrView * view );

// 
// 
// 

#endif // RYUSTR_STR_VIEW_H