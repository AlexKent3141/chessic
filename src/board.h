#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

struct Board* CreateBoardEmpty();

/* Get the colour and piece type at the specified location. */
void LocDetails(struct Board*, int, int*, int*);

Piece RemovePiece(struct Board*, int);
void AddPiece(struct Board*, int, Piece);

bool IsOrthAttacked(struct Board*, int, BB, BB(*)[8]);
bool IsDiagAttacked(struct Board*, int, BB, BB(*)[8]);

#endif /* __CHESSIC_BOARD_H__ */
