#include "../include/chessic.h"

Piece CreatePiece(char col, enum PieceType type)
{
    return col + (type << 1);
}

char GetPieceColour(Piece p)
{
    return p & 0x1;
}

enum PieceType GetPieceType(Piece p)
{
    return p >> 1;
}

void SetPieceType(Piece* p, enum PieceType type)
{
    *p = (*p & 0x1) + (type << 1);
}

