#include "make_undo_tests.h"
#include "minunit.h"
#include "stdio.h"

char* make_undo_test(const char* fen)
{
    printf("FEN: %s\n", fen);

    board* b = board_from_fen(fen);
    move_list* l = get_moves(b, ALL);

    for (int i = 0; i < l->n; i++)
    {
        move m = l->moves[i];
        print_move(m);

        board* initial = copy_board(b);
        if (make_move(b, m))
        {
            undo_move(b);
        }

        if (!board_equal(b, initial))
        {
            printf("Initial: \n");
            print_board(initial);
            printf("After move: \n");
            print_board(b);

            mu_assert("Boards do not match.", false);
        }
    }

    free_move_list(l);
    free_board(b);

    return NULL;
}

char* make_undo_test1()
{
    return make_undo_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* make_undo_test2()
{
    return make_undo_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
}

char* make_undo_test3()
{
    return make_undo_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
}

char* all_make_undo_tests()
{
    mu_run_test(make_undo_test1);
    mu_run_test(make_undo_test2);
    mu_run_test(make_undo_test3);
    return NULL;
}
