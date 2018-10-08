#include "board.h"
#include "utils.h"
#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

board* create_board_empty()
{
    board* b = malloc(sizeof(board));

    b->player = WHITE;
    b->turn = 0;

    state* bs = malloc(sizeof(state));
    memset(bs, 0, sizeof(state));
    bs->ep = BAD_LOC;
    bs->previous = NULL;
    b->bs = bs;

    memset(b->squares, 0, SQUARE_NB*sizeof(piece));

    memset(b->all, 0, 2*sizeof(bb));
    for (int i = 0; i <= KING; i++) memset(b->pieces[i], 0, 2*sizeof(bb));

    return b;
}

bool make_move(board* b, move m)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    int s = get_start(m);
    int e = get_end(m);
    piece ps = get_piece_type(b->squares[s]);
    piece pe = get_piece_type(b->squares[e]);

    // Clone the previous board state.
    state* next = malloc(sizeof(state));
    memcpy(next, b->bs, sizeof(state));

    return true;
}

void undo_move(board* b)
{
}

void free_board(board* b)
{
    if (b)
    {
        free(b);
        b = NULL;
    }
}

void loc_details(board* b, int loc, int* col, int* type)
{
    *col = get_piece_colour(b->squares[loc]);
    *type = get_piece_type(b->squares[loc]);
}

void print_board(board* b)
{
    int col, type;
    for (int r = 7; r >= 0; r--)
    {
        for (int f = 0; f < 8; f++)
        {
            loc_details(b, 8*r+f, &col, &type);
            putchar(loc_to_char(col, type));
        }

        putchar('\n');
    }
}
