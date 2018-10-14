#include "board.h"
#include "bits.h"
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

board* copy_board(board* b)
{
    board* copy = malloc(sizeof(board));
    memcpy(copy, b, sizeof(board));
    return copy;
}

bool board_equal(board* b1, board* b2)
{
    bool equal = true;
    for (int i = 0; i < SQUARE_NB; i++) equal &= (b1->squares[i] == b2->squares[i]);
    for (int p = PAWN; p <= KING; p++) equal &= (b1->pieces[p][WHITE] == b2->pieces[p][WHITE]);
    for (int p = PAWN; p <= KING; p++) equal &= (b1->pieces[p][BLACK] == b2->pieces[p][BLACK]);

    equal &= (b1->all[WHITE] == b2->all[WHITE]);
    equal &= (b1->all[BLACK] == b2->all[BLACK]);

    return equal;
}

bool make_move(board* b, move m)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    int p = b->player;
    int s = get_start(m);
    int e = get_end(m);

    piece sp = remove_piece(b, s);
    piece cap = b->squares[e];

    PIECE_TYPE pt = get_piece_type(sp);
    MOVE_TYPE mt = get_move_type(m);

    if (cap && mt != ENPASSENT) remove_piece(b, e);

    if (mt == ENPASSENT)
    {
        assert(b->bs->ep != BAD_LOC);

        int cap_loc = e + (p == WHITE ? -FILE_NB : FILE_NB);
        cap = b->squares[cap_loc];
        remove_piece(b, cap_loc);
    }
    else if (mt == PROMOTION)
    {
        set_piece_type(&sp, get_promotion(m));
    }
    else if (mt & CASTLE)
    {
        // Move the corresponding rook.
        int start_file = mt == KINGCASTLE ? 7 : 0;
        int end_file = mt == KINGCASTLE ? 5 : 3;
        int rank = p == WHITE ? 0 : 7;

        piece rook = b->squares[8*rank + start_file];

        assert(get_piece_type(rook) == ROOK);
        assert(get_piece_colour(rook) == p);

        remove_piece(b, 8*rank + start_file);
        add_piece(b, 8*rank + end_file, rook);
    }

    // Add the piece back now that the promotion has been applied.
    add_piece(b, e, sp);

    castling my_crs = b->bs->crs[p];
    if (pt == KING)
    {
        my_crs.ks = false;
        my_crs.qs = false;
    }
    else if (pt == ROOK)
    {
        int king_rook = p == WHITE ? 7 : 63;
        int queen_rook = p == WHITE ? 0 : 56;
        my_crs.ks &= (s != king_rook);
        my_crs.qs &= (s != queen_rook);
    }

    castling their_crs = b->bs->crs[1-p];
    if (cap && get_piece_type(cap) == ROOK)
    {
        int enemy_king_rook = p == WHITE ? 63 : 7;
        int enemy_queen_rook = p == WHITE ? 56 : 0;
        their_crs.ks &= (e != enemy_king_rook);
        their_crs.qs &= (e != enemy_queen_rook);
    }

    int ep = BAD_LOC;
    if (mt == TWOSPACE) ep = e + (p == WHITE ? -8 : 8);

    // Clone the previous board state.
    state* next = malloc(sizeof(state));
    memcpy(next, b->bs, sizeof(state));
    next->last_move = m;
    next->cap = cap;
    next->crs[p] = my_crs;
    next->crs[1-p] = their_crs;
    next->ep = ep;

    ++next->plies_50_move;
    if (cap || pt == PAWN) next->plies_50_move = 0;

    next->previous = (struct state*)b->bs;
    b->bs = next;

    // Ensure legality: the king cannot end in check.
    if (is_attacked(b, lsb(b->pieces[KING][p])))
    {
        b->player = 1 - p;
        undo_move(b);
        return false;
    }

    b->player = 1 - p;

    return true;
}

