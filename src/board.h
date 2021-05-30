#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "chessic.h"

struct CSC_Board* CreateBoardEmpty();

/* Get the colour and piece type at the specified location. */
void LocDetails(struct CSC_Board*, int, int*, int*);

#endif /* __CHESSIC_BOARD_H__ */
