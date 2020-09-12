#include "../include/chessic.h"
#include "make_undo_tests.h"
#include "minunit.h"
#include "stdio.h"

char* MakeUndoTest(const char* fen)
{
    printf("FEN: %s\n", fen);

    struct CSC_Board* b = CSC_BoardFromFEN(fen);
    struct CSC_MoveList* l = CSC_GetMoves(b, CSC_ALL);

    for (int i = 0; i < l->n; i++)
    {
        CSC_Move m = l->moves[i];
        CSC_PrintMove(m);

        struct CSC_Board* initial = CSC_CopyBoard(b);
        if (CSC_MakeMove(b, m))
        {
            CSC_UndoMove(b);
        }

        if (!CSC_BoardEqual(b, initial))
        {
            printf("Initial: \n");
            CSC_PrintBoard(initial);
            printf("After move: \n");
            CSC_PrintBoard(b);

            mu_assert("Boards do not match.", false);
        }

        CSC_FreeBoard(initial);
    }

    CSC_FreeMoveList(l);
    CSC_FreeBoard(b);

    return NULL;
}

char* MakeUndoTest1()
{
    return MakeUndoTest(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* MakeUndoTest2()
{
    return MakeUndoTest(
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
}

char* MakeUndoTest3()
{
    return MakeUndoTest(
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
}

char* MakeUndoTest4()
{
    return MakeUndoTest(
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
}

char* AllMakeUndoTests()
{
    mu_run_test(MakeUndoTest1);
    mu_run_test(MakeUndoTest2);
    mu_run_test(MakeUndoTest3);
    mu_run_test(MakeUndoTest4);
    return NULL;
}
