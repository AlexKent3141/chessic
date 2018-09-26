#include "board.h"
#include "utils.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

board* create_board_empty()
{
    const int NUM_SQUARES = 128;

    board* b = malloc(sizeof(board));
    b->squares = malloc(NUM_SQUARES*sizeof(char));
    memset(b->squares, 0, NUM_SQUARES*sizeof(char));

    state* bs = malloc(sizeof(state));
    memset(bs, 0, sizeof(state));
    bs->ep = BAD_LOC;
    bs->previous = NULL;
    b->bs = bs;

    return b;
}

// Gets the 0x88 for this file and rank.
int file_rank_to_board(int file, int rank)
{
    return (rank << 4) + file;
}

// Gets the 0x88 for the specified 0-63 coordinate.
int coord_to_board(int coord)
{
    return file_rank_to_board(coord % 8, coord / 8);
}

// Gets the 0-63 coordinate for the specified 0x88.
int board_to_coord(int loc)
{
    return 8*(loc >> 4) + (loc & 0x7);
}

// The first nibble is the piece type and the second is the colour.
char create_square(int col, int piece)
{
    return (col << 4) + piece;
}

int piece_from_square(char s)
{
    return s & 0xF;
}

int col_from_square(char s)
{
    return s >> 4;
}

void free_board(board* b)
{
    if (b)
    {
        if (b->squares) free(b->squares);
        free(b);
    }
}

void print_board(board* b)
{
    int loc;
    for (int r = 7; r >= 0; r--)
    {
        for (int f = 0; f < 8; f++)
        {
            loc = file_rank_to_board(f, r);
            putchar(square_to_char(b->squares[loc]));
        }

        printf("\n");
    }
}
