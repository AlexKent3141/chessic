#include "bits.h"
#include "stdio.h"

bb RANKS[8];
bb FILES[8];
bb KNIGHT_ATTACKS[64];
bb KING_ATTACKS[64];
bb RAY_ATTACKS[64][8];
bb RAY_ATTACKS_ALL[64][2];

void init_bits()
{
    RANKS[0] = 0xFF;
    for (int i = 1; i < 8; i++) RANKS[i] = RANKS[i-1] << 8;

    FILES[0] = 0x0101010101010101;
    for (int i = 1; i < 8; i++) FILES[i] = FILES[i-1] << 1;

    init_steppers();
    init_rays();
}

void init_steppers()
{
    // Knights.
    int nrd[8] = { 2, 2, 1, 1, -1, -1, -2, -2 };
    int nfd[8] = { 1, -1, 2, -2, 2, -2, 1, -1 };

    // Kings.
    int krd[8] = { 1, 1, 1, 0, 0, -1, -1, -1 };
    int kfd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    int r1, f1;
    for (int r = 0; r < 8; r++)
    {
        for (int f = 0; f < 8; f++)
        {
            bb nc = 0;
            bb kc = 0;
            for (int d = 0; d < 8; d++)
            {
                r1 = r + nrd[d];
                f1 = f + nfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) nc |= (bb)1 << (8*r1+f1);

                r1 = r + krd[d];
                f1 = f + kfd[d];
                if (r1 > -1 && r1 < 8 && f1 > -1 && f1 < 8) kc |= (bb)1 << (8*r1+f1);
            }

            KNIGHT_ATTACKS[8*r+f] = nc;
            KING_ATTACKS[8*r+f] = kc;
        }
    }
}

// Initialise the rays.
// The first 4 are orthogonals and the latter 4 are diagonals.
// The first 2 of each set are positive rays and the latter 2 are negative.
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

    // Make the combined orthogonal and diagonal rays.
    for (int i = 0; i < 64; i++)
    {
        RAY_ATTACKS_ALL[i][ORTH] =
            RAY_ATTACKS[i][N] | RAY_ATTACKS[i][E] | RAY_ATTACKS[i][S] | RAY_ATTACKS[i][W];

        RAY_ATTACKS_ALL[i][DIAG] =
            RAY_ATTACKS[i][NE] | RAY_ATTACKS[i][NW] | RAY_ATTACKS[i][SE] | RAY_ATTACKS[i][SW];
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

int lsb(bb board)
{
    return __builtin_ctzll(board);
}

int msb(bb board)
{
    return 63 - __builtin_clzll(board);
}

bool test(bb board, int loc)
{
    return board & ((bb)1 << loc);
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
