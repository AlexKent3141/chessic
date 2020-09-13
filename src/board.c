#include "board.h"
#include "zobrist.h"
#include "assert.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct CSC_Board* CreateBoardEmpty()
{
    struct CSC_Board* b = malloc(sizeof(struct CSC_Board));

    b->player = CSC_WHITE;
    b->turnNumber = 0;

    struct CSC_BoardState* bs = malloc(sizeof(struct CSC_BoardState));
    memset(bs, 0, sizeof(struct CSC_BoardState));
    bs->enPassentIndex = CSC_BAD_LOC;
    bs->previousState = NULL;
    bs->hash = 0;
    b->bs = bs;

    memset(b->squares, 0, CSC_SQUARE_NB*sizeof(CSC_Piece));

    memset(b->all, 0, 2*sizeof(CSC_Bitboard));
    for (int i = 0; i <= CSC_KING; i++)
        memset(b->pieces[i], 0, 2*sizeof(CSC_Bitboard));

    return b;
}

struct CSC_Board* CSC_CopyBoard(struct CSC_Board* b)
{
    struct CSC_Board* copy = malloc(sizeof(struct CSC_Board));
    memcpy(copy, b, sizeof(struct CSC_Board));

    /* Perform a deep copy of the board state. */
    /* TODO: Should we copy all previous board states here? */
    copy->bs = malloc(sizeof(struct CSC_BoardState));
    memcpy(copy->bs, b->bs, sizeof(struct CSC_BoardState));

    return copy;
}

bool CSC_BoardEqual(struct CSC_Board* b1, struct CSC_Board* b2)
{
    /* First check the board hash - normally this is sufficient. */
    if (b1->bs->hash != b2->bs->hash)
        return false;

    /* The boards hash to the same value, need to check the details. */
    bool equal = true;
    for (int i = 0; i < CSC_SQUARE_NB; i++)
        equal &= (b1->squares[i] == b2->squares[i]);
    for (int p = CSC_PAWN; p <= CSC_KING; p++)
        equal &= (b1->pieces[p][CSC_WHITE] == b2->pieces[p][CSC_WHITE]);
    for (int p = CSC_PAWN; p <= CSC_KING; p++)
        equal &= (b1->pieces[p][CSC_BLACK] == b2->pieces[p][CSC_BLACK]);

    equal &= (b1->all[CSC_WHITE] == b2->all[CSC_WHITE]);
    equal &= (b1->all[CSC_BLACK] == b2->all[CSC_BLACK]);

    return equal;
}

CSC_Piece RemovePiece(struct CSC_Board* b, int loc, CSC_Hash* hash)
{
    CSC_Piece pc = b->squares[loc];
    int p = CSC_GetPieceColour(pc);
    enum CSC_PieceType pt = CSC_GetPieceType(pc);

    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    b->all[p] ^= bit;
    b->pieces[pt][p] ^= bit;
    b->squares[loc] = 0;
    *hash ^= keys.pieceSquare[b->player][pt][loc];

    return pc;
}

void AddPiece(struct CSC_Board* b, int loc, CSC_Piece pc, CSC_Hash* hash)
{
    int p = CSC_GetPieceColour(pc);
    enum CSC_PieceType pt = CSC_GetPieceType(pc);

    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    b->all[p] |= bit;
    b->pieces[pt][p] |= bit;
    b->squares[loc] = pc;
    *hash ^= keys.pieceSquare[b->player][pt][loc];
}

