#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

board* create_board_empty();

// Get the colour and piece type at the specified location.
void loc_details(board*, int, int*, int*);

void find_pawn_moves(board*, move_list*, MOVE_TYPE);
void find_knight_moves(board*, move_list*, bb);
void find_king_moves(board*, move_list*, bb);

void find_castling_moves(board*, move_list*);
void find_stepper_moves(board*, move_list*, bb, bb, bb*);
void find_orth_moves(board*, move_list*, MOVE_TYPE, bb, bb, bb(*)[8]);

void add_moves(int, move_list*, bb);
void add_pawn_moves(bb, move_list*, int);
void add_promo_moves(bb, move_list*, int);

#endif // __CHESSIC_BOARD_H__
