#ifndef __BITS_H__
#define __BITS_H__

#include "../include/chessic.h"

/* This file contains all of the bit board constants that get pre-calculated. */

typedef enum
{
    N, S, W, E, NE, NW, SE, SW 
} Directions;

typedef enum
{
    ORTH, DIAG
} RayType;

extern CSC_Bitboard Ranks[8];
extern CSC_Bitboard Files[8];
extern CSC_Bitboard KnightAttacks[64];
extern CSC_Bitboard KingAttacks[64];
extern CSC_Bitboard RayAttacks[64][8];
extern CSC_Bitboard RayAttacksAll[64][2];

void InitSteppers();
void InitRays();

#endif /* __BITS_H__ */
