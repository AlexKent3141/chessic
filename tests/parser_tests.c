#include "../include/chessic.h"
#include "parser_tests.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char* FENTest(const char* fen)
{
    struct CSC_Board* b = CSC_BoardFromFEN(fen);
    char* buf;

    printf("FEN: %s\n", fen);
    CSC_PrintBoard(b);

    buf = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    CSC_FENFromBoard(b, buf, NULL);
    CSC_FreeBoard(b);
    printf("Output: %s\n", buf);

    mu_assert("Mismatch detected in FEN output", strcmp(fen, buf) == 0);

    free(buf);

    return NULL;
}

char* FENTest1()
{
    return FENTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* FENTest2()
{
    return FENTest("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
}

char* FENTest3()
{
    return FENTest("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
}

char* FENTest4()
{
    return FENTest("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
}

char* FENTest5()
{
    return FENTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
}

char* FENTest6()
{
    return FENTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
}

char* FENTest7()
{
    return FENTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 21 100");
}

char* AllParserTests()
{
    mu_run_test(FENTest1);
    mu_run_test(FENTest2);
    mu_run_test(FENTest3);
    mu_run_test(FENTest4);
    mu_run_test(FENTest5);
    mu_run_test(FENTest6);
    mu_run_test(FENTest7);

    return NULL;
}
