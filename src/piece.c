#include "../include/chessic.h"

CSC_Piece CSC_CreatePiece(char col, enum CSC_PieceType type)
{
    return col + (type << 1);
}

char CSC_GetPieceColour(CSC_Piece p)
{
    return p & 0x1;
}

enum CSC_PieceType CSC_GetPieceType(CSC_Piece p)
{
    return p >> 1;
}

void CSC_SetPieceType(CSC_Piece* p, enum CSC_PieceType type)
{
    *p = (*p & 0x1) + (type << 1);
}

