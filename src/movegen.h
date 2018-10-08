#ifndef __MOVE_GEN_H__
#define __MOVE_GEN_H__

#include "../include/chessic.h"

void find_pawn_moves(board*, move_list*, MOVE_TYPE);
void find_knight_moves(board*, move_list*, bb);
void find_king_moves(board*, move_list*, bb);

void find_castling_moves(board*, move_list*);
void find_stepper_moves(board*, move_list*, bb, bb, bb*);
void find_orth_moves(board*, move_list*, MOVE_TYPE, bb, bb, bb(*)[8]);
void find_diag_moves(board*, move_list*, MOVE_TYPE, bb, bb, bb(*)[8]);

void add_moves(int, move_list*, bb);
void add_pawn_moves(bb, move_list*, int, MOVE_TYPE);
void add_promo_moves(bb, move_list*, int);

bool is_attacked(board*, int);
bool is_orth_attacked(board*, int, bb, bb(*)[8]);
bool is_diag_attacked(board*, int, bb, bb(*)[8]);

#endif // __MOVE_GEN_H__
