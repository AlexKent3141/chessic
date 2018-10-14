#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

board* create_board_empty();

// Get the colour and piece type at the specified location.
void loc_details(board*, int, int*, int*);

piece remove_piece(board*, int);
void add_piece(board*, int, piece);

bool is_orth_attacked(board*, int, bb, bb(*)[8]);
bool is_diag_attacked(board*, int, bb, bb(*)[8]);

#endif // __CHESSIC_BOARD_H__