bool CSC_MakeMove(struct CSC_Board* b, CSC_Move m)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    int p = b->player;
    int s = CSC_GetMoveStart(m);
    int e = CSC_GetMoveEnd(m);

    CSC_Hash hash = b->bs->hash;

    CSC_Piece sp = RemovePiece(b, s, &hash);
    CSC_Piece cap = b->squares[e];

    enum CSC_PieceType pt = CSC_GetPieceType(sp);
    enum CSC_MoveType mt = CSC_GetMoveType(m);

    if (cap && mt != CSC_ENPASSENT) RemovePiece(b, e, &hash);

    if (mt == CSC_ENPASSENT)
    {
        assert(b->bs->enPassentIndex != CSC_BAD_LOC);

        int capLoc = e + (p == CSC_WHITE ? -CSC_FILE_NB : CSC_FILE_NB);
        cap = b->squares[capLoc];
        RemovePiece(b, capLoc, &hash);
    }
    else if (mt == CSC_PROMOTION)
    {
        // TODO: Hash update?
        CSC_SetPieceType(&sp, CSC_GetMovePromotion(m));
    }
    else if (mt & CSC_CASTLE)
    {
        // Move the corresponding rook.
        int startFile = mt == CSC_KINGCASTLE ? 7 : 0;
        int end_file = mt == CSC_KINGCASTLE ? 5 : 3;
        int rank = p == CSC_WHITE ? 0 : 7;

        CSC_Piece rook = b->squares[8*rank + startFile];

        assert(CSC_GetPieceType(rook) == CSC_ROOK);
        assert(CSC_GetPieceColour(rook) == p);

        RemovePiece(b, 8*rank + startFile, &hash);
        AddPiece(b, 8*rank + end_file, rook, &hash);

        hash ^= mt == CSC_KINGCASTLE
            ? keys.castling[b->player][0]
            : keys.castling[b->player][1];
    }

    // Add the piece back now that the promotion has been applied.
    AddPiece(b, e, sp, &hash);

    struct CSC_CastlingRights ourCastlingRights = b->bs->castlingRights[p];
    if (pt == CSC_KING)
    {
        ourCastlingRights.kingSide = false;
        ourCastlingRights.queenSide = false;
    }
    else if (pt == CSC_ROOK)
    {
        int kingRook = p == CSC_WHITE ? 7 : 63;
        int queenRook = p == CSC_WHITE ? 0 : 56;
        ourCastlingRights.kingSide &= (s != kingRook);
        ourCastlingRights.queenSide &= (s != queenRook);
    }

    struct CSC_CastlingRights theirCastlingRights = b->bs->castlingRights[1-p];
    if (cap && CSC_GetPieceType(cap) == CSC_ROOK)
    {
        int enemyKingRook = p == CSC_WHITE ? 63 : 7;
        int enemyQueenRook = p == CSC_WHITE ? 56 : 0;
        theirCastlingRights.kingSide &= (e != enemyKingRook);
        theirCastlingRights.queenSide &= (e != enemyQueenRook);
    }

    int ep = CSC_BAD_LOC;
    if (mt == CSC_TWOSPACE)
    {
        ep = e + (p == CSC_WHITE ? -8 : 8);
        hash ^= keys.enpassentFile[e % CSC_FILE_NB];
    }

    // Clone the previous board state.
    struct CSC_BoardState* next = malloc(sizeof(struct CSC_BoardState));
    memcpy(next, b->bs, sizeof(struct CSC_BoardState));
    next->lastMove = m;
    next->lastMoveCapture = cap;
    next->lastMovePieceType = pt;
    next->enPassentIndex = ep;
    next->castlingRights[p] = ourCastlingRights;
    next->castlingRights[1-p] = theirCastlingRights;
    next->hash = hash;

    ++next->plies50Move;
    if (cap || pt == CSC_PAWN) next->plies50Move = 0;

    next->previousState = b->bs;
    b->bs = next;

    // Ensure legality: the king cannot end in check.
    if (CSC_IsAttacked(b, CSC_LSB(b->pieces[CSC_KING][p])))
    {
        b->player = 1 - p;
        CSC_UndoMove(b);
        return false;
    }

    b->player = 1 - p;
    next->hash ^= keys.side;

    return true;
}

void CSC_UndoMove(struct CSC_Board* b)
{
    assert(b != NULL);
    assert(b->bs != NULL);

    // Extract the required info from the state and revert to previous.
    struct CSC_BoardState* old = b->bs;
    CSC_Move m = old->lastMove;
    int cap = old->lastMoveCapture;
    b->bs = old->previousState;
    free(old);

    b->player = 1 - b->player;

    int p = b->player;
    int s = CSC_GetMoveStart(m);
    int e = CSC_GetMoveEnd(m);
    CSC_Hash temp;

    // Move the pieces back.
    CSC_Piece pc = b->squares[e];

    RemovePiece(b, e, &temp);

    enum CSC_MoveType mt = CSC_GetMoveType(m);
    if (mt == CSC_PROMOTION) CSC_SetPieceType(&pc, CSC_PAWN);

    AddPiece(b, s, pc, &temp);

    if (cap)
    {
        int capLoc = e;
        if (mt == CSC_ENPASSENT)
            capLoc = e + (p == CSC_WHITE ? -CSC_FILE_NB : CSC_FILE_NB);

        AddPiece(b, capLoc, cap, &temp);
    }

    if (mt & CSC_CASTLE)
    {
        // Move the corresponding rook back.
        int startFile = mt == CSC_KINGCASTLE ? 5 : 3;
        int endFile = mt == CSC_KINGCASTLE ? 7 : 0;
        int rank = p == CSC_WHITE ? 0 : 7;

        CSC_Piece rook = b->squares[8*rank + startFile];

        assert(CSC_GetPieceType(rook) == CSC_ROOK);
        assert(CSC_GetPieceColour(rook) == p);

        RemovePiece(b, 8*rank + startFile, &temp);
        AddPiece(b, 8*rank + endFile, rook, &temp);
    }
}

