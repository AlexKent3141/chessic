#include "utils.h"
#include "../include/chessic.h"
#include "ctype.h"

char square_to_char(char sq)
{
    int p = sq & 0x0F;
    int col = sq & 0xF0;

    char c[] = { '.', 'p', 'n', 'b', 'r', 'q', 'k' };
    return col == WHITE ? toupper(c[p]) : c[p];
}
