#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stdint.h"

#define CSC_FILE_NB 8
#define CSC_RANK_NB 8
#define CSC_SQUARE_NB 64

#define CSC_BAD_LOC -1
#define CSC_MAX_MOVES 250

#define CSC_MAX_FEN_LENGTH 100

typedef uint64_t CSC_Bitboard; /* This is the bit board type. */
typedef uint32_t CSC_Move;
typedef uint16_t CSC_Piece;

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
    CSC_Piece captureOnLastMove;

    /* The index of the square where an en-passent capture is possible. */
    int enPassentIndex;

    /* The number of plies since the last move that reset the 50 move rule. */
    int plies50Move;

    /* The castling rights for each player. */
    struct CSC_CastlingRights castlingRights[2];

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

/* Methods for interacting with bit boards. */
void CSC_InitBits(); /* This must be called before generating moves! */
int CSC_PopLSB(CSC_Bitboard*);
int CSC_PopMSB(CSC_Bitboard*);
int CSC_LSB(CSC_Bitboard);
int CSC_MSB(CSC_Bitboard);
bool CSC_Test(CSC_Bitboard, int);
void CSC_PrintBitboard(CSC_Bitboard);

/* Methods for creating and interacting with the board. */
struct CSC_Board* CSC_BoardFromFEN(const char*);
char* CSC_FENFromBoard(struct CSC_Board*);
struct CSC_Board* CSC_CopyBoard(struct CSC_Board*);
bool CSC_BoardEqual(struct CSC_Board*, struct CSC_Board*);
void CSC_FreeBoard(struct CSC_Board*);
void CSC_PrintBoard(struct CSC_Board*);

/* Generate pseudo-legal moves. */
struct CSC_MoveList* CSC_GetMoves(struct CSC_Board*, enum CSC_MoveType);

/* Attempt to make the move (returns false if it's illegal). */
bool CSC_MakeMove(struct CSC_Board*, CSC_Move);

/* Undo the last made move. */
void CSC_UndoMove(struct CSC_Board*);

bool CSC_IsAttacked(struct CSC_Board*, int);

/* Methods for creating and interacting with pieces. */
CSC_Piece CSC_CreatePiece(char, enum CSC_PieceType);
char CSC_GetPieceColour(CSC_Piece);
enum CSC_PieceType CSC_GetPieceType(CSC_Piece);
void CSC_SetPieceType(CSC_Piece*, enum CSC_PieceType);

/* Methods for creating and interacting with moves. */
CSC_Move CSC_CreateMove(char, char, enum CSC_PieceType, enum CSC_MoveType);
char CSC_GetMoveStart(CSC_Move);
char CSC_GetMoveEnd(CSC_Move);
enum CSC_PieceType CSC_GetMovePromotion(CSC_Move);
enum CSC_MoveType CSC_GetMoveType(CSC_Move);
void CSC_PrintMove(CSC_Move);
struct CSC_MoveList* CSC_MakeMoveList();
void CSC_AddMove(struct CSC_MoveList*, CSC_Move);
void CSC_FreeMoveList(struct CSC_MoveList*);

#endif /* __CHESSIC_H__ */
