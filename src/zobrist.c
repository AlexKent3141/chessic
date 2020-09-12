#include "../include/chessic.h"
#include "zobrist.h"

uint64_t xorshift128plus(uint64_t s[2])
{
    uint64_t x = s[0];
    uint64_t y = s[1];
    s[0] = y;
    x ^= x << 23;
    s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
    return s[1] + y;
}

void CSC_InitZobrist()
{
    int p, pt, sq, ct, f;
    uint64_t seed[2] = { 0xDEADBEEF, 0x8BADF00D };

    for (p = 0; p < 2; p++)
    {
        for (pt = 0; pt < 7; pt++)
        {
            for (sq = 0; sq < CSC_SQUARE_NB; sq++)
            {
                keys.pieceSquare[p][pt][sq] = xorshift128plus(seed);
            }
        }

        for (ct = 0; ct < 2; ct++)
        {
            keys.castling[p][ct] = xorshift128plus(seed);
        }
    }

    for (f = 0; f < CSC_FILE_NB; f++)
    {
        keys.enpassentFile[f] = xorshift128plus(seed);
    }

    keys.side = xorshift128plus(seed);
}

