#include "../include/chessic.h"
#include "board_state.h"

void AddMove(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Move move)
{
    /* Check whether the move is legal and add to the list if it is. */
    if (CSC_IsLegal(b, move))
    {
        CSC_AddMove(l, move);
    }
}

void AddPawnMoves(
    struct CSC_Board* b,
    CSC_Bitboard ends,
    struct CSC_MoveList* l,
    int d,
    enum CSC_MoveType type)
{
    int loc;
    while (ends)
    {
        loc = CSC_PopLSB(&ends);
        AddMove(b, l, CSC_CreateMove(loc-d, loc, CSC_NONE, type));
    }
}

void AddPromoMoves(
    struct CSC_Board* b,
    CSC_Bitboard ends,
    struct CSC_MoveList* l,
    int d)
{
    int loc;
    while (ends)
    {
        loc = CSC_PopLSB(&ends);
        AddMove(b, l, CSC_CreateMove(loc-d, loc, CSC_KNIGHT, CSC_PROMOTION));
        AddMove(b, l, CSC_CreateMove(loc-d, loc, CSC_BISHOP, CSC_PROMOTION));
        AddMove(b, l, CSC_CreateMove(loc-d, loc, CSC_ROOK, CSC_PROMOTION));
        AddMove(b, l, CSC_CreateMove(loc-d, loc, CSC_QUEEN, CSC_PROMOTION));
    }
}

void FindPawnMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    enum CSC_MoveGenType type)
{
    int p = b->player;
    CSC_Bitboard enemies = b->all[1-p];
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];
    CSC_Bitboard promo = p == CSC_WHITE ? CSC_Ranks[6] : CSC_Ranks[1];
    int forward = p == CSC_WHITE ? CSC_FILE_NB : -CSC_FILE_NB;
    int capLeft = CSC_FILE_NB-1;
    int capRight = CSC_FILE_NB+1;

    // Single and double pawn pushes (without promotions).
    if (type & CSC_QUIETS)
    {
        CSC_Bitboard pawns = b->pieces[CSC_PAWN][p] & ~promo;
        CSC_Bitboard f1 = p == CSC_WHITE
            ? pawns << CSC_FILE_NB
            : pawns >> CSC_FILE_NB;

        f1 &= ~all;

        CSC_Bitboard f2 = p == CSC_WHITE
            ? (f1 & CSC_Ranks[2]) << CSC_FILE_NB
            : (f1 & CSC_Ranks[5]) >> CSC_FILE_NB;

        f2 &= ~all;

        AddPawnMoves(b, f1, l, forward, CSC_NORMAL);
        AddPawnMoves(b, f2, l, 2*forward, CSC_TWOSPACE);
    }

    // Normal and en-passent captures (without promotions).
    if (type & CSC_CAPTURES)
    {
        CSC_Bitboard pawns = b->pieces[CSC_PAWN][p] & ~promo;

        // Ensure that no captures wrap around the struct CSC_Board.
        CSC_Bitboard leftCapPawns = pawns & ~CSC_Files[0];
        CSC_Bitboard rightCapPawns = pawns & ~CSC_Files[7];

        // Defining left and right from white's perspective...
        CSC_Bitboard leftCaps = p == CSC_WHITE
            ? leftCapPawns << capLeft
            : leftCapPawns >> capRight;

        CSC_Bitboard rightCaps = p == CSC_WHITE
            ? rightCapPawns << capRight
            : rightCapPawns >> capLeft;

        AddPawnMoves(
            b,
            leftCaps & enemies,
            l,
            p == CSC_WHITE ? capLeft : -capRight, CSC_NORMAL);

        AddPawnMoves(
            b,
            rightCaps & enemies,
            l,
            p == CSC_WHITE ? capRight : -capLeft, CSC_NORMAL);

        struct BoardState* state = Top((struct StateStack*)b->states);
        int epLoc = state->enPassentIndex;
        if (epLoc != CSC_BAD_LOC)
        {
            CSC_Bitboard ep = (CSC_Bitboard)1 << epLoc;

            // Need to shift "backwards" from the ep location.
            CSC_Bitboard caps = 0;
            if (!(ep & CSC_Files[0]))
            {
                caps |= p == CSC_WHITE ? ep >> capRight : ep << capLeft;
            }

            if (!(ep & CSC_Files[7]))
            {
                caps |= p == CSC_WHITE ? ep >> capLeft : ep << capRight;
            }

            caps &= pawns;

            while (caps)
            {
                AddMove(b, l, CSC_CreateMove(CSC_PopLSB(&caps), epLoc, CSC_NONE, CSC_ENPASSENT));
            }
        }
    }

    // Promotions.
    CSC_Bitboard pawns = b->pieces[CSC_PAWN][p] & promo;
    if (pawns)
    {
        CSC_Bitboard f1 = p == CSC_WHITE ? pawns << CSC_FILE_NB : pawns >> CSC_FILE_NB;
        f1 &= ~all;

        AddPromoMoves(b, f1, l, forward);

        if (type & CSC_CAPTURES)
        {
            // Ensure that no captures wrap around the struct CSC_Board.
            CSC_Bitboard leftCapPawns = pawns & ~CSC_Files[0];
            CSC_Bitboard rightCapPawns = pawns & ~CSC_Files[7];

            // Defining left and right from white's perspective...
            CSC_Bitboard leftCaps = p == CSC_WHITE
                ? leftCapPawns << capLeft
                : leftCapPawns >> capRight;

            CSC_Bitboard rightCaps = p == CSC_WHITE
                ? rightCapPawns << capRight
                : rightCapPawns >> capLeft;

            AddPromoMoves(
                b,
                leftCaps & enemies,
                l,
                p == CSC_WHITE ? capLeft : -capRight);

            AddPromoMoves(
                b,
                rightCaps & enemies,
                l,
                p == CSC_WHITE ? capRight : -capLeft);
        }
    }
}

