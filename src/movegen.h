#ifndef __MOVE_GEN_H__
#define __MOVE_GEN_H__

#include "../include/chessic.h"

void FindPawnMoves(struct Board*, struct MoveList*, enum MoveType);
void FindKnightMoves(struct Board*, struct MoveList*, BB);
void FindKingMoves(struct Board*, struct MoveList*, BB);

void FindCastlingMoves(struct Board*, struct MoveList*);
void FindStepperMoves(struct MoveList*, BB, BB, BB*);
void FindOrthMoves(struct Board*, struct MoveList*, BB, BB, BB(*)[8]);
void FindDiagMoves(struct Board*, struct MoveList*, BB, BB, BB(*)[8]);

void AddMoves(int, struct MoveList*, BB);
void AddPawnMoves(BB, struct MoveList*, int, enum MoveType);
void AddPromoMoves(BB, struct MoveList*, int);

#endif // __MOVE_GEN_H__
