#ifndef __PARSER_H__
#define __PARSER_H__

#include "board.h"

void set_piece_from_fen(board*, int, char);
char fen_from_piece(int, int);
int loc_from_fen(const char*);

#endif // __PARSER_H__
