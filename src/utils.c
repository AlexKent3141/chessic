#include "utils.h"
#include "../include/chessic.h"
#include "ctype.h"

char LocToChar(int col, int type)
{
    static const char c[] = { '.', 'p', 'n', 'b', 'r', 'q', 'k' };
    return col == CSC_WHITE ? toupper(c[type]) : c[type];
}

