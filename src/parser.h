#ifndef __PARSER_H__
#define __PARSER_H__

#include "board.h"

char square_from_fen(char);
char fen_from_square(char);
int coord_from_fen(const char*);

#endif // __PARSER_H__
