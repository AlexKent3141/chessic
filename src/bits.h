#ifndef __BITS_H__
#define __BITS_H__

#include "../include/chessic.h"

// This file contains all of the bit board constants that get pre-calculated.

typedef enum
{
    N, S, W, E, NE, NW, SE,SW 
} DIRECTIONS;

extern bb RANKS[8];
extern bb FILES[8];
extern bb RAY_ATTACKS[64][8];

void init_rays();

#endif // __BITS_H__
