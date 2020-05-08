#include "movegen.h"
#include "bits.h"

struct MoveList* GetMoves(
    struct Board* b,
    enum MoveType type)
{
    struct MoveList* l = MakeMoveList();
    FindPawnMoves(b, l, type);

    BB targets = 0;
    if (type & QUIETS) targets |= ~(b->all[WHITE] | b->all[BLACK]);
    if (type & CAPTURES) targets |= b->all[1-b->player];

    FindKnightMoves(b, l, targets);
    FindKingMoves(b, l, targets);

    BB orth = b->pieces[ROOK][b->player] | b->pieces[QUEEN][b->player];
    FindOrthMoves(b, l, orth, targets, RayAttacks);

    BB diag = b->pieces[BISHOP][b->player] | b->pieces[QUEEN][b->player];
    FindDiagMoves(b, l, diag, targets, RayAttacks);

    return l;
}

void FindPawnMoves(
    struct Board* b,
    struct MoveList* l,
    enum MoveType type)
{
    int p = b->player;
    BB enemies = b->all[1-p];
    BB all = b->all[WHITE] | b->all[BLACK];
    BB promo = p == WHITE ? Ranks[6] : Ranks[1];
    int forward = p == WHITE ? FILE_NB : -FILE_NB;
    int capLeft = FILE_NB-1;
    int capRight = FILE_NB+1;

    // Single and double pawn pushes (without promotions).
    if (type & QUIETS)
    {
        BB pawns = b->pieces[PAWN][p] & ~promo;
        BB f1 = p == WHITE
            ? pawns << FILE_NB
            : pawns >> FILE_NB;

        f1 &= ~all;

        BB f2 = p == WHITE
            ? (f1 & Ranks[2]) << FILE_NB
            : (f1 & Ranks[5]) >> FILE_NB;

        f2 &= ~all;

        AddPawnMoves(f1, l, forward, NORMAL);
        AddPawnMoves(f2, l, 2*forward, TWOSPACE);
    }

    // Normal and en-passent captures (without promotions).
    if (type & CAPTURES)
    {
        BB pawns = b->pieces[PAWN][p] & ~promo;

        // Ensure that no captures wrap around the struct Board.
        BB leftCapPawns = pawns & ~Files[0];
        BB rightCapPawns = pawns & ~Files[7];

        // Defining left and right from white's perspective...
        BB leftCaps = p == WHITE
            ? leftCapPawns << capLeft
            : leftCapPawns >> capRight;

        BB rightCaps = p == WHITE
            ? rightCapPawns << capRight
            : rightCapPawns >> capLeft;

        AddPawnMoves(
            leftCaps & enemies,
            l,
            p == WHITE ? capLeft : -capRight, NORMAL);

        AddPawnMoves(
            rightCaps & enemies,
            l,
            p == WHITE ? capRight : -capLeft, NORMAL);

        int epLoc = b->bs->enPassentIndex;
        if (epLoc != BAD_LOC)
        {
            BB ep = (BB)1 << epLoc;

            // Need to shift "backwards" from the ep location.
            BB caps = 0;
            if (!(ep & Files[0]))
            {
                caps |= p == WHITE ? ep >> capRight : ep << capLeft;
            }

            if (!(ep & Files[7]))
            {
                caps |= p == WHITE ? ep >> capLeft : ep << capRight;
            }

            caps &= pawns;

            while (caps)
            {
                AddMove(l, CreateMove(PopLSB(&caps), epLoc, NONE, ENPASSENT));
            }
        }
    }

    // Promotions.
    BB pawns = b->pieces[PAWN][p] & promo;
    if (pawns)
    {
        BB f1 = p == WHITE ? pawns << FILE_NB : pawns >> FILE_NB;
        f1 &= ~all;

        AddPromoMoves(f1, l, forward);

        if (type & CAPTURES)
        {
            // Ensure that no captures wrap around the struct Board.
            BB leftCapPawns = pawns & ~Files[0];
            BB rightCapPawns = pawns & ~Files[7];

            // Defining left and right from white's perspective...
            BB leftCaps = p == WHITE
                ? leftCapPawns << capLeft
                : leftCapPawns >> capRight;

            BB rightCaps = p == WHITE
                ? rightCapPawns << capRight
                : rightCapPawns >> capLeft;

            AddPromoMoves(
                leftCaps & enemies,
                l,
                p == WHITE ? capLeft : -capRight);

            AddPromoMoves(
                rightCaps & enemies,
                l,
                p == WHITE ? capRight : -capLeft);
        }
    }
}

void FindKnightMoves(
    struct Board* b,
    struct MoveList* l,
    BB targets)
{
    FindStepperMoves(l, b->pieces[KNIGHT][b->player], targets, KnightAttacks);
}

