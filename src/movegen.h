#ifndef __MOVE_GEN_H__
#define __MOVE_GEN_H__

#include "../include/chessic.h"

void FindPawnMoves(
    struct CSC_Board*,
    struct CSC_MoveList*,
    enum CSC_MoveType);

void FindKnightMoves(
    struct CSC_Board*,
    struct CSC_MoveList*,
    CSC_Bitboard);

void FindKingMoves(
    struct CSC_Board*,
    struct CSC_MoveList*,
    CSC_Bitboard);

void FindCastlingMoves(
    struct CSC_Board*,
    struct CSC_MoveList*);

void FindStepperMoves(
    struct CSC_MoveList*,
    CSC_Bitboard,
    CSC_Bitboard,
    CSC_Bitboard*);

void FindOrthMoves(
    struct CSC_Board*,
    struct CSC_MoveList*,
    CSC_Bitboard,
    CSC_Bitboard,
    CSC_Bitboard(*)[8]);

void FindDiagMoves(
    struct CSC_Board*,
    struct CSC_MoveList*,
    CSC_Bitboard,
    CSC_Bitboard,
    CSC_Bitboard(*)[8]);

void AddMoves(
    int,
    struct CSC_MoveList*,
    CSC_Bitboard);

void AddPawnMoves(
    CSC_Bitboard,
    struct CSC_MoveList*,
    int,
    enum CSC_MoveType);

void AddPromoMoves(
    CSC_Bitboard,
    struct CSC_MoveList*,
    int);

#endif // __MOVE_GEN_H__
