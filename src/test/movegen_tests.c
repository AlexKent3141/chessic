#include "movegen_tests.h"
#include "minunit.h"
#include "stdio.h"

char* MoveGenTest(const char* fen, int expected)
{
    printf("FEN: %s\n", fen);
    printf("Expected: %d\n", expected);

    struct CSC_Board* b = CSC_BoardFromFEN(fen);
    CSC_PrintBoard(b);
    struct CSC_MoveList* l = CSC_GetMoves(b, CSC_ALL);
    CSC_FreeBoard(b);

    printf("Num moves generated: %d\n", l->n);
    for (int i = 0; i < l->n; i++) CSC_PrintMove(l->moves[i]);

    mu_assert("Wrong number of moves generated.", l->n == expected);

    CSC_FreeMoveList(l);

    return NULL;
}

char* MoveGenTest1()
{
    return MoveGenTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20);
}

char* MoveGenTest2()
{
    return MoveGenTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 16);
}

char* MoveGenTest3()
{
    return MoveGenTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48);
}

char* MoveGenTest4()
{
    return MoveGenTest("r3kr2/8/8/8/8/8/2p5/R3K2R w KQkq - - 0 1", 24);
}

char* MoveGenTest5()
{
    return MoveGenTest("rnbqkbnr/ppp1pppp/8/2PpP3/8/8/PP1P1PPP/RNBQKBNR w KQkq d6 0 1", 34);
}

char* AllMoveGenTests()
{
    mu_run_test(MoveGenTest1);
    mu_run_test(MoveGenTest2);
    mu_run_test(MoveGenTest3);
    mu_run_test(MoveGenTest4);
    mu_run_test(MoveGenTest5);
    return NULL;
}
