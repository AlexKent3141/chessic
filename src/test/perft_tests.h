#ifndef __PERFT_TESTS_H__
#define __PERFT_TESTS_H__

#include "../../include/chessic.h"

struct TestCase
{
    char fen[CSC_MAX_FEN_LENGTH];
    int depth;
    int expected;
};

void LoadTestCases();

int Perft(struct CSC_Board*, int);

char* PerftTest();

char* AllPerftTests();

#endif /* __PERFT_TESTS_H__ */
