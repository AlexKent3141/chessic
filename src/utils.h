#ifndef __CHESSIC_UTILS_H__
#define __CHESSIC_UTILS_H__

#include "stddef.h"

struct TokenState
{
    char* str;
    size_t start;
    size_t len;
};

char* Token(
    char* str,
    char delimiter,
    struct TokenState* state);

#endif /* __CHESSIC_UTILS_H__ */