void FindKingMoves(
    struct Board* b,
    struct MoveList* l,
    BB targets)
{
    FindStepperMoves(l, b->pieces[KING][b->player], targets, KingAttacks);
    FindCastlingMoves(b, l);
}

void FindCastlingMoves(
    struct Board* b,
    struct MoveList* l)
{
    int p = b->player;
    BB all = b->all[WHITE] | b->all[BLACK];
    int startLoc = p == WHITE ? 4 : 60;
    if (b->bs->castlingRights[p].kingSide)
    {
        if (!Test(all, startLoc+1)
         && !Test(all, startLoc+2)
         && !IsAttacked(b, startLoc)
         && !IsAttacked(b, startLoc+1))
        {
            AddMove(l, CreateMove(startLoc, startLoc+2, NONE, KINGCASTLE));
        }
    }

    if (b->bs->castlingRights[p].queenSide)
    {
        if (!Test(all, startLoc-1)
         && !Test(all, startLoc-2)
         && !Test(all, startLoc-3)
         && !IsAttacked(b, startLoc)
         && !IsAttacked(b, startLoc-1))
        {
            AddMove(l, CreateMove(startLoc, startLoc-2, NONE, QUEENCASTLE));
        }
    }
}

void FindOrthMoves(
    struct Board* b,
    struct MoveList* l,
    BB pieces,
    BB targets,
    BB (*rays)[8])
{
    BB all = b->all[WHITE] | b->all[BLACK];

    int p;
    BB blockers, ray;
    while (pieces)
    {
        p = PopLSB(&pieces);

        ray = rays[p][N];
        blockers = ray & all;
        if (blockers) ray ^= rays[LSB(blockers)][N];
        AddMoves(p, l, ray & targets);

        ray = rays[p][E];
        blockers = ray & all;
        if (blockers) ray ^= rays[LSB(blockers)][E];
        AddMoves(p, l, ray & targets);

        ray = rays[p][S];
        blockers = ray & all;
        if (blockers) ray ^= rays[MSB(blockers)][S];
        AddMoves(p, l, ray & targets);

        ray = rays[p][W];
        blockers = ray & all;
        if (blockers) ray ^= rays[MSB(blockers)][W];
        AddMoves(p, l, ray & targets);
    }
}

void FindDiagMoves(
    struct Board* b,
    struct MoveList* l,
    BB pieces,
    BB targets,
    BB (*rays)[8])
{
    BB all = b->all[WHITE] | b->all[BLACK];

    int p;
    BB blockers, ray;
    while (pieces)
    {
        p = PopLSB(&pieces);

        ray = rays[p][NW];
        blockers = ray & all;
        if (blockers) ray ^= rays[LSB(blockers)][NW];
        AddMoves(p, l, ray & targets);

        ray = rays[p][NE];
        blockers = ray & all;
        if (blockers) ray ^= rays[LSB(blockers)][NE];
        AddMoves(p, l, ray & targets);

        ray = rays[p][SW];
        blockers = ray & all;
        if (blockers) ray ^= rays[MSB(blockers)][SW];
        AddMoves(p, l, ray & targets);

        ray = rays[p][SE];
        blockers = ray & all;
        if (blockers) ray ^= rays[MSB(blockers)][SE];
        AddMoves(p, l, ray & targets);
    }
}

void FindStepperMoves(
    struct MoveList* l,
    BB steppers,
    BB targets,
    BB* attacks)
{
    int loc;
    while (steppers)
    {
        loc = PopLSB(&steppers);
        AddMoves(loc, l, attacks[loc] & targets);
    }
}

void AddMoves(
    int loc,
    struct MoveList* l,
    BB ends)
{
    while (ends)
    {
        AddMove(l, CreateMove(loc, PopLSB(&ends), NONE, NORMAL));
    }
}

void AddPawnMoves(
    BB ends,
    struct MoveList* l,
    int d,
    enum MoveType type)
{
    int loc;
    while (ends)
    {
        loc = PopLSB(&ends);
        AddMove(l, CreateMove(loc-d, loc, NONE, type));
    }
}

void AddPromoMoves(
    BB ends,
    struct MoveList* l,
    int d)
{
    int loc;
    while (ends)
    {
        loc = PopLSB(&ends);
        AddMove(l, CreateMove(loc-d, loc, KNIGHT, PROMOTION));
        AddMove(l, CreateMove(loc-d, loc, BISHOP, PROMOTION));
        AddMove(l, CreateMove(loc-d, loc, ROOK, PROMOTION));
        AddMove(l, CreateMove(loc-d, loc, QUEEN, PROMOTION));
    }
}
