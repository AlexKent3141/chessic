#ifndef __PERFT_TESTS_H__
#define __PERFT_TESTS_H__

#include "../../include/chessic.h"

typedef struct
{
    char fen[MAX_FEN_LENGTH];
    int depth;
    int expected;
} test_case;

void load_test_cases();

int perft(board*, int);

char* perft_test();

char* all_perft_tests();

#endif //  __PERFT_TESTS_H__
