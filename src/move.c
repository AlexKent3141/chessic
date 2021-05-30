#include "chessic.h"
#include "stdio.h"
#include "stdlib.h"

/* The bit layout for a move is:
   6 bits for the start location
   6 bits for the end location
   3 bits for the promotion piece type
   6 bits for the move type */

CSC_Move CSC_CreateMove(
    char start,
    char end,
    enum CSC_PieceType promo,
    enum CSC_MoveType type)
{
    return start + (end << 6) + (promo << 12) + (type << 15);
}

char CSC_GetMoveStart(CSC_Move m)
{
    return m & 0x3F;
}

char CSC_GetMoveEnd(CSC_Move m)
{
    return (m >> 6) & 0x3F;
}

enum CSC_PieceType CSC_GetMovePromotion(CSC_Move m)
{
    return (m >> 12) & 0x7;
}

enum CSC_MoveType CSC_GetMoveType(CSC_Move m)
{
    return (m >> 15) & 0x3F;
}

struct CSC_MoveList* CSC_MakeMoveList()
{
    struct CSC_MoveList* l = malloc(sizeof(struct CSC_MoveList));
    l->moves = malloc(CSC_MAX_MOVES*sizeof(CSC_Move));
    l->end = &l->moves[0];
    l->n = 0;
    return l;
}

void CSC_AddMove(struct CSC_MoveList* l, CSC_Move m)
{
    *l->end = m;
    ++l->end;
    l->n++;
}

void CSC_FreeMoveList(struct CSC_MoveList* l)
{
    if (l)
    {
        if (l->moves) free(l->moves);
        free(l);
        l = NULL;
    }
}
