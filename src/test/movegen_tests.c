#include "movegen_tests.h"
#include "minunit.h"
#include "stdio.h"

char* movegen_test(const char* fen, int expected)
{
    printf("FEN: %s\n", fen);
    printf("Expected: %d\n", expected);

    board* b = board_from_fen(fen);
    print_board(b);
    move_list* l = get_moves(b, ALL);
    free_board(b);

    printf("Num moves generated: %d\n", l->n);
    for (int i = 0; i < l->n; i++) print_move(l->moves[i]);

    mu_assert("Wrong number of moves generated.", l->n == expected);

    free_move_list(l);

    return NULL;
}

char* movegen_test1()
{
    return movegen_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20);
}

char* all_movegen_tests()
{
    mu_run_test(movegen_test1);
    return NULL;
}
