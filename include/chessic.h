#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stdint.h"

#define FILE_NB 8
#define RANK_NB 8
#define SQUARE_NB 64

#define BAD_LOC -1
#define MAX_MOVES 250

typedef uint64_t bb; // This is the bit board type.
typedef uint32_t move;
typedef uint16_t piece;

typedef enum
{
    WHITE, BLACK
} COLOUR;

// The piece types.
typedef enum
{
    NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PIECE_TYPE;


typedef enum
{
    NORMAL = 0,
    TWOSPACE = 1,
    PROMOTION = 2,
    ENPASSENT = 4,
    CASTLE = 8,
    KINGCASTLE = CASTLE | 16,
    QUEENCASTLE = CASTLE | 32
} MOVE_TYPE;

typedef enum
{
    QUIETS = 1,
    CAPTURES = 2,
    ALL = QUIETS | CAPTURES
} MOVE_GEN_TYPE;

typedef struct
{
    move* moves;
    move* end;
    int n;
} move_list;

typedef struct
{
    bool ks; // Whether the player can castle kingside.
    bool qs; // Whether the player can castle queenside.
} castling;

typedef struct
{
    move last_move;         // The move that was applied to reach this state.
    piece cap;              // The piece that was captured on the last move.
    int ep;                 // The index of the square where an en-passent capture is possible.
    int plies_50_move;      // The plies since the last move that reset the 50 move rule.
    castling crs[2];        // The castling rights for each player.
    struct state* previous; // The previous state data.
} state;

typedef struct
{
    int player;     // The player to move.
    int turn;       // The number of full turns so far.
    state* bs;      // The board state which varies per move.
    piece squares[SQUARE_NB]; // The pieces stored as an array.

    bb all[2]; // All pieces for each player.
    bb pieces[7][2]; // The pieces of each type for each player.
} board;

// Methods for interacting with bit boards.
void init_bits(); // This must be called before generating moves!
int pop_lsb(bb*);
int pop_msb(bb*);
int lsb(bb);
int msb(bb);
bool test(bb, int);
void print_bb(bb);

// Methods for creating and interacting with the board.
board* board_from_fen(const char*);
char* fen_from_board(board*);
void free_board(board*);
void print_board(board*);
move_list* get_moves(board*, MOVE_TYPE); // Generate pseudo-legal moves.
bool make_move(board*, move); // Attempt to make the move (returns false if it's illegal).
void undo_move(board*); // Undo the last made move.

// Methods for creating and interacting with pieces.
piece create_piece(char, PIECE_TYPE);
char get_piece_colour(piece);
PIECE_TYPE get_piece_type(piece);
void set_piece_type(piece*, PIECE_TYPE);

// Methods for creating and interacting with moves.
move create_move(char, char, PIECE_TYPE, MOVE_TYPE);
char get_start(move);
char get_end(move);
PIECE_TYPE get_promotion(move);
MOVE_TYPE get_move_type(move);
void print_move(move);
move_list* make_move_list();
void add_move(move_list*, move);
void free_move_list(move_list*);

#endif // __CHESSIC_H__
