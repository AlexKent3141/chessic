#include "board.h"
#include "board_state.h"
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
    b->states = CreateStack();

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
    copy->states = CopyStack((struct StateStack*)b->states);

    return copy;
}

bool CSC_BoardEqual(struct CSC_Board* b1, struct CSC_Board* b2)
{
    /* First check the board hash - normally this is sufficient. */
    if (CSC_GetHash(b1) != CSC_GetHash(b2))
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

CSC_Hash CSC_GetHash(struct CSC_Board* b)
{
    struct BoardState* bs = Top((struct StateStack*)b->states);
    return bs->hash;
}

int CSC_GetEnPassentIndex(struct CSC_Board* b)
{
    struct BoardState* bs = Top((struct StateStack*)b->states);
    return bs->enPassentIndex;
}

int CSC_GetPlies50Move(struct CSC_Board* b)
{
    struct BoardState* bs = Top((struct StateStack*)b->states);
    return bs->plies50Move;
}

struct CSC_CastlingRights CSC_GetCastlingRights(
    struct CSC_Board* b,
    enum CSC_Colour p)
{
    struct BoardState* bs = Top((struct StateStack*)b->states);
    return bs->castlingRights[p];
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

    if (hash != NULL)
    {
        *hash ^= keys.pieceSquare[p][pt][loc];
    }

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

    if (hash != NULL)
    {
        *hash ^= keys.pieceSquare[p][pt][loc];
    }
}

void CSC_MakeMove(struct CSC_Board* b, CSC_Move m)
{
    assert(b != NULL);
    assert(b->states != NULL);

    int p = b->player;
    int s = CSC_GetMoveStart(m);
    int e = CSC_GetMoveEnd(m);

    struct BoardState* bs = Top((struct StateStack*)b->states);
    CSC_Hash hash = bs->hash;

    CSC_Piece sp = RemovePiece(b, s, &hash);
    CSC_Piece cap = b->squares[e];

    enum CSC_PieceType pt = CSC_GetPieceType(sp);
    enum CSC_MoveType mt = CSC_GetMoveType(m);

    if (cap && mt != CSC_ENPASSENT) RemovePiece(b, e, &hash);

    if (mt == CSC_ENPASSENT)
    {
        assert(bs->enPassentIndex != CSC_BAD_LOC);

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

    struct CSC_CastlingRights ourCastlingRights = bs->castlingRights[p];
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

    struct CSC_CastlingRights theirCastlingRights = bs->castlingRights[1-p];
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

    // Fill in the next board state.
    int plies50Move = bs->plies50Move + 1;

    struct BoardState* next = Push((struct StateStack*)b->states);
    next->lastMove = m;
    next->lastMoveCapture = cap;
    next->lastMovePieceType = pt;
    next->enPassentIndex = ep;
    next->plies50Move = plies50Move;
    next->castlingRights[p] = ourCastlingRights;
    next->castlingRights[1-p] = theirCastlingRights;
    next->hash = hash;

    if (cap || pt == CSC_PAWN) next->plies50Move = 0;

    b->player = 1 - p;
    next->hash ^= keys.side;
}

void CSC_UndoMove(struct CSC_Board* b)
{
    assert(b != NULL);
    assert(b->states != NULL);

    // Extract the required info from the state and revert to previous.
    struct BoardState* old = Pop((struct StateStack*)b->states);
    CSC_Move m = old->lastMove;
    int cap = old->lastMoveCapture;

    b->player = 1 - b->player;

    int p = b->player;
    int s = CSC_GetMoveStart(m);
    int e = CSC_GetMoveEnd(m);

    // Move the pieces back.
    CSC_Piece pc = RemovePiece(b, e, NULL);

    enum CSC_MoveType mt = CSC_GetMoveType(m);
    if (mt == CSC_PROMOTION) CSC_SetPieceType(&pc, CSC_PAWN);

    AddPiece(b, s, pc, NULL);

    if (cap)
    {
        int capLoc = e;
        if (mt == CSC_ENPASSENT)
            capLoc = e + (p == CSC_WHITE ? -CSC_FILE_NB : CSC_FILE_NB);

        AddPiece(b, capLoc, cap, NULL);
    }

    if (mt & CSC_CASTLE)
    {
        // Move the corresponding rook back.
        int startFile = mt == CSC_KINGCASTLE ? 5 : 3;
        int endFile = mt == CSC_KINGCASTLE ? 7 : 0;
        int rank = p == CSC_WHITE ? 0 : 7;

        CSC_Piece rook = RemovePiece(b, 8*rank + startFile, NULL);
        AddPiece(b, 8*rank + endFile, rook, NULL);

        assert(CSC_GetPieceType(rook) == CSC_ROOK);
        assert(CSC_GetPieceColour(rook) == p);
    }
}

/* There's some duplication of both MakeMove and UndoMove in this function. */
/* Essentially we need to partially make the move in order to check that it
   is legal (i.e. the king doesn't end in check). */
bool CSC_IsLegal(struct CSC_Board* b, CSC_Move m)
{
    assert(b != NULL);
    assert(b->states != NULL);

    int p = b->player;
    int s = CSC_GetMoveStart(m);
    int e = CSC_GetMoveEnd(m);

    struct BoardState* bs = Top((struct StateStack*)b->states);
    CSC_Piece sp = RemovePiece(b, s, NULL);
    CSC_Piece cap = b->squares[e];

    enum CSC_MoveType mt = CSC_GetMoveType(m);

    if (cap && mt != CSC_ENPASSENT) RemovePiece(b, e, NULL);

    int capLoc = e;
    if (mt == CSC_ENPASSENT)
    {
        assert(bs->enPassentIndex != CSC_BAD_LOC);

        capLoc = e + (p == CSC_WHITE ? -CSC_FILE_NB : CSC_FILE_NB);
        cap = RemovePiece(b, capLoc, NULL);
    }
    else if (mt & CSC_CASTLE)
    {
        // Move the corresponding rook.
        int startFile = mt == CSC_KINGCASTLE ? 7 : 0;
        int end_file = mt == CSC_KINGCASTLE ? 5 : 3;
        int rank = p == CSC_WHITE ? 0 : 7;

        CSC_Piece rook = RemovePiece(b, 8*rank + startFile, NULL);
        AddPiece(b, 8*rank + end_file, rook, NULL);

        assert(CSC_GetPieceType(rook) == CSC_ROOK);
        assert(CSC_GetPieceColour(rook) == p);
    }

    // Add the piece back now that the promotion has been applied.
    AddPiece(b, e, sp, NULL);

    bool legal = !CSC_IsAttacked(b, CSC_LSB(b->pieces[CSC_KING][p]));

    /* Undo the previously made board changes. */
    RemovePiece(b, e, NULL);

    AddPiece(b, s, sp, NULL);

    if (cap)
    {
        AddPiece(b, capLoc, cap, NULL);
    }

    if (mt & CSC_CASTLE)
    {
        // Move the corresponding rook back.
        int startFile = mt == CSC_KINGCASTLE ? 5 : 3;
        int endFile = mt == CSC_KINGCASTLE ? 7 : 0;
        int rank = p == CSC_WHITE ? 0 : 7;

        CSC_Piece rook = RemovePiece(b, 8*rank + startFile, NULL);
        AddPiece(b, 8*rank + endFile, rook, NULL);

        assert(CSC_GetPieceType(rook) == CSC_ROOK);
        assert(CSC_GetPieceColour(rook) == p);
    }

    return legal;
}

bool CSC_IsDrawn(struct CSC_Board* b)
{
    struct BoardState* bs = Top((struct StateStack*)b->states);
    if (bs->plies50Move >= 50) return true;

    /* Examine hashes to determine whether there has been a draw by repetition. */
    /* When checking these we can stop when we find the first irreversible move
       i.e. a pawn move or a capture. */
    CSC_Hash latestHash = bs->hash;
    int hashCount = 1;
    bs = bs->previousState;
    while (bs != NULL
        && !bs->lastMoveCapture
        && bs->lastMovePieceType != CSC_PAWN
        && hashCount < 3)
    {
        hashCount += bs->hash == latestHash;
        bs = bs->previousState;
    }

    return hashCount >= 3;
}

void CSC_FreeBoard(struct CSC_Board* b)
{
    if (b)
    {
        if (b->states != NULL)
        {
            FreeStack((struct StateStack*)b->states);
            b->states = NULL;
        }

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

    ray = rays[loc][CSC_NORTH];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][CSC_NORTH]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_EAST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][CSC_EAST]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_SOUTH];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][CSC_SOUTH]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_WEST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][CSC_WEST]);
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

    ray = rays[loc][CSC_NORTHEAST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][CSC_NORTHEAST]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_NORTHWEST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_LSB(attackers)][CSC_NORTHWEST]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_SOUTHEAST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][CSC_SOUTHEAST]);
        if (!(ray & all)) return true;
    }

    ray = rays[loc][CSC_SOUTHWEST];
    attackers = ray & targets;
    if (attackers)
    {
        ray ^= (attackers | rays[CSC_MSB(attackers)][CSC_SOUTHWEST]);
        if (!(ray & all)) return true;
    }

    return false;
}