void AddMoves(
    struct CSC_Board* b,
    int loc,
    struct CSC_MoveList* l,
    CSC_Bitboard ends)
{
    while (ends)
    {
        AddMove(b, l, CSC_CreateMove(loc, CSC_PopLSB(&ends), CSC_NONE, CSC_NORMAL));
    }
}

void FindStepperMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Bitboard steppers,
    CSC_Bitboard targets,
    CSC_Bitboard* attacks)
{
    int loc;
    while (steppers)
    {
        loc = CSC_PopLSB(&steppers);
        AddMoves(b, loc, l, attacks[loc] & targets);
    }
}

void FindKnightMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Bitboard targets)
{
    FindStepperMoves(
        b,
        l,
        b->pieces[CSC_KNIGHT][b->player],
        targets,
        CSC_KnightAttacks);
}

void FindCastlingMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l)
{
    int p = b->player;
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];
    int startLoc = p == CSC_WHITE ? 4 : 60;
    struct BoardState* state = Top((struct StateStack*)b->states);
    if (state->castlingRights[p].kingSide)
    {
        if (!CSC_Test(all, startLoc+1)
         && !CSC_Test(all, startLoc+2)
         && !CSC_IsAttacked(b, startLoc)
         && !CSC_IsAttacked(b, startLoc+1))
        {
            AddMove(b, l, CSC_CreateMove(startLoc, startLoc+2, CSC_NONE, CSC_KINGCASTLE));
        }
    }

    if (state->castlingRights[p].queenSide)
    {
        if (!CSC_Test(all, startLoc-1)
         && !CSC_Test(all, startLoc-2)
         && !CSC_Test(all, startLoc-3)
         && !CSC_IsAttacked(b, startLoc)
         && !CSC_IsAttacked(b, startLoc-1))
        {
            AddMove(b, l, CSC_CreateMove(startLoc, startLoc-2, CSC_NONE, CSC_QUEENCASTLE));
        }
    }
}

void FindKingMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Bitboard targets)
{
    FindStepperMoves(b, l, b->pieces[CSC_KING][b->player], targets, CSC_KingAttacks);
    FindCastlingMoves(b, l);
}

void FindOrthMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Bitboard pieces,
    CSC_Bitboard targets,
    CSC_Bitboard (*rays)[8])
{
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];

    int p;
    CSC_Bitboard blockers, ray;
    while (pieces)
    {
        p = CSC_PopLSB(&pieces);

        ray = rays[p][CSC_NORTH];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_LSB(blockers)][CSC_NORTH];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_EAST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_LSB(blockers)][CSC_EAST];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_SOUTH];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_MSB(blockers)][CSC_SOUTH];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_WEST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_MSB(blockers)][CSC_WEST];
        AddMoves(b, p, l, ray & targets);
    }
}

void FindDiagMoves(
    struct CSC_Board* b,
    struct CSC_MoveList* l,
    CSC_Bitboard pieces,
    CSC_Bitboard targets,
    CSC_Bitboard (*rays)[8])
{
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];

    int p;
    CSC_Bitboard blockers, ray;
    while (pieces)
    {
        p = CSC_PopLSB(&pieces);

        ray = rays[p][CSC_NORTHWEST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_LSB(blockers)][CSC_NORTHWEST];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_NORTHEAST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_LSB(blockers)][CSC_NORTHEAST];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_SOUTHWEST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_MSB(blockers)][CSC_SOUTHWEST];
        AddMoves(b, p, l, ray & targets);

        ray = rays[p][CSC_SOUTHEAST];
        blockers = ray & all;
        if (blockers) ray ^= rays[CSC_MSB(blockers)][CSC_SOUTHEAST];
        AddMoves(b, p, l, ray & targets);
    }
}

struct CSC_MoveList* CSC_GetMoves(
    struct CSC_Board* b,
    enum CSC_MoveGenType type)
{
    struct CSC_MoveList* l = CSC_MakeMoveList();

    if (CSC_IsDrawn(b)) return l;

    FindPawnMoves(b, l, type);

    CSC_Bitboard targets = 0;
    if (type & CSC_QUIETS) targets |= ~(b->all[CSC_WHITE] | b->all[CSC_BLACK]);
    if (type & CSC_CAPTURES) targets |= b->all[1-b->player];

    FindKnightMoves(b, l, targets);
    FindKingMoves(b, l, targets);

    CSC_Bitboard orth = b->pieces[CSC_ROOK][b->player] | b->pieces[CSC_QUEEN][b->player];
    FindOrthMoves(b, l, orth, targets, CSC_RayAttacks);

    CSC_Bitboard diag = b->pieces[CSC_BISHOP][b->player] | b->pieces[CSC_QUEEN][b->player];
    FindDiagMoves(b, l, diag, targets, CSC_RayAttacks);

    return l;
}
