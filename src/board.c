#include "board.h"
#include "bits.h"
#include "utils.h"
#include "ctype.h"
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

    memset(b->pieces, 0, 2*sizeof(bb));
    memset(b->pawns, 0, 2*sizeof(bb));
    memset(b->knights, 0, 2*sizeof(bb));
    memset(b->bishops, 0, 2*sizeof(bb));
    memset(b->rooks, 0, 2*sizeof(bb));
    memset(b->queens, 0, 2*sizeof(bb));
    memset(b->kings, 0, 2*sizeof(bb));

    return b;
}

void free_board(board* b)
{
    if (b)
    {
        free(b);
        b = NULL;
    }
}

move_list* get_moves(board* b)
{
    move_list* l = make_move_list();
    add_pawn_moves(b, l);

    return l;
}

void add_pawn_moves(board* b, move_list* l)
{
    int p = b->player;
    bb pawns = b->pawns[p];
    bb enemy = b->pieces[1-p];
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
    int forward = p == WHITE ? FILE_NB : -FILE_NB;
    int loc;

    // Get all one-space forward moves.
    bb f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
    f1 &= ~all;

    // Two-space moves only apply to pawns on their starting rank.
    bb f2 = p == WHITE ? (f1 & RANKS[2]) << FILE_NB : (f1 & RANKS[5]) >> FILE_NB;
    f2 &= ~all;

    // Now associate these with the pawns which can make the moves.
    while (f1)
    {
        loc = pop_lsb(&f1);
        add_move(l, create_move(loc-forward, loc));
    }

    while (f2)
    {
        loc = pop_lsb(&f2);
        add_move(l, create_move(loc-2*forward, loc));
    }

    return NULL;
}

void loc_details(board* b, int loc, int* col, int* type)
{
    bb bloc = (bb)1 << loc;
    if (b->pieces[WHITE] & bloc) *col = WHITE;
    else if (b->pieces[BLACK] & bloc) *col = BLACK;
    else
    {
        *type = NONE;
        return;
    }

    if (b->pawns[*col] & bloc) *type = PAWN;
    else if (b->knights[*col] & bloc) *type = KNIGHT;
    else if (b->bishops[*col] & bloc) *type = BISHOP;
    else if (b->rooks[*col] & bloc) *type = ROOK;
    else if (b->queens[*col] & bloc) *type = QUEEN;
    else if (b->kings[*col] & bloc) *type = KING;
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
