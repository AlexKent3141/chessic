#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stdint.h"

#define EXPORT __attribute__ ((visibility("default")))

#define CSC_FILE_NB 8
#define CSC_RANK_NB 8
#define CSC_SQUARE_NB 64

#define CSC_BAD_LOC -1
#define CSC_MAX_MOVES 250

#define CSC_MAX_FEN_LENGTH 100

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t CSC_Bitboard; /* This is the bit board type. */
typedef uint32_t CSC_Move;
typedef uint16_t CSC_Piece;
typedef uint64_t CSC_Hash;

typedef enum
{
    N, S, W, E, NE, NW, SE, SW
} Directions;

typedef enum
{
    ORTH, DIAG
} RayType;

enum CSC_Colour
{
    CSC_WHITE,
    CSC_BLACK
};

/* The piece types. */
enum CSC_PieceType
{
    CSC_NONE,
    CSC_PAWN,
    CSC_KNIGHT,
    CSC_BISHOP,
    CSC_ROOK,
    CSC_QUEEN,
    CSC_KING
};

enum CSC_MoveType
{
    CSC_NORMAL = 0,
    CSC_TWOSPACE = 1,
    CSC_PROMOTION = 2,
    CSC_ENPASSENT = 4,
    CSC_CASTLE = 8,
    CSC_KINGCASTLE = CSC_CASTLE | 16,
    CSC_QUEENCASTLE = CSC_CASTLE | 32
};

enum CSC_MoveGenType
{
    CSC_QUIETS = 1,
    CSC_CAPTURES = 2,
    CSC_ALL = CSC_QUIETS | CSC_CAPTURES
};

struct CSC_MoveList
{
    CSC_Move* moves;
    CSC_Move* end;
    int n;
};

struct CSC_CastlingRights
{
    /* Whether the player can castle kingside. */
    bool kingSide;

    /* Whether the player can castle queenside. */
    bool queenSide;
};

struct CSC_BoardState
{
    /* The move that was applied to reach this state. */
    CSC_Move lastMove;

    /* The piece that was captured on the last move. */
    CSC_Piece lastMoveCapture;

    /* The type of piece that moved on the last move. */
    enum CSC_PieceType lastMovePieceType;

    /* The index of the square where an en-passent capture is possible. */
    int enPassentIndex;

    /* The number of plies since the last move that reset the 50 move rule. */
    int plies50Move;

    /* The castling rights for each player. */
    struct CSC_CastlingRights castlingRights[2];

    /* The current board hash. */
    CSC_Hash hash;

    /* The previous game state. */
    struct CSC_BoardState* previousState;
};

struct CSC_Board
{
    /* The player to move. */
    int player;

    /* The number of full turns so far. */
    int turnNumber;

    /* The state which varies per move. */
    struct CSC_BoardState* bs;

    /* The pieces on each square. */
    CSC_Piece squares[CSC_SQUARE_NB];

    /* All pieces for each player. */
    CSC_Bitboard all[2];

    /* The pieces of each type for each player. */
    CSC_Bitboard pieces[7][2];
};

/* Bitboard constants. */
EXPORT extern CSC_Bitboard Ranks[8];
EXPORT extern CSC_Bitboard Files[8];
EXPORT extern CSC_Bitboard KnightAttacks[64];
EXPORT extern CSC_Bitboard KingAttacks[64];
EXPORT extern CSC_Bitboard RayAttacks[64][8];
EXPORT extern CSC_Bitboard RayAttacksAll[64][2];

/* Initialisation functions which must be called first. */
EXPORT void CSC_InitBits();
EXPORT void CSC_InitZobrist();

/* Methods for interacting with bit boards. */
EXPORT int CSC_PopLSB(CSC_Bitboard*);
EXPORT int CSC_PopMSB(CSC_Bitboard*);
EXPORT int CSC_LSB(CSC_Bitboard);
EXPORT int CSC_MSB(CSC_Bitboard);
EXPORT bool CSC_Test(CSC_Bitboard, int);
EXPORT void CSC_PrintBitboard(CSC_Bitboard);

/* Methods for creating and interacting with the board. */
EXPORT struct CSC_Board* CSC_BoardFromFEN(const char*);
EXPORT char* CSC_FENFromBoard(struct CSC_Board*);
EXPORT struct CSC_Board* CSC_CopyBoard(struct CSC_Board*);
EXPORT bool CSC_BoardEqual(struct CSC_Board*, struct CSC_Board*);
EXPORT void CSC_FreeBoard(struct CSC_Board*);
EXPORT void CSC_PrintBoard(struct CSC_Board*);

/* Check whether the board is in a drawn state. */
EXPORT bool CSC_IsDrawn(struct CSC_Board*);

/* Generate pseudo-legal moves. */
EXPORT struct CSC_MoveList* CSC_GetMoves(struct CSC_Board*, enum CSC_MoveType);

/* Attempt to make the move (returns false if it's illegal). */
EXPORT bool CSC_MakeMove(struct CSC_Board*, CSC_Move);

/* Undo the last made move. */
EXPORT void CSC_UndoMove(struct CSC_Board*);

EXPORT bool CSC_IsAttacked(struct CSC_Board*, int);

/* Methods for creating and interacting with pieces. */
EXPORT CSC_Piece CSC_CreatePiece(char, enum CSC_PieceType);
EXPORT char CSC_GetPieceColour(CSC_Piece);
EXPORT enum CSC_PieceType CSC_GetPieceType(CSC_Piece);
EXPORT void CSC_SetPieceType(CSC_Piece*, enum CSC_PieceType);

/* Methods for creating and interacting with moves. */
EXPORT CSC_Move CSC_CreateMove(char, char, enum CSC_PieceType, enum CSC_MoveType);
EXPORT char CSC_GetMoveStart(CSC_Move);
EXPORT char CSC_GetMoveEnd(CSC_Move);
EXPORT enum CSC_PieceType CSC_GetMovePromotion(CSC_Move);
EXPORT enum CSC_MoveType CSC_GetMoveType(CSC_Move);
EXPORT void CSC_PrintMove(CSC_Move);
EXPORT struct CSC_MoveList* CSC_MakeMoveList();
EXPORT void CSC_AddMove(struct CSC_MoveList*, CSC_Move);
EXPORT void CSC_FreeMoveList(struct CSC_MoveList*);

#ifdef __cplusplus
}
#endif

#endif /* __CHESSIC_H__ */
