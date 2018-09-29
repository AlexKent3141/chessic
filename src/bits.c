#include "bits.h"
#include "stdio.h"

bb RANKS[8];
bb FILES[8];
bb RAY_ATTACKS[64][8];

void init_bits()
{
    RANKS[0] = 0xFF;
    for (int i = 1; i < 8; i++) RANKS[i] = RANKS[i-1] << 8;

    FILES[0] = 0x0101010101010101;
    for (int i = 1; i < 8; i++) FILES[i] = FILES[i-1] << 1;

    init_rays();
}

void init_rays()
{
    // North: start from A1.
    bb file_start = 0x0101010101010100;
    for (int f = 0; f < 8; f++)
    {
        bb c = file_start;
        for (int r = 0; r < 8; r++, c <<= 8) RAY_ATTACKS[8*r+f][N] = c;
        file_start <<= 1;
    }

    // South: start from H8.
    file_start = 0x0080808080808080;
    for (int f = 7; f >= 0; f--)
    {
        bb c = file_start;
        for (int r = 7; r >= 0; r--, c >>= 8) RAY_ATTACKS[8*r+f][S] = c;
        file_start >>= 1;
    }

    // East: start from A1.
    bb rank_start = 0xFE;
    for (int r = 0; r < 8; r++)
    {
        bb c = rank_start;
        for (int f = 0; f < 8; f++, c = (c & ~FILES[7]) << 1) RAY_ATTACKS[8*r+f][E] = c;
        rank_start <<= 8;
    }

    // West: start from H1.
    rank_start = 0x7F;
    for (int r = 0; r < 8; r++)
    {
        bb c = rank_start;
        for (int f = 7; f >= 0; f--, c = (c & ~FILES[0]) >> 1) RAY_ATTACKS[8*r+f][W] = c;
        rank_start <<= 8;
    }

    // North-east: start from A1.
    bb diag_start = 0x8040201008040200;
    for (int f = 0; f < 8; f++)
    {
        bb c = diag_start;
        for (int r = 0; r < 8; r++, c = (c & ~RANKS[7]) << 8) RAY_ATTACKS[8*r+f][NE] = c;
        diag_start = (diag_start & ~FILES[7]) << 1;
    }

    // North-west: start from H1.
    diag_start = 0x102040810204000;
    for (int f = 7; f >= 0; f--)
    {
        bb c = diag_start;
        for (int r = 0; r < 8; r++, c = (c & ~RANKS[7]) << 8) RAY_ATTACKS[8*r+f][NW] = c;
        diag_start = (diag_start & ~FILES[0]) >> 1;
    }

    // South-east: start from A8.
    diag_start = 0x2040810204080;
    for (int f = 0; f < 8; f++)
    {
        bb c = diag_start;
        for (int r = 7; r >= 0; r--, c = (c & ~RANKS[0]) >> 8) RAY_ATTACKS[8*r+f][SE] = c;
        diag_start = (diag_start & ~FILES[7]) << 1;
    }

    // South-west: start from H8.
    diag_start = 0x40201008040201;
    for (int f = 7; f >= 0; f--)
    {
        bb c = diag_start;
        for (int r = 7; r >= 0; r--, c = (c & ~RANKS[0]) >> 8) RAY_ATTACKS[8*r+f][SW] = c;
        diag_start = (diag_start & ~FILES[0]) >> 1;
    }
}

int pop_lsb(bb* board)
{
    int b = __builtin_ctzll(*board);
    *board &= ~((bb)1 << b);
    return b;
}

int pop_msb(bb* board)
{
    int b = 63 - __builtin_clzll(*board);
    *board &= ~((bb)1 << b);
    return b;
}

void print_bb(bb mask)
{
    for (int r = 7; r >= 0; r--)
    {
        bb c = (bb)1 << 8*r;
        for (int f = 0; f < 8; f++, c <<= 1) putchar((c & mask) ? '1' : '0');
        putchar('\n');
    }

    putchar('\n');
}
