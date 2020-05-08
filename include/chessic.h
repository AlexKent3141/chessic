#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stdint.h"

#define FILE_NB 8
#define RANK_NB 8
#define SQUARE_NB 64

#define BAD_LOC -1
#define MAX_MOVES 250

#define MAX_FEN_LENGTH 100

typedef uint64_t BB; /* This is the bit board type. */
typedef uint32_t Move;
typedef uint16_t Piece;

enum Colour
{
    WHITE, BLACK
};

/* The piece types. */
enum PieceType
{
    NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum MoveType
{
    NORMAL = 0,
    TWOSPACE = 1,
    PROMOTION = 2,
    ENPASSENT = 4,
    CASTLE = 8,
    KINGCASTLE = CASTLE | 16,
    QUEENCASTLE = CASTLE | 32
};

enum MoveGenType
{
    QUIETS = 1,
    CAPTURES = 2,
    ALL = QUIETS | CAPTURES
};

struct MoveList
{
    Move* moves;
    Move* end;
    int n;
};

struct CastlingRights
{
    bool kingSide;  /* Whether the player can castle kingside. */
    bool queenSide; /* Whether the player can castle queenside. */
};

struct BoardState
{
    Move lastMove;             /* The move that was applied to reach this state. */
    Piece captureOnLastMove;   /* The piece that was captured on the last move. */

    /* The index of the square where an en-passent capture is possible. */
    int enPassentIndex;

    /* The number of plies since the last move that reset the 50 move rule. */
    int plies50Move;

    /* The castling rights for each player. */
    struct CastlingRights castlingRights[2];

    /* The previous game state. */
    struct BoardState* previousState;
};

struct Board
{
    int player;               /* The player to move. */
    int turnNumber;           /* The number of full turns so far. */
    struct BoardState* bs;    /* The state which varies per move. */
    Piece squares[SQUARE_NB]; /* The pieces on each square. */
    BB all[2];                /* All pieces for each player. */
    BB pieces[7][2];          /* The pieces of each type for each player. */
};

/* Methods for interacting with bit boards. */
void InitBits(); /* This must be called before generating moves! */
int PopLSB(BB*);
int PopMSB(BB*);
int LSB(BB);
int MSB(BB);
bool Test(BB, int);
void PrintBB(BB);

/* Methods for creating and interacting with the board. */
struct Board* BoardFromFEN(const char*);
char* FENFromBoard(struct Board*);
struct Board* CopyBoard(struct Board*);
bool BoardEqual(struct Board*, struct Board*);
void FreeBoard(struct Board*);
void PrintBoard(struct Board*);

/* Generate pseudo-legal moves. */
struct MoveList* GetMoves(struct Board*, enum MoveType);

/* Attempt to make the move (returns false if it's illegal). */
bool MakeMove(struct Board*, Move);

/* Undo the last made move. */
void UndoMove(struct Board*);

bool IsAttacked(struct Board*, int);

/* Methods for creating and interacting with pieces. */
Piece CreatePiece(char, enum PieceType);
char GetPieceColour(Piece);
enum PieceType GetPieceType(Piece);
void SetPieceType(Piece*, enum PieceType);

/* Methods for creating and interacting with moves. */
Move CreateMove(char, char, enum PieceType, enum MoveType);
char GetMoveStart(Move);
char GetMoveEnd(Move);
enum PieceType GetMovePromotion(Move);
enum MoveType GetMoveType(Move);
void PrintMove(Move);
struct MoveList* MakeMoveList();
void AddMove(struct MoveList*, Move);
void FreeMoveList(struct MoveList*);

#endif /* __CHESSIC_H__ */
