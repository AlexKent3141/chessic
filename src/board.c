#include "board.h"
#include "bits.h"
#include "utils.h"
#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct Board* CreateBoardEmpty()
{
    struct Board* b = malloc(sizeof(struct Board));

    b->player = WHITE;
    b->turnNumber = 0;

    struct BoardState* bs = malloc(sizeof(struct BoardState));
    memset(bs, 0, sizeof(struct BoardState));
    bs->enPassentIndex = BAD_LOC;
    bs->previousState = NULL;
    b->bs = bs;

    memset(b->squares, 0, SQUARE_NB*sizeof(Piece));

    memset(b->all, 0, 2*sizeof(BB));
    for (int i = 0; i <= KING; i++) memset(b->pieces[i], 0, 2*sizeof(BB));

    return b;
}

struct Board* CopyBoard(struct Board* b)
{
    struct Board* copy = malloc(sizeof(struct Board));
    memcpy(copy, b, sizeof(struct Board));

    /* Perform a deep copy of the board state. */
    copy->bs = malloc(sizeof(struct BoardState));
    memcpy(copy->bs, b->bs, sizeof(struct BoardState));

    return copy;
}

bool BoardEqual(struct Board* b1, struct Board* b2)
{
    bool equal = true;
    for (int i = 0; i < SQUARE_NB; i++)
        equal &= (b1->squares[i] == b2->squares[i]);
    for (int p = PAWN; p <= KING; p++)
        equal &= (b1->pieces[p][WHITE] == b2->pieces[p][WHITE]);
    for (int p = PAWN; p <= KING; p++)
        equal &= (b1->pieces[p][BLACK] == b2->pieces[p][BLACK]);

    equal &= (b1->all[WHITE] == b2->all[WHITE]);
    equal &= (b1->all[BLACK] == b2->all[BLACK]);

    return equal;
}

bool MakeMove(struct Board* b, Move m)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    int p = b->player;
    int s = GetMoveStart(m);
    int e = GetMoveEnd(m);

    Piece sp = RemovePiece(b, s);
    Piece cap = b->squares[e];

    enum PieceType pt = GetPieceType(sp);
    enum MoveType mt = GetMoveType(m);

    if (cap && mt != ENPASSENT) RemovePiece(b, e);

    if (mt == ENPASSENT)
    {
        assert(b->bs->enPassentIndex != BAD_LOC);

        int capLoc = e + (p == WHITE ? -FILE_NB : FILE_NB);
        cap = b->squares[capLoc];
        RemovePiece(b, capLoc);
    }
    else if (mt == PROMOTION)
    {
        SetPieceType(&sp, GetMovePromotion(m));
    }
    else if (mt & CASTLE)
    {
        // Move the corresponding rook.
        int startFile = mt == KINGCASTLE ? 7 : 0;
        int end_file = mt == KINGCASTLE ? 5 : 3;
        int rank = p == WHITE ? 0 : 7;

        Piece rook = b->squares[8*rank + startFile];

        assert(GetPieceType(rook) == ROOK);
        assert(GetPieceColour(rook) == p);

        RemovePiece(b, 8*rank + startFile);
        AddPiece(b, 8*rank + end_file, rook);
    }

    // Add the piece back now that the promotion has been applied.
    AddPiece(b, e, sp);

    struct CastlingRights ourCastlingRights = b->bs->castlingRights[p];
    if (pt == KING)
    {
        ourCastlingRights.kingSide = false;
        ourCastlingRights.queenSide = false;
    }
    else if (pt == ROOK)
    {
        int kingRook = p == WHITE ? 7 : 63;
        int queenRook = p == WHITE ? 0 : 56;
        ourCastlingRights.kingSide &= (s != kingRook);
        ourCastlingRights.queenSide &= (s != queenRook);
    }

    struct CastlingRights theirCastlingRights = b->bs->castlingRights[1-p];
    if (cap && GetPieceType(cap) == ROOK)
    {
        int enemyKingRook = p == WHITE ? 63 : 7;
        int enemyQueenRook = p == WHITE ? 56 : 0;
        theirCastlingRights.kingSide &= (e != enemyKingRook);
        theirCastlingRights.queenSide &= (e != enemyQueenRook);
    }

    int ep = BAD_LOC;
    if (mt == TWOSPACE) ep = e + (p == WHITE ? -8 : 8);

    // Clone the previous board state.
    struct BoardState* next = malloc(sizeof(struct BoardState));
    memcpy(next, b->bs, sizeof(struct BoardState));
    next->lastMove = m;
    next->captureOnLastMove = cap;
    next->castlingRights[p] = ourCastlingRights;
    next->castlingRights[1-p] = theirCastlingRights;
    next->enPassentIndex = ep;

    ++next->plies50Move;
    if (cap || pt == PAWN) next->plies50Move = 0;

    next->previousState = b->bs;
    b->bs = next;

    // Ensure legality: the king cannot end in check.
    if (IsAttacked(b, LSB(b->pieces[KING][p])))
    {
        b->player = 1 - p;
        UndoMove(b);
        return false;
    }

    b->player = 1 - p;

    return true;
}

