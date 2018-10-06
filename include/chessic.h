#ifndef __CHESSIC_H__
#define __CHESSIC_H__

#include "stdbool.h"
#include "stdint.h"

#define FILE_NB 8
#define RANK_NB 8

#define WHITE 0
#define BLACK 1

#define NONE   0
#define PAWN   1
#define KNIGHT 2
#define BISHOP 3
#define ROOK   4
#define QUEEN  5
#define KING   6

#define BAD_LOC -1

#define MAX_MOVES 250

// This is the bit board type.
typedef uint64_t bb;

// The first byte is the start location of the move and the second is the end location.
// The locations are specified in the 0x88 coordinate system.
typedef uint16_t move;

typedef enum
{
    QUIETS = 1,
    CAPTURES = 2,
    ALL = QUIETS | CAPTURES
} MOVE_TYPE;

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
    int cap;                // The piece that was captured on the last move.
    int ep;                 // The index of the square where an en-passent capture is possible.
    int plies_50_move;      // The plies since the last move that reset the 50 move rule.
    castling crs[2];        // The castling rights for each player.
    struct state* previous; // The previous state data.
} state;

typedef struct
{
    int player;    // The player to move.
    int turn;      // The number of full turns so far.
    state* bs;     // The board state which varies per move.
    bb pieces[2];  // The pieces for each colour.
    bb pawns[2];
    bb knights[2];
    bb bishops[2];
    bb rooks[2];
    bb queens[2];
    bb kings[2];
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

// Methods for creating and interacting with individual moves.
move create_move(char, char, char);
char get_start(move);
char get_end(move);
void print_move(move);
move_list* make_move_list();
void add_move(move_list*, move);
void free_move_list(move_list*);

#endif // __CHESSIC_H__
