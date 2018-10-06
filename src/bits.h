#ifndef __BITS_H__
#define __BITS_H__

#include "../include/chessic.h"

// This file contains all of the bit board constants that get pre-calculated.

typedef enum
{
    N, S, W, E, NE, NW, SE,SW 
} DIRECTIONS;

typedef enum
{
    ORTH, DIAG
} RAY_TYPE;

extern bb RANKS[8];
extern bb FILES[8];
extern bb KNIGHT_ATTACKS[64];
extern bb KING_ATTACKS[64];
extern bb RAY_ATTACKS[64][8];
extern bb RAY_ATTACKS_ALL[64][2];

void init_steppers();
void init_rays();

#endif // __BITS_H__
