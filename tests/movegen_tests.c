#include "chessic.h"
#include "movegen_tests.h"
#include "minunit.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

char* MoveGenTest(const char* fen, int expected)
{
    struct CSC_Board* b = CSC_BoardFromFEN(fen);
    struct CSC_MoveList* l = CSC_MakeMoveList();
    CSC_Move move;
    char* buf;
    int i;

    CSC_GetMoves(b, l, CSC_ALL);

    printf("FEN: %s\n", fen);
    printf("Expected: %d\n", expected);

    CSC_PrintBoard(b);

    printf("Num moves generated: %d\n", l->n);
    buf = malloc(CSC_MAX_UCI_MOVE_LENGTH*sizeof(char));
    for (i = 0; i < l->n; i++)
    {
        memset(buf, 0, CSC_MAX_UCI_MOVE_LENGTH*sizeof(char));
        CSC_MoveToUCIString(l->moves[i], buf, NULL);
        printf("Move: %s\n", buf);

        /* Convert the string back into a move and check it hasn't changed. */
        move = CSC_MoveFromUCIString(b, buf);
        mu_assert("Move parsing failed.", l->moves[i] == move);
    }

    free(buf);

    mu_assert("Wrong number of moves generated.", l->n == expected);

    CSC_FreeMoveList(l);
    CSC_FreeBoard(b);

    return NULL;
}

char* MoveGenTest1()
{
    return MoveGenTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20);
}

char* MoveGenTest2()
{
    /*
    ........
    ..p.....
    ...p....
    KP.....r
    .R...p.k
    ........
    ....P.P.
    ........
    */
    return MoveGenTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 14);
}

char* MoveGenTest3()
{
    return MoveGenTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48);
}

char* MoveGenTest4()
{
    /*
    r...kr..
    ........
    ........
    ........
    ........
    ........
    ..p.....
    R...K..R
    */
    return MoveGenTest("r3kr2/8/8/8/8/8/2p5/R3K2R w KQkq - - 0 1", 21);
}

char* MoveGenTest5()
{
    return MoveGenTest("rnbqkbnr/ppp1pppp/8/2PpP3/8/8/PP1P1PPP/RNBQKBNR w KQkq d6 0 1", 34);
}

/* In this test I use the starting position and repeatedly move knights until the game should be drawn by repetition. In this situation no moves should
   be generated. */
char* MoveGenTestDrawByRepetition1()
{
    struct CSC_Board* b = CSC_BoardFromFEN(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    struct CSC_MoveList* l = CSC_MakeMoveList();
    CSC_Move whiteKnightForward, blackKnightForward;
    CSC_Move whiteKnightBack, blackKnightBack;

    int turn;

    for (turn = 0; turn < 3; turn++)
    {
        whiteKnightForward = CSC_MoveFromUCIString(b, "g1f3");
        CSC_MakeMove(b, whiteKnightForward);

        blackKnightForward = CSC_MoveFromUCIString(b, "g8f6");
        CSC_MakeMove(b, blackKnightForward);

        whiteKnightBack = CSC_MoveFromUCIString(b, "f3g1");
        CSC_MakeMove(b, whiteKnightBack);

        blackKnightBack = CSC_MoveFromUCIString(b, "f6g8");
        CSC_MakeMove(b, blackKnightBack);
    }

    /* Now when the white knight moves out again the game should be drawn. */
    whiteKnightForward = CSC_MoveFromUCIString(b, "g1f3");
    CSC_MakeMove(b, whiteKnightForward);

    CSC_GetMoves(b, l, CSC_ALL);
    mu_assert("No moves should be available because the game is drawn", l->n == 0);

    CSC_FreeMoveList(l);
    CSC_FreeBoard(b);

    return NULL;
}

char* AllMoveGenTests()
{
    mu_run_test(MoveGenTest1);
    mu_run_test(MoveGenTest2);
    mu_run_test(MoveGenTest3);
    mu_run_test(MoveGenTest4);
    mu_run_test(MoveGenTest5);
    mu_run_test(MoveGenTestDrawByRepetition1);
    return NULL;
}
