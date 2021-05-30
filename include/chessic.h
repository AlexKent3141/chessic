#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#define EXPORT __attribute__ ((visibility("default")))

#define CSC_FILE_NB 8
#define CSC_RANK_NB 8
#define CSC_SQUARE_NB 64

#define CSC_BAD_LOC -1
#define CSC_MAX_MOVES 250
#define CSC_MAX_GAME_LENGTH 6000

#define CSC_MAX_FEN_LENGTH 100
#define CSC_MAX_UCI_MOVE_LENGTH 6

#define CSC_MAX_UCI_COMMAND_LENGTH \
    CSC_MAX_UCI_MOVE_LENGTH * CSC_MAX_GAME_LENGTH + 100

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t CSC_Bitboard; /* This is the bit board type. */
typedef uint32_t CSC_Move;
typedef uint16_t CSC_Piece;
typedef uint64_t CSC_Hash;

enum CSC_Directions
{
    CSC_NORTH,
    CSC_SOUTH,
    CSC_WEST,
    CSC_EAST,
    CSC_NORTHEAST,
    CSC_NORTHWEST,
    CSC_SOUTHEAST,
    CSC_SOUTHWEST
};

enum CSC_RayType
{
    CSC_ORTHOGONAL,
    CSC_DIAGONAL
};

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

struct CSC_Board
{
    /* The player to move. */
    int player;

    /* The number of full turns so far. */
    int turnNumber;

    /* The pieces on each square. */
    CSC_Piece squares[CSC_SQUARE_NB];

    /* All pieces for each player. */
    CSC_Bitboard all[2];

    /* The pieces of each type for each player. */
    CSC_Bitboard pieces[7][2];

    /* Opaque pointer to a stack containing the state that varies per move. */
    void* states;
};

/* Bitboard constants. */
EXPORT extern CSC_Bitboard CSC_Ranks[8];
EXPORT extern CSC_Bitboard CSC_Files[8];
EXPORT extern CSC_Bitboard CSC_KnightAttacks[64];
EXPORT extern CSC_Bitboard CSC_KingAttacks[64];
EXPORT extern CSC_Bitboard CSC_RayAttacks[64][8];
EXPORT extern CSC_Bitboard CSC_RayAttacksAll[64][2];

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
EXPORT void CSC_FENFromBoard(struct CSC_Board*, char*, int*);
EXPORT struct CSC_Board* CSC_CopyBoard(struct CSC_Board*);
EXPORT bool CSC_BoardEqual(struct CSC_Board*, struct CSC_Board*);
EXPORT void CSC_FreeBoard(struct CSC_Board*);
EXPORT void CSC_PrintBoard(struct CSC_Board*);
EXPORT CSC_Hash CSC_GetHash(struct CSC_Board*);
EXPORT int CSC_GetEnPassentIndex(struct CSC_Board*);
EXPORT int CSC_GetPlies50Move(struct CSC_Board*);

struct CSC_CastlingRights CSC_GetCastlingRights(
    struct CSC_Board*,
    enum CSC_Colour);

/* Check whether the board is in a drawn state. */
EXPORT bool CSC_IsDrawn(struct CSC_Board*);

/* Check whether the specified move is legal in the given board state. */
EXPORT bool CSC_IsLegal(struct CSC_Board*, CSC_Move);

/* Generate pseudo-legal moves. */
EXPORT struct CSC_MoveList* CSC_GetMoves(struct CSC_Board*, enum CSC_MoveGenType);

/* Attempt to make the move (returns false if it's illegal). */
EXPORT void CSC_MakeMove(struct CSC_Board*, CSC_Move);

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
EXPORT struct CSC_MoveList* CSC_MakeMoveList();
EXPORT void CSC_AddMove(struct CSC_MoveList*, CSC_Move);
EXPORT void CSC_FreeMoveList(struct CSC_MoveList*);
EXPORT void CSC_MoveToUCIString(CSC_Move, char*, int*);
EXPORT CSC_Move CSC_MoveFromUCIString(struct CSC_Board*, const char*);

/*** From here on are functions to support UCI. ***/

/* The supported search constraints. If a field is NULL in this structure then
   it has not been specified in the command. */
struct CSC_SearchConstraints
{
    struct CSC_MoveList* searchMoves;
    int* depth;
    int* numNodes;
    int* mate;
    bool* ponder;
};

/* Time information for a search. */
/* The supported time constraints. If a field is NULL in this structure then
   it has not been specified in the command. */
struct CSC_TimeConstraints
{
    int* wTime;
    int* bTime;
    int* wInc;
    int* bInc;
    int* movesToGo;
    int* moveTime;
    bool* infinite;
};

/* Callbacks for each type of UCI command. */
struct CSC_UCICallbacks
{
    void (*onUCI)();
    void (*onDebug)(bool);
    void (*onIsReady)();
    void (*onSetOptionName)(const char*);
    void (*onSetOptionNameValue)(const char*, const char*);
    void (*onNewGame)();
    void (*onPosition)(struct CSC_Board*);
    void (*onGo)(struct CSC_SearchConstraints*, struct CSC_TimeConstraints*);
    void (*onStop)();
    void (*onPonderHit)();
    void (*onQuit)();
};

/* Process a UCI command using the specified callbacks. */
EXPORT void CSC_UCIProcess(
    const char*,
    struct CSC_UCICallbacks*);

/* From here on are the commands that the engine can send to the GUI. */

/* The possible types of score that can be reported. Exactly one of these
   should be provided - the rest should be NULL. */
struct CSC_UCIScore
{
    int* cp;
    int* mate;
    int* lowerBound;
    int* upperBound;
};

struct CSC_UCIInfo
{
    int* depth;
    int* selDepth;
    int* time;
    int* nodes;
    struct CSC_MoveList* pv;
    void* multipv; /* TODO. */
    struct CSC_UCIScore* score;
    CSC_Move* currMove;
    int* currMoveNumber;
    int* hashFull;
    int* nps;
    int* tbHits;
    const char* string;
    void* refutation; /* TODO */
    void* currLine; /* TODO */
};

enum CSC_UCIOptionType
{
    CSC_UCI_CHECK,
    CSC_UCI_SPIN,
    CSC_UCI_COMBO,
    CSC_UCI_STRING,
    CSC_UCI_BUTTON
};

enum CSC_UCIOptionValueType
{
    CSC_UCI_DEFAULT,
    CSC_UCI_MIN,
    CSC_UCI_MAX,
    CSC_UCI_VAR
};

struct CSC_UCIOption
{
    char* name;
    enum CSC_UCIOptionType type;
    enum CSC_UCIOptionValueType valueType;
};

EXPORT void CSC_UCISendId(
    const char* name,
    const char* author);

EXPORT void CSC_UCISendReadyOK();

EXPORT void CSC_UCIBestMove(
    CSC_Move bestMove,
    CSC_Move* ponderMove);

EXPORT void CSC_UCIInfo(
    struct CSC_UCIInfo* info);

EXPORT void CSC_UCISupportedOptions(
    struct CSC_UCIOption* options,
    int numOptions);

/* Small utilities to help with parsing. */
struct CSC_TokenState
{
    char* str;
    size_t start;
    size_t len;
};

EXPORT char* CSC_Token(
    char* str,
    char delimiter,
    struct CSC_TokenState* state);

#ifdef __cplusplus
}
#endif

#endif /* __CHESSIC_H__ */
