#ifndef __PARSER_H__
#define __PARSER_H__

#include "board.h"

void SetPieceFromFEN(struct Board*, int, char);
char FENFromPiece(int, int);
int LocFromFEN(const char*);

#endif /* __PARSER_H__ */
