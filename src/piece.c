#include "../include/chessic.h"

piece create_piece(char col, char type)
{
    return col + (type << 1);
}

char get_piece_colour(piece p)
{
    return p & 0x1;
}

char get_piece_type(piece p)
{
    return p >> 1;
}
