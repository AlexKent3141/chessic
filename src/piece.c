#include "../include/chessic.h"

piece create_piece(char col, PIECE_TYPE type)
{
    return col + (type << 1);
}

char get_piece_colour(piece p)
{
    return p & 0x1;
}

PIECE_TYPE get_piece_type(piece p)
{
    return p >> 1;
}

void set_piece_type(piece* p, PIECE_TYPE type)
{
    *p = (*p & 0x1) + (type << 1);
}

