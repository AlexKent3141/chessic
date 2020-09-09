#ifndef __CHESSIC_BOARD_H__
#define __CHESSIC_BOARD_H__

#include "../include/chessic.h"

struct CSC_Board* CreateBoardEmpty();

/* Get the colour and piece type at the specified location. */
void LocDetails(struct CSC_Board*, int, int*, int*);

CSC_Piece RemovePiece(struct CSC_Board*, int);
void AddPiece(struct CSC_Board*, int, CSC_Piece);

bool IsOrthAttacked(struct CSC_Board*, int, CSC_Bitboard, CSC_Bitboard(*)[8]);
bool IsDiagAttacked(struct CSC_Board*, int, CSC_Bitboard, CSC_Bitboard(*)[8]);

#endif /* __CHESSIC_BOARD_H__ */
