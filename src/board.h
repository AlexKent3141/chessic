#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

board* create_board_empty();
int file_rank_to_board(int, int);
int coord_to_board(int);
int board_to_coord(int);

char create_square(int, int);
int piece_from_square(char);
int col_from_square(char);

#endif // __CHESSIC_BOARD_H__
