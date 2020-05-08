#include "make_undo_tests.h"
#include "minunit.h"
#include "stdio.h"

char* MakeUndoTest(const char* fen)
{
    printf("FEN: %s\n", fen);

    struct Board* b = BoardFromFEN(fen);
    struct MoveList* l = GetMoves(b, ALL);

    for (int i = 0; i < l->n; i++)
    {
        Move m = l->moves[i];
        PrintMove(m);

        struct Board* initial = CopyBoard(b);
        if (MakeMove(b, m))
        {
            UndoMove(b);
        }

        if (!BoardEqual(b, initial))
        {
            printf("Initial: \n");
            PrintBoard(initial);
            printf("After move: \n");
            PrintBoard(b);

            mu_assert("Boards do not match.", false);
        }
    }

    FreeMoveList(l);
    FreeBoard(b);

    return NULL;
}

char* MakeUndoTest1()
{
    return MakeUndoTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* MakeUndoTest2()
{
    return MakeUndoTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
}

char* MakeUndoTest3()
{
    return MakeUndoTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
}

char* AllMakeUndoTests()
{
    mu_run_test(MakeUndoTest1);
    mu_run_test(MakeUndoTest2);
    mu_run_test(MakeUndoTest3);
    return NULL;
}
