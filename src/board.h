#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

board* create_board_empty();

// Get the colour and piece type at the specified location.
void loc_details(board*, int, int*, int*);

// Generate the pawn moves.
void add_pawn_moves(board*, move_list*);

#endif // __CHESSIC_BOARD_H__
