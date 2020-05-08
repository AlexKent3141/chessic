#include "bits.h"
#include "stdio.h"

BB Ranks[8];
BB Files[8];
BB KnightAttacks[64];
BB KingAttacks[64];
BB RayAttacks[64][8];
BB RayAttacksAll[64][2];

void InitBits()
{
    Ranks[0] = 0xFF;
    for (int i = 1; i < 8; i++) Ranks[i] = Ranks[i-1] << 8;

    Files[0] = 0x0101010101010101;
    for (int i = 1; i < 8; i++) Files[i] = Files[i-1] << 1;

    InitSteppers();
    InitRays();
}

void InitSteppers()
{
    /* Knight steps. */
    int nrd[8] = { 2, 2, 1, 1, -1, -1, -2, -2 };
    int nfd[8] = { 1, -1, 2, -2, 2, -2, 1, -1 };

    /* King steps. */
    int krd[8] = { 1, 1, 1, 0, 0, -1, -1, -1 };
    int kfd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    int r1, f1;
    for (int r = 0; r < 8; r++)
    {
        for (int f = 0; f < 8; f++)
        {
            BB nc = 0;
            BB kc = 0;
            for (int d = 0; d < 8; d++)
            {
                r1 = r + nrd[d];
                f1 = f + nfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) nc |= (BB)1 << (8*r1+f1);

                r1 = r + krd[d];
                f1 = f + kfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) kc |= (BB)1 << (8*r1+f1);
            }

            KnightAttacks[8*r+f] = nc;
            KingAttacks[8*r+f] = kc;
        }
    }
}

/* Initialise the rays.
   The first 4 are orthogonals and the latter 4 are diagonals.
   The first 2 of each set are positive rays and the latter 2 are negative. */
void InitRays()
{
    /* North: start from A1. */
    BB fileStart = 0x0101010101010100;
    for (int f = 0; f < 8; f++)
    {
        BB c = fileStart;
        for (int r = 0; r < 8; r++, c <<= 8) RayAttacks[8*r+f][N] = c;
        fileStart <<= 1;
    }

    /* South: start from H8. */
    fileStart = 0x0080808080808080;
    for (int f = 7; f >= 0; f--)
    {
        BB c = fileStart;
        for (int r = 7; r >= 0; r--, c >>= 8) RayAttacks[8*r+f][S] = c;
        fileStart >>= 1;
    }

    /* East: start from A1. */
    BB rankStart = 0xFE;
    for (int r = 0; r < 8; r++)
    {
        BB c = rankStart;
        for (int f = 0; f < 8; f++, c = (c & ~Files[7]) << 1) RayAttacks[8*r+f][E] = c;
        rankStart <<= 8;
    }

    /* West: start from H1. */
    rankStart = 0x7F;
    for (int r = 0; r < 8; r++)
    {
        BB c = rankStart;
        for (int f = 7; f >= 0; f--, c = (c & ~Files[0]) >> 1) RayAttacks[8*r+f][W] = c;
        rankStart <<= 8;
    }

    /* North-east: start from A1. */
    BB diagStart = 0x8040201008040200;
    for (int f = 0; f < 8; f++)
    {
        BB c = diagStart;
        for (int r = 0; r < 8; r++, c = (c & ~Ranks[7]) << 8) RayAttacks[8*r+f][NE] = c;
        diagStart = (diagStart & ~Files[7]) << 1;
    }

    // North-west: start from H1.
    diagStart = 0x102040810204000;
    for (int f = 7; f >= 0; f--)
    {
        BB c = diagStart;
        for (int r = 0; r < 8; r++, c = (c & ~Ranks[7]) << 8) RayAttacks[8*r+f][NW] = c;
        diagStart = (diagStart & ~Files[0]) >> 1;
    }

    // South-east: start from A8.
    diagStart = 0x2040810204080;
    for (int f = 0; f < 8; f++)
    {
        BB c = diagStart;
        for (int r = 7; r >= 0; r--, c = (c & ~Ranks[0]) >> 8) RayAttacks[8*r+f][SE] = c;
        diagStart = (diagStart & ~Files[7]) << 1;
    }

    // South-west: start from H8.
    diagStart = 0x40201008040201;
    for (int f = 7; f >= 0; f--)
    {
        BB c = diagStart;
        for (int r = 7; r >= 0; r--, c = (c & ~Ranks[0]) >> 8) RayAttacks[8*r+f][SW] = c;
        diagStart = (diagStart & ~Files[0]) >> 1;
    }

    // Make the combined orthogonal and diagonal rays.
    for (int i = 0; i < 64; i++)
    {
        RayAttacksAll[i][ORTH] =
            RayAttacks[i][N] | RayAttacks[i][E] | RayAttacks[i][S] | RayAttacks[i][W];

        RayAttacksAll[i][DIAG] =
            RayAttacks[i][NE] | RayAttacks[i][NW] | RayAttacks[i][SE] | RayAttacks[i][SW];
    }
}

int PopLSB(BB* board)
{
    int b = __builtin_ctzll(*board);
    *board &= ~((BB)1 << b);
    return b;
}

int PopMSB(BB* board)
{
    int b = 63 - __builtin_clzll(*board);
    *board &= ~((BB)1 << b);
    return b;
}

int LSB(BB board)
{
    return __builtin_ctzll(board);
}

int MSB(BB board)
{
    return 63 - __builtin_clzll(board);
}

bool Test(BB board, int loc)
{
    return board & ((BB)1 << loc);
}

void PrintBB(BB mask)
{
    for (int r = 7; r >= 0; r--)
    {
        BB c = (BB)1 << 8*r;
        for (int f = 0; f < 8; f++, c <<= 1) putchar((c & mask) ? '1' : '0');
        putchar('\n');
    }

    putchar('\n');
}
