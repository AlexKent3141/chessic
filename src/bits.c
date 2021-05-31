#include "chessic.h"
#include "stdio.h"

#ifdef _MSC_VER
#include "intrin.h"
#endif

CSC_Bitboard CSC_Ranks[8];
CSC_Bitboard CSC_Files[8];
CSC_Bitboard CSC_KnightAttacks[64];
CSC_Bitboard CSC_KingAttacks[64];
CSC_Bitboard CSC_RayAttacks[64][8];
CSC_Bitboard CSC_RayAttacksAll[64][2];

void InitSteppers()
{
    /* Knight steps. */
    int nrd[8] = { 2, 2, 1, 1, -1, -1, -2, -2 };
    int nfd[8] = { 1, -1, 2, -2, 2, -2, 1, -1 };

    /* King steps. */
    int krd[8] = { 1, 1, 1, 0, 0, -1, -1, -1 };
    int kfd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    int r, f, r1, f1, d;
    for (r = 0; r < 8; r++)
    {
        for (f = 0; f < 8; f++)
        {
            CSC_Bitboard nc = 0;
            CSC_Bitboard kc = 0;
            for (d = 0; d < 8; d++)
            {
                r1 = r + nrd[d];
                f1 = f + nfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) nc |= (CSC_Bitboard)1 << (8*r1+f1);

                r1 = r + krd[d];
                f1 = f + kfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) kc |= (CSC_Bitboard)1 << (8*r1+f1);
            }

            CSC_KnightAttacks[8*r+f] = nc;
            CSC_KingAttacks[8*r+f] = kc;
        }
    }
}

/* Initialise the rays.
   The first 4 are orthogonals and the latter 4 are diagonals.
   The first 2 of each set are positive rays and the latter 2 are negative. */
void InitRays()
{
    int r, f, i;
    CSC_Bitboard fileStart, rankStart, diagStart, c;

    /* North: start from A1. */
    fileStart = 0x0101010101010100;
    for (f = 0; f < 8; f++)
    {
        c = fileStart;
        for (r = 0; r < 8; r++, c <<= 8)
        {
            CSC_RayAttacks[8*r+f][CSC_NORTH] = c;
        }

        fileStart <<= 1;
    }

    /* South: start from H8. */
    fileStart = 0x0080808080808080;
    for (f = 7; f >= 0; f--)
    {
        c = fileStart;
        for (r = 7; r >= 0; r--, c >>= 8)
        {
            CSC_RayAttacks[8*r+f][CSC_SOUTH] = c;
        }

        fileStart >>= 1;
    }

    /* East: start from A1. */
    rankStart = 0xFE;
    for (r = 0; r < 8; r++)
    {
        c = rankStart;
        for (f = 0; f < 8; f++, c = (c & ~CSC_Files[7]) << 1)
        {
            CSC_RayAttacks[8*r+f][CSC_EAST] = c;
        }

        rankStart <<= 8;
    }

    /* West: start from H1. */
    rankStart = 0x7F;
    for (r = 0; r < 8; r++)
    {
        c = rankStart;
        for (f = 7; f >= 0; f--, c = (c & ~CSC_Files[0]) >> 1)
        {
            CSC_RayAttacks[8*r+f][CSC_WEST] = c;
        }

        rankStart <<= 8;
    }

    /* North-east: start from A1. */
    diagStart = 0x8040201008040200;
    for (f = 0; f < 8; f++)
    {
        c = diagStart;
        for (r = 0; r < 8; r++, c = (c & ~CSC_Ranks[7]) << 8)
        {
            CSC_RayAttacks[8*r+f][CSC_NORTHEAST] = c;
        }

        diagStart = (diagStart & ~CSC_Files[7]) << 1;
    }

    /* North-west: start from H1. */
    diagStart = 0x102040810204000;
    for (f = 7; f >= 0; f--)
    {
        c = diagStart;
        for (r = 0; r < 8; r++, c = (c & ~CSC_Ranks[7]) << 8)
        {
            CSC_RayAttacks[8*r+f][CSC_NORTHWEST] = c;
        }

        diagStart = (diagStart & ~CSC_Files[0]) >> 1;
    }

    /* South-east: start from A8. */
    diagStart = 0x2040810204080;
    for (f = 0; f < 8; f++)
    {
        c = diagStart;
        for (r = 7; r >= 0; r--, c = (c & ~CSC_Ranks[0]) >> 8)
        {
            CSC_RayAttacks[8*r+f][CSC_SOUTHEAST] = c;
        }

        diagStart = (diagStart & ~CSC_Files[7]) << 1;
    }

    /* South-west: start from H8. */
    diagStart = 0x40201008040201;
    for (f = 7; f >= 0; f--)
    {
        c = diagStart;
        for (r = 7; r >= 0; r--, c = (c & ~CSC_Ranks[0]) >> 8)
        {
            CSC_RayAttacks[8*r+f][CSC_SOUTHWEST] = c;
        }

        diagStart = (diagStart & ~CSC_Files[0]) >> 1;
    }

    /* Make the combined orthogonal and diagonal rays. */
    for (i = 0; i < 64; i++)
    {
        CSC_RayAttacksAll[i][CSC_ORTHOGONAL] =
            CSC_RayAttacks[i][CSC_NORTH]
          | CSC_RayAttacks[i][CSC_EAST]
          | CSC_RayAttacks[i][CSC_SOUTH]
          | CSC_RayAttacks[i][CSC_WEST];

        CSC_RayAttacksAll[i][CSC_DIAGONAL] =
            CSC_RayAttacks[i][CSC_NORTHEAST]
          | CSC_RayAttacks[i][CSC_NORTHWEST]
          | CSC_RayAttacks[i][CSC_SOUTHEAST]
          | CSC_RayAttacks[i][CSC_SOUTHWEST];
    }
}

void CSC_InitBits()
{
    int i;
    CSC_Ranks[0] = 0xFF;
    for (i = 1; i < 8; i++) CSC_Ranks[i] = CSC_Ranks[i-1] << 8;

    CSC_Files[0] = 0x0101010101010101;
    for (i = 1; i < 8; i++) CSC_Files[i] = CSC_Files[i-1] << 1;

    InitSteppers();
    InitRays();
}

int CSC_PopLSB(CSC_Bitboard* board)
{
    int b = CSC_LSB(*board);
    *board &= *board - 1;
    return b;
}

int CSC_PopMSB(CSC_Bitboard* board)
{
    int b = CSC_MSB(*board);
    *board &= ~((CSC_Bitboard)1 << b);
    return b;
}

int CSC_LSB(CSC_Bitboard board)
{
    int bit;
#ifdef _MSC_VER
    _BitScanForward64(&bit, board);
#else
    bit = __builtin_ffsll(board) - 1;
#endif
    return bit;
}

int CSC_MSB(CSC_Bitboard board)
{
    int bit;
#ifdef _MSC_VER
    _BitScanReverse64(&bit, board);
#else
    bit = 63 - __builtin_clzll(board);
#endif
    return bit;
}

bool CSC_Test(CSC_Bitboard board, int loc)
{
    return board & ((CSC_Bitboard)1 << loc);
}

void CSC_PrintBitboard(CSC_Bitboard mask)
{
    int r, f;
    CSC_Bitboard c;
    for (r = 7; r >= 0; r--)
    {
        c = (CSC_Bitboard)1 << 8*r;
        for (f = 0; f < 8; f++, c <<= 1) putchar((c & mask) ? '1' : '0');
        putchar('\n');
    }

    putchar('\n');
}
