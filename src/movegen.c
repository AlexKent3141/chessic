#include "movegen.h"
#include "bits.h"

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

    bb diag = b->bishops[b->player] | b->queens[b->player];
    find_diag_moves(b, l, type, diag, targets, RAY_ATTACKS);

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
    int p = b->player;
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
    int start_loc = p == WHITE ? 4 : 60;
    if (b->bs->crs[p].ks)
    {
        if (!test(all, start_loc+1)    && !test(all, start_loc+2)
         && !is_attacked(b, start_loc) && !is_attacked(b, start_loc+1))
        {
            add_move(l, create_move(start_loc, start_loc+2, NONE));
        }
    }

    if (b->bs->crs[p].qs)
    {
        if (!test(all, start_loc-1) && !test(all, start_loc-2) && !test(all, start_loc-3)
         && !is_attacked(b, start_loc) && !is_attacked(b, start_loc-1))
        {
            add_move(l, create_move(start_loc, start_loc-2, NONE));
        }
    }
}

void find_orth_moves(board* b, move_list* l, MOVE_TYPE type, bb pieces, bb targets, bb (*rays)[8])
{
    bb all = b->pieces[WHITE] | b->pieces[BLACK];

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
    bb all = b->pieces[WHITE] | b->pieces[BLACK];

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

bool is_attacked(board* b, int loc)
{
    int p = b->player;
    int e = 1-p;

    // Check steppers.
    if (KING_ATTACKS[loc] & b->kings[e]) return true;
    if (KNIGHT_ATTACKS[loc] & b->knights[e]) return true;

    // Check orthogonal rays.
    bb targets = b->rooks[e] | b->queens[e];
    if (RAY_ATTACKS_ALL[loc][ORTH] & targets)
    {
        if (is_orth_attacked(b, loc, targets, RAY_ATTACKS)) return true;
    }

    // Check diagonal rays.
    targets = b->bishops[e] | b->queens[e];
    if (RAY_ATTACKS_ALL[loc][DIAG] & targets)
    {
        if (is_diag_attacked(b, loc, targets, RAY_ATTACKS)) return true;
    }

    // Check pawns.
    bb l = (bb)1 << loc;
    bb pawns = b->pawns[e];
    bb attackers = p == WHITE ? l << 7 : l >> 7;
    attackers |= (p == WHITE ? l << 9 : l >> 9);
    if (attackers & pawns) return true;

    return false;
}

bool is_orth_attacked(board* b, int loc, bb targets, bb (*rays)[8])
{
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
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
    bb all = b->pieces[WHITE] | b->pieces[BLACK];
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

