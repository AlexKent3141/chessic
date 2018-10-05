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

move_list* get_moves(board* b, MOVE_TYPE type)
{
    move_list* l = make_move_list();
    find_pawn_moves(b, l, type);

    bb targets = 0;
    if (type & QUIETS) targets |= ~(b->pieces[WHITE] | b->pieces[BLACK]);
    if (type & CAPTURES) targets |= b->pieces[1-b->player];

    find_knight_moves(b, l, targets);
    find_king_moves(b, l, targets);

    bb orth = b->rooks[b->player] | b->queens[b->player];
    find_orth_moves(b, l, type, orth, targets, RAY_ATTACKS);

    return l;
}

void find_pawn_moves(board* b, move_list* l, MOVE_TYPE type)
{
    int p = b->player;
    bb enemies = b->pieces[1-p];
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
    bb promo = p == WHITE ? RANKS[6] : RANKS[1];
    int forward = p == WHITE ? FILE_NB : -FILE_NB;
    int cap1 = FILE_NB-1;
    int cap2 = FILE_NB+1;

    // Single and double pawn pushes (without promotions).
    if (type & QUIETS)
    {
        bb pawns = b->pawns[p] & ~promo;
        bb f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
        f1 &= ~all;

        bb f2 = p == WHITE ? (f1 & RANKS[2]) << FILE_NB : (f1 & RANKS[5]) >> FILE_NB;
        f2 &= ~all;

        add_pawn_moves(f1, l, forward);
        add_pawn_moves(f2, l, 2*forward);
    }

    // Normal and en-passent captures (without promotions).
    if (type & CAPTURES)
    {
        bb pawns = b->pawns[p] & ~promo;
        bb left_caps = p == WHITE ? pawns << cap1 : pawns >> cap1;
        bb right_caps = p == WHITE ? pawns << cap2 : pawns >> cap2;

        bb ep = b->bs->ep == BAD_LOC ? 0 : (bb)1 << b->bs->ep;
        left_caps &= (enemies | ep);
        right_caps &= (enemies | ep);

        add_pawn_moves(left_caps, l, cap1);
        add_pawn_moves(right_caps, l, cap2);
    }

    // Promotions.
    bb pawns = b->pawns[p] & promo;
    if (pawns)
    {
        bb f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
        f1 &= ~all;

        add_promo_moves(f1, l, forward);

        if (type & CAPTURES)
        {
            bb left_caps = p == WHITE ? pawns << cap1 : pawns >> cap1;
            bb right_caps = p == WHITE ? pawns << cap2 : pawns >> cap2;

            left_caps &= enemies;
            right_caps &= enemies;

            add_promo_moves(left_caps, l, cap1);
            add_promo_moves(right_caps, l, cap2);
        }
    }
}

void find_knight_moves(board* b, move_list* l, bb targets)
{
    find_stepper_moves(b, l, b->knights[b->player], targets, KNIGHT_ATTACKS);
}

void find_king_moves(board* b, move_list* l, bb targets)
{
    find_stepper_moves(b, l, b->kings[b->player], targets, KING_ATTACKS);
    find_castling_moves(b, l);
}

void find_castling_moves(board* b, move_list* l)
{
    // TODO
}

void find_orth_moves(board* b, move_list* l, MOVE_TYPE type, bb pieces, bb targets, bb (*rays)[8])
{
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
    bb enemies = b->pieces[1-b->player];

    int p;
    bb blockers, ray;
    while (pieces)
    {
        p = pop_lsb(&pieces);

        ray = rays[p][N];
        blockers = ray & all;
        if (blockers) ray ^= rays[lsb(blockers)][N];
        add_moves(p, l, ray & targets);

        ray = rays[p][E];
        blockers = ray & all;
        if (blockers) ray ^= rays[lsb(blockers)][E];
        add_moves(p, l, ray & targets);

        ray = rays[p][S];
        blockers = ray & all;
        if (blockers) ray ^= rays[msb(blockers)][S];
        add_moves(p, l, ray & targets);

        ray = rays[p][W];
        blockers = ray & all;
        if (blockers) ray ^= rays[msb(blockers)][W];
        add_moves(p, l, ray & targets);
    }
}

void find_stepper_moves(board* b, move_list* l, bb steppers, bb targets, bb* attacks)
{
    int loc;
    while (steppers)
    {
        loc = pop_lsb(&steppers);
        add_moves(loc, l, attacks[loc] & targets);
    }
}

void add_moves(int loc, move_list* l, bb ends)
{
    while (ends)
    {
        add_move(l, create_move(loc, pop_lsb(&ends), NONE));
    }
}

void add_pawn_moves(bb ends, move_list* l, int d)
{
    int loc;
    while (ends)
    {
        loc = pop_lsb(&ends);
        add_move(l, create_move(loc-d, loc, NONE));
    }
}

void add_promo_moves(bb ends, move_list* l, int d)
{
    int loc;
    while (ends)
    {
        loc = pop_lsb(&ends);
        add_move(l, create_move(loc-d, loc, KNIGHT));
        add_move(l, create_move(loc-d, loc, BISHOP));
        add_move(l, create_move(loc-d, loc, ROOK));
        add_move(l, create_move(loc-d, loc, QUEEN));
    }
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
