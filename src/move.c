#include "chessic.h"
#include "stdio.h"
#include "stdlib.h"

struct CSC_MoveList* CSC_MakeMoveList()
{
    struct CSC_MoveList* l = malloc(sizeof(struct CSC_MoveList));
    l->moves = malloc(CSC_MAX_MOVES*sizeof(CSC_Move));
    l->n = 0;
    return l;
}

void CSC_AddMove(struct CSC_MoveList* l, CSC_Move m)
{
    l->moves[l->n++] = m;
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
