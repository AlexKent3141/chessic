#include "movegen.h"
#include "bits.h"

move_list* get_moves(board* b, MOVE_TYPE type)
{
    move_list* l = make_move_list();
    find_pawn_moves(b, l, type);

    bb targets = 0;
    if (type & QUIETS) targets |= ~(b->all[WHITE] | b->all[BLACK]);
    if (type & CAPTURES) targets |= b->all[1-b->player];

    find_knight_moves(b, l, targets);
    find_king_moves(b, l, targets);

    bb orth = b->pieces[ROOK][b->player] | b->pieces[QUEEN][b->player];
    find_orth_moves(b, l, type, orth, targets, RAY_ATTACKS);

    bb diag = b->pieces[BISHOP][b->player] | b->pieces[QUEEN][b->player];
    find_diag_moves(b, l, type, diag, targets, RAY_ATTACKS);

    return l;
}

void find_pawn_moves(board* b, move_list* l, MOVE_TYPE type)
{
    int p = b->player;
    bb enemies = b->all[1-p];
    bb all = b->all[WHITE] | b->all[BLACK];
    bb promo = p == WHITE ? RANKS[6] : RANKS[1];
    int forward = p == WHITE ? FILE_NB : -FILE_NB;
    int cap_left = FILE_NB-1;
    int cap_right = FILE_NB+1;

    // Single and double pawn pushes (without promotions).
    if (type & QUIETS)
    {
        bb pawns = b->pieces[PAWN][p] & ~promo;
        bb f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
        f1 &= ~all;

        bb f2 = p == WHITE ? (f1 & RANKS[2]) << FILE_NB : (f1 & RANKS[5]) >> FILE_NB;
        f2 &= ~all;

        add_pawn_moves(f1, l, forward, NORMAL);
        add_pawn_moves(f2, l, 2*forward, TWOSPACE);
    }

    // Normal and en-passent captures (without promotions).
    if (type & CAPTURES)
    {
        bb pawns = b->pieces[PAWN][p] & ~promo;

        // Ensure that no captures wrap around the board.
        bb left_cap_pawns = pawns & ~FILES[0];
        bb right_cap_pawns = pawns & ~FILES[7];

        // Defining left and right from white's perspective...
        bb left_caps = p == WHITE ? left_cap_pawns << cap_left : left_cap_pawns >> cap_right;
        bb right_caps = p == WHITE ? right_cap_pawns << cap_right : right_cap_pawns >> cap_left;

        add_pawn_moves(left_caps & enemies, l, p == WHITE ? cap_left : -cap_right, NORMAL);
        add_pawn_moves(right_caps & enemies, l, p == WHITE ? cap_right : -cap_left, NORMAL);

        int ep_loc = b->bs->ep;
        if (ep_loc != BAD_LOC)
        {
            bb ep = (bb)1 << ep_loc;

            // Need to shift "backwards" from the ep location.
            bb caps = 0;
            if (!(ep & FILES[0])) caps |= p == WHITE ? ep >> cap_right : ep << cap_left;
            if (!(ep & FILES[7])) caps |= p == WHITE ? ep >> cap_left : ep << cap_right;

            caps &= pawns;

            while (caps)
                add_move(l, create_move(pop_lsb(&caps), ep_loc, NONE, ENPASSENT));
        }
    }

    // Promotions.
    bb pawns = b->pieces[PAWN][p] & promo;
    if (pawns)
    {
        bb f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
        f1 &= ~all;

        add_promo_moves(f1, l, forward);

        if (type & CAPTURES)
        {
            // Ensure that no captures wrap around the board.
            bb left_cap_pawns = pawns & ~FILES[0];
            bb right_cap_pawns = pawns & ~FILES[7];

            // Defining left and right from white's perspective...
            bb left_caps = p == WHITE ? left_cap_pawns << cap_left : left_cap_pawns >> cap_right;
            bb right_caps = p == WHITE ? right_cap_pawns << cap_right : right_cap_pawns >> cap_left;

            add_promo_moves(left_caps & enemies, l, p == WHITE ? cap_left : -cap_right);
            add_promo_moves(right_caps & enemies, l, p == WHITE ? cap_right : -cap_left);
        }
    }
}

void find_knight_moves(board* b, move_list* l, bb targets)
{
    find_stepper_moves(b, l, b->pieces[KNIGHT][b->player], targets, KNIGHT_ATTACKS);
}

void find_king_moves(board* b, move_list* l, bb targets)
{
    find_stepper_moves(b, l, b->pieces[KING][b->player], targets, KING_ATTACKS);
    find_castling_moves(b, l);
}

void find_castling_moves(board* b, move_list* l)
{
    int p = b->player;
    bb all = b->all[WHITE] | b->all[BLACK];
    int start_loc = p == WHITE ? 4 : 60;
    if (b->bs->crs[p].ks)
    {
        if (!test(all, start_loc+1) && !test(all, start_loc+2)
         && !is_attacked(b, start_loc) && !is_attacked(b, start_loc+1))
        {
            add_move(l, create_move(start_loc, start_loc+2, NONE, KINGCASTLE));
        }
    }

    if (b->bs->crs[p].qs)
    {
        if (!test(all, start_loc-1) && !test(all, start_loc-2) && !test(all, start_loc-3)
         && !is_attacked(b, start_loc) && !is_attacked(b, start_loc-1))
        {
            add_move(l, create_move(start_loc, start_loc-2, NONE, QUEENCASTLE));
        }
    }
}

void find_orth_moves(board* b, move_list* l, MOVE_TYPE type, bb pieces, bb targets, bb (*rays)[8])
{
    bb all = b->all[WHITE] | b->all[BLACK];

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

void find_diag_moves(board* b, move_list* l, MOVE_TYPE type, bb pieces, bb targets, bb (*rays)[8])
{
    bb all = b->all[WHITE] | b->all[BLACK];

    int p;
    bb blockers, ray;
    while (pieces)
    {
        p = pop_lsb(&pieces);

        ray = rays[p][NW];
        blockers = ray & all;
        if (blockers) ray ^= rays[lsb(blockers)][NW];
        add_moves(p, l, ray & targets);

        ray = rays[p][NE];
        blockers = ray & all;
        if (blockers) ray ^= rays[lsb(blockers)][NE];
        add_moves(p, l, ray & targets);

        ray = rays[p][SW];
        blockers = ray & all;
        if (blockers) ray ^= rays[msb(blockers)][SW];
        add_moves(p, l, ray & targets);

        ray = rays[p][SE];
        blockers = ray & all;
        if (blockers) ray ^= rays[msb(blockers)][SE];
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
        add_move(l, create_move(loc, pop_lsb(&ends), NONE, NORMAL));
    }
}

void add_pawn_moves(bb ends, move_list* l, int d, MOVE_TYPE type)
{
    int loc;
    while (ends)
    {
        loc = pop_lsb(&ends);
        add_move(l, create_move(loc-d, loc, NONE, type));
    }
}

void add_promo_moves(bb ends, move_list* l, int d)
{
    int loc;
    while (ends)
    {
        loc = pop_lsb(&ends);
        add_move(l, create_move(loc-d, loc, KNIGHT, PROMOTION));
        add_move(l, create_move(loc-d, loc, BISHOP, PROMOTION));
        add_move(l, create_move(loc-d, loc, ROOK, PROMOTION));
        add_move(l, create_move(loc-d, loc, QUEEN, PROMOTION));
    }
}