bool CSC_IsDrawn(struct CSC_Board* b)
{
    if (b->bs->plies50Move >= 50) return true;

    /* Examine hashes to determine whether there has been a draw by repetition. */
    /* When checking these we can stop when we find the first irreversible move
       i.e. a pawn move or a capture. */
    CSC_Hash latestHash = b->bs->hash;
    int hashCount = 1;
    struct CSC_BoardState* prev = b->bs->previousState;
    while (prev != NULL
        && !prev->lastMoveCapture
        && prev->lastMovePieceType != CSC_PAWN
        && hashCount < 3)
    {
        hashCount += prev->hash == latestHash;
        prev = prev->previousState;
    }

    return hashCount >= 3;
}

void CSC_FreeBoard(struct CSC_Board* b)
{
    if (b)
    {
        struct CSC_BoardState* bs = b->bs;
        while (bs != NULL)
        {
            struct CSC_BoardState* prev = bs->previousState;
            free(bs);
            bs = prev;
        }

        b->bs = NULL;

        free(b);
        b = NULL;
    }
}

void LocDetails(struct CSC_Board* b, int loc, int* col, int* type)
{
    *col = CSC_GetPieceColour(b->squares[loc]);
    *type = CSC_GetPieceType(b->squares[loc]);
}

char LocToChar(int col, int type)
{
    static const char c[] = { '.', 'p', 'n', 'b', 'r', 'q', 'k' };
    return col == CSC_WHITE ? toupper(c[type]) : c[type];
}

void CSC_PrintBoard(struct CSC_Board* b)
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

bool IsOrthAttacked(
    struct CSC_Board* b,
    int loc,
    CSC_Bitboard targets,
    CSC_Bitboard (*rays)[8])
{
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];
    CSC_Bitboard ray, attackers;

    ray = rays[loc][N];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][N]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][E];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][E]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][S];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][S]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][W];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][W]);
        if (!(ray & all)) return true;
    }

    return false;
}

bool IsDiagAttacked(
    struct CSC_Board* b,
    int loc,
    CSC_Bitboard targets,
    CSC_Bitboard (*rays)[8])
{
    CSC_Bitboard all = b->all[CSC_WHITE] | b->all[CSC_BLACK];
    CSC_Bitboard ray, attackers;

    ray = rays[loc][NE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][NE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][NW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][NW]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SE];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][SE]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][SW];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][SW]);
        if (!(ray & all)) return true;
    }

    return false;
}

bool CSC_IsAttacked(struct CSC_Board* b, int loc)
{
    int p = b->player;
    int e = 1-p;

    // Check steppers.
    if (KingAttacks[loc] & b->pieces[CSC_KING][e]) return true;
    if (KnightAttacks[loc] & b->pieces[CSC_KNIGHT][e]) return true;

    // Check orthogonal rays.
    CSC_Bitboard targets = b->pieces[CSC_ROOK][e] | b->pieces[CSC_QUEEN][e];
    if (RayAttacksAll[loc][ORTH] & targets)
    {
        if (IsOrthAttacked(b, loc, targets, RayAttacks)) return true;
    }

    // Check diagonal rays.
    targets = b->pieces[CSC_BISHOP][e] | b->pieces[CSC_QUEEN][e];
    if (RayAttacksAll[loc][DIAG] & targets)
    {
        if (IsDiagAttacked(b, loc, targets, RayAttacks)) return true;
    }

    // Check pawns.
    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    CSC_Bitboard pawns = b->pieces[CSC_PAWN][e];

    CSC_Bitboard attackers = 0;
    if (!(bit & Files[0])) attackers |= e == CSC_WHITE ? bit >> 9 : bit << 7;
    if (!(bit & Files[7])) attackers |= e == CSC_WHITE ? bit >> 7 : bit << 9;
    if (attackers & pawns) return true;

    return false;
}
