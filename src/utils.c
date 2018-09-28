#include "utils.h"
#include "../include/chessic.h"
#include "ctype.h"

char loc_to_char(int col, int type)
{
    static const char c[] = { '.', 'p', 'n', 'b', 'r', 'q', 'k' };
    return col == WHITE ? toupper(c[type]) : c[type];
}