void UndoMove(struct Board* b)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    // Extract the required info from the state and revert to previous.
    struct BoardState* old = b->bs;
    Move m = old->lastMove;
    int cap = old->captureOnLastMove;
    b->bs = old->previousState;
    free(old);

    b->player = 1 - b->player;

    int p = b->player;
    int s = GetMoveStart(m);
    int e = GetMoveEnd(m);

    // Move the pieces back.
    Piece pc = b->squares[e];

    RemovePiece(b, e);

    enum MoveType mt = GetMoveType(m);
    if (mt == PROMOTION) SetPieceType(&pc, PAWN);

    AddPiece(b, s, pc);

    if (cap)
    {
        int capLoc = e;
        if (mt == ENPASSENT) capLoc = e + (p == WHITE ? -FILE_NB : FILE_NB);
        AddPiece(b, capLoc, cap);
    }

    if (mt & CASTLE)
    {
        // Move the corresponding rook back.
        int startFile = mt == KINGCASTLE ? 5 : 3;
        int endFile = mt == KINGCASTLE ? 7 : 0;
        int rank = p == WHITE ? 0 : 7;

        Piece rook = b->squares[8*rank + startFile];

        assert(GetPieceType(rook) == ROOK);
        assert(GetPieceColour(rook) == p);

        RemovePiece(b, 8*rank + startFile);
        AddPiece(b, 8*rank + endFile, rook);
    }
}

Piece RemovePiece(struct Board* b, int loc)
{
    Piece pc = b->squares[loc];
    int p = GetPieceColour(pc);
    enum PieceType pt = GetPieceType(pc);

    BB bit = (BB)1 << loc;
    b->all[p] ^= bit;
    b->pieces[pt][p] ^= bit;
    b->squares[loc] = 0;

    return pc;
}

void AddPiece(struct Board* b, int loc, Piece pc)
{
    int p = GetPieceColour(pc);
    enum PieceType pt = GetPieceType(pc);

    BB bit = (BB)1 << loc;
    b->all[p] |= bit;
    b->pieces[pt][p] |= bit;
    b->squares[loc] = pc;
}

void FreeBoard(struct Board* b)
{
    if (b)
    {
        if (b->bs)
        {
            free(b->bs);
            b->bs = NULL;
        }

        free(b);
        b = NULL;
    }
}

void LocDetails(struct Board* b, int loc, int* col, int* type)
{
    *col = GetPieceColour(b->squares[loc]);
    *type = GetPieceType(b->squares[loc]);
}

void PrintBoard(struct Board* b)
{
    int col, type;
    for (int r = 7; r >= 0; r--)
    {
        for (int f = 0; f < 8; f++)
        {
            LocDetails(b, 8*r+f, &col, &type);
            putchar(LocToChar(col, type));
        }

        putchar('\n');
    }
}

bool IsAttacked(struct Board* b, int loc)
{
    int p = b->player;
    int e = 1-p;

    // Check steppers.
    if (KingAttacks[loc] & b->pieces[KING][e]) return true;
    if (KnightAttacks[loc] & b->pieces[KNIGHT][e]) return true;

    // Check orthogonal rays.
    BB targets = b->pieces[ROOK][e] | b->pieces[QUEEN][e];
    if (RayAttacksAll[loc][ORTH] & targets)
    {
        if (IsOrthAttacked(b, loc, targets, RayAttacks)) return true;
    }

    // Check diagonal rays.
    targets = b->pieces[BISHOP][e] | b->pieces[QUEEN][e];
    if (RayAttacksAll[loc][DIAG] & targets)
    {
        if (IsDiagAttacked(b, loc, targets, RayAttacks)) return true;
    }

    // Check pawns.
    BB bit = (BB)1 << loc;
    BB pawns = b->pieces[PAWN][e];

    BB attackers = 0;
    if (!(bit & Files[0])) attackers |= e == WHITE ? bit >> 9 : bit << 7;
    if (!(bit & Files[7])) attackers |= e == WHITE ? bit >> 7 : bit << 9;
    if (attackers & pawns) return true;

    return false;
}

bool IsOrthAttacked(struct Board* b, int loc, BB targets, BB (*rays)[8])
{
    BB all = b->all[WHITE] | b->all[BLACK];
    BB ray, attackers;

    ray = rays[loc][N];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[LSB(attackers)][N]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][E];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[LSB(attackers)][E]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][S];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[MSB(attackers)][S]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][W];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[MSB(attackers)][W]);
        if (!(ray & all)) return true;
    }

    return false;
}

bool IsDiagAttacked(struct Board* b, int loc, BB targets, BB (*rays)[8])
{
    BB all = b->all[WHITE] | b->all[BLACK];
    BB ray, attackers;

    ray = rays[loc][NE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[LSB(attackers)][NE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][NW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[LSB(attackers)][NW]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[MSB(attackers)][SE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[MSB(attackers)][SW]);
        if (!(ray & all)) return true;
    }

    return false;
}
