#include "parser_tests.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char* fen_serial_test(const char* fen)
{
    printf("FEN: %s\n", fen);
    board* b = board_from_fen(fen);
    print_board(b);
    char* out = fen_from_board(b);
    free_board(b);
    printf("Output: %s\n", out);

    mu_assert("Mismatch detected in FEN output", strcmp(fen, out) == 0);

    free(out);

    return NULL;
}

char* fen_test1()
{
    return fen_serial_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* fen_test2()
{
    return fen_serial_test("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
}

char* fen_test3()
{
    return fen_serial_test("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
}

char* fen_test4()
{
    return fen_serial_test("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
}

char* fen_test5()
{
    return fen_serial_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
}

char* fen_test6()
{
    return fen_serial_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
}

char* fen_test7()
{
    return fen_serial_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 21 100");
}

char* all_parser_tests()
{
    mu_run_test(fen_test1);
    mu_run_test(fen_test2);
    mu_run_test(fen_test3);
    mu_run_test(fen_test4);
    mu_run_test(fen_test5);
    mu_run_test(fen_test6);
    mu_run_test(fen_test7);

    return NULL;
}
