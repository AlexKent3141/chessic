#include "../include/chessic.h"
#include "stdio.h"
#include "stdlib.h"

/* The bit layout for a move is:
   6 bits for the start location
   6 bits for the end location
   3 bits for the promotion piece type
   6 bits for the move type */

Move CreateMove(char start, char end, enum PieceType promo, enum MoveType type)
{
    return start + (end << 6) + (promo << 12) + (type << 15);
}

char GetMoveStart(Move m)
{
    return m & 0x3F;
}

char GetMoveEnd(Move m)
{
    return (m >> 6) & 0x3F;
}

enum PieceType GetMovePromotion(Move m)
{
    return (m >> 12) & 0x7;
}

enum MoveType GetMoveType(Move m)
{
    return (m >> 15) & 0x3F;
}

void PrintMove(Move m)
{
    printf("Start: %d End: %d Promotion: %d Type: %d\n",
        GetMoveStart(m),
        GetMoveEnd(m),
        GetMovePromotion(m),
        GetMoveType(m));
}

struct MoveList* MakeMoveList()
{
    struct MoveList* l = malloc(sizeof(struct MoveList));
    l->moves = malloc(MAX_MOVES*sizeof(Move));
    l->end = &l->moves[0];
    l->n = 0;
    return l;
}

void AddMove(struct MoveList* l, Move m)
{
    *l->end = m;
    ++l->end;
    l->n++;
}

void FreeMoveList(struct MoveList* l)
{
    if (l)
    {
        if (l->moves) free(l->moves);
        free(l);
        l = NULL;
    }
}
