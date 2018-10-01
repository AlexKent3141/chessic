#include "../include/chessic.h"
#include "stdio.h"
#include "stdlib.h"

// Give 6 bits for the start and end positions.
move create_move(char start, char end, char piece)
{
    return start + (end << 6) + (piece << 12);
}

char get_start(move m)
{
    return m & 0x3F;
}

char get_end(move m)
{
    return (m & 0x0FC0) >> 6;
}

char get_promotion(move m)
{
    return (m & 0xF000) >> 12;
}

void print_move(move m)
{
    printf("%d %d\n", get_start(m), get_end(m));
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