bool CSC_IsAttacked(struct CSC_Board* b, int loc)
{
    int p = b->player;
    int e = 1-p;

    // Check steppers.
    if (CSC_KingAttacks[loc] & b->pieces[CSC_KING][e]) return true;
    if (CSC_KnightAttacks[loc] & b->pieces[CSC_KNIGHT][e]) return true;

    // Check orthogonal rays.
    CSC_Bitboard targets = b->pieces[CSC_ROOK][e] | b->pieces[CSC_QUEEN][e];
    if (CSC_RayAttacksAll[loc][CSC_ORTHOGONAL] & targets)
    {
        if (IsOrthAttacked(b, loc, targets, CSC_RayAttacks)) return true;
    }

    // Check diagonal rays.
    targets = b->pieces[CSC_BISHOP][e] | b->pieces[CSC_QUEEN][e];
    if (CSC_RayAttacksAll[loc][CSC_DIAGONAL] & targets)
    {
        if (IsDiagAttacked(b, loc, targets, CSC_RayAttacks)) return true;
    }

    // Check pawns.
    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    CSC_Bitboard pawns = b->pieces[CSC_PAWN][e];

    CSC_Bitboard attackers = 0;
    if (!(bit & CSC_Files[0])) attackers |= e == CSC_WHITE ? bit >> 9 : bit << 7;
    if (!(bit & CSC_Files[7])) attackers |= e == CSC_WHITE ? bit >> 7 : bit << 9;
    if (attackers & pawns) return true;

    return false;
}
