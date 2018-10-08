#include "../include/chessic.h"
#include "stdio.h"
#include "stdlib.h"

// The bit layout for a move is:
// 6 bits for the start location
// 6 bits for the end location
// 3 bits for the promotion piece type
// 6 bits for the move type

move create_move(char start, char end, char promo, char type)
{
    return start + (end << 6) + (promo << 12) + (type << 15);
}

char get_start(move m)
{
    return m & 0x3F;
}

char get_end(move m)
{
    return (m >> 6) & 0x3F;
}

char get_promotion(move m)
{
    return (m >> 12) & 0x7;
}

char get_move_type(move m)
{
    return (m >> 15) & 0x3F;
}

void print_move(move m)
{
    printf("Start: %d End: %d Promotion: %d Type: %d\n",
        get_start(m), get_end(m), get_promotion(m), get_move_type(m));
}

move_list* make_move_list()
{
    move_list* l = malloc(sizeof(move_list));
    l->moves = malloc(MAX_MOVES*sizeof(move));
    l->end = &l->moves[0];
    l->n = 0;
    return l;
}

void add_move(move_list* l, move m)
{
    *l->end = m;
    ++l->end;
    l->n++;
}

void free_move_list(move_list* l)
{
    if (l)
    {
        if (l->moves) free(l->moves);
        free(l);
        l = NULL;
    }
}