void undo_move(board* b)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    // Extract the required info from the state and revert to previous.
    state* old = b->bs;
    move m = old->last_move;
    int cap = old->cap;
    b->bs = (state*)old->previous;
    free(old);

    b->player = 1 - b->player;

    int p = b->player;
    int s = get_start(m);
    int e = get_end(m);

    // Move the pieces back.
    piece pc = b->squares[e];

    remove_piece(b, e);

    MOVE_TYPE mt = get_move_type(m);
    if (mt == PROMOTION) set_piece_type(&pc, PAWN);

    add_piece(b, s, pc);

    if (cap)
    {
        int cap_loc = e;
        if (mt == ENPASSENT) cap_loc = e + (p == WHITE ? -FILE_NB : FILE_NB);
        add_piece(b, cap_loc, cap);
    }

    if (mt & CASTLE)
    {
        // Move the corresponding rook back.
        int start_file = mt == KINGCASTLE ? 5 : 3;
        int end_file = mt == KINGCASTLE ? 7 : 0;
        int rank = p == WHITE ? 0 : 7;

        piece rook = b->squares[8*rank + start_file];

        assert(get_piece_type(rook) == ROOK);
        assert(get_piece_colour(rook) == p);

        remove_piece(b, 8*rank + start_file);
        add_piece(b, 8*rank + end_file, rook);
    }
}

piece remove_piece(board* b, int loc)
{
    piece pc = b->squares[loc];
    int p = get_piece_colour(pc);
    PIECE_TYPE pt = get_piece_type(pc);

    bb bit = (bb)1 << loc;
    b->all[p] ^= bit;
    b->pieces[pt][p] ^= bit;
    b->squares[loc] = 0;

    return pc;
}

void add_piece(board* b, int loc, piece pc)
{
    int p = get_piece_colour(pc);
    PIECE_TYPE pt = get_piece_type(pc);

    bb bit = (bb)1 << loc;
    b->all[p] |= bit;
    b->pieces[pt][p] |= bit;
    b->squares[loc] = pc;
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

bool is_attacked(board* b, int loc)
{
    int p = b->player;
    int e = 1-p;

    // Check steppers.
    if (KING_ATTACKS[loc] & b->pieces[KING][e]) return true;
    if (KNIGHT_ATTACKS[loc] & b->pieces[KNIGHT][e]) return true;

    // Check orthogonal rays.
    bb targets = b->pieces[ROOK][e] | b->pieces[QUEEN][e];
    if (RAY_ATTACKS_ALL[loc][ORTH] & targets)
    {
        if (is_orth_attacked(b, loc, targets, RAY_ATTACKS)) return true;
    }

    // Check diagonal rays.
    targets = b->pieces[BISHOP][e] | b->pieces[QUEEN][e];
    if (RAY_ATTACKS_ALL[loc][DIAG] & targets)
    {
        if (is_diag_attacked(b, loc, targets, RAY_ATTACKS)) return true;
    }

    // Check pawns.
    bb l = (bb)1 << loc;
    bb pawns = b->pieces[PAWN][e];

    bb attackers = 0;
    if (!(l & FILES[0])) attackers |= e == WHITE ? l >> 9 : l << 7;
    if (!(l & FILES[7])) attackers |= e == WHITE ? l >> 7 : l << 9;
    if (attackers & pawns) return true;

    return false;
}

bool is_orth_attacked(board* b, int loc, bb targets, bb (*rays)[8])
{
    bb all = b->all[WHITE] | b->all[BLACK];
    bb ray, attackers;

    ray = rays[loc][N];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[lsb(attackers)][N]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][E];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[lsb(attackers)][E]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][S];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[msb(attackers)][S]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][W];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[msb(attackers)][W]);
        if (!(ray & all)) return true;
    }

    return false;
}

bool is_diag_attacked(board* b, int loc, bb targets, bb (*rays)[8])
{
    bb all = b->all[WHITE] | b->all[BLACK];
    bb ray, attackers;

    ray = rays[loc][NE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[lsb(attackers)][NE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][NW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[lsb(attackers)][NW]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[msb(attackers)][SE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[msb(attackers)][SW]);
        if (!(ray & all)) return true;
    }

    return false;
}
