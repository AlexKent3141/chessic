#ifndef __PERFT_TESTS_H__
#define __PERFT_TESTS_H__

#include "../../include/chessic.h"

struct TestCase
{
    char fen[MAX_FEN_LENGTH];
    int depth;
    int expected;
};

void LoadTestCases();

int Perft(struct Board*, int);

char* PerftTest();

char* AllPerftTests();

#endif /* __PERFT_TESTS_H__ */
