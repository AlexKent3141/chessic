#include "chessic.h"
#include "attackgen_tests.h"
#include "minunit.h"
#include "stdio.h"

char* AttackGenTest1()
{
    /* Check attacks for pieces in the kiwipete position. */
    const char* kiwipete =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    struct CSC_Board* board = CSC_BoardFromFEN(kiwipete);
    CSC_Bitboard attacks;
    int loc, count;

    printf("FEN: %s\n", kiwipete);
    
    /* Check the white queen's attacks. */
    loc = CSC_LSB(board->pieces[CSC_QUEEN][CSC_WHITE]);
    attacks = CSC_GetAttacks(board, loc);

    /* We check the total number of attacked locations, how many are
       ours and how many are theirs. */
    count = CSC_Count(attacks);
    printf("Num attacks: %d\n", count);
    mu_assert("Wrong number of attacks", count == 14);
    count = CSC_Count(attacks & board->all[board->player]);
    printf("Num attacks (ours): %d\n", count);
    mu_assert("Wrong number of attacks (ours)", count == 5);
    count = CSC_Count(attacks & board->all[1 - board->player]);
    printf("Num attacks (theirs): %d\n", count);
    mu_assert("Wrong number of attacks (theirs)", count == 2);

    /* Check the white pawn on d5 */
    loc = CSC_MSB(board->pieces[CSC_PAWN][CSC_WHITE]);
    attacks = CSC_GetAttacks(board, loc);

    /* We check the total number of attacked locations, how many are
       ours and how many are theirs. */
    count = CSC_Count(attacks);
    printf("Num attacks: %d\n", count);
    mu_assert("Wrong number of attacks", count == 2);
    count = CSC_Count(attacks & board->all[board->player]);
    printf("Num attacks (ours): %d\n", count);
    mu_assert("Wrong number of attacks (ours)", count == 0);
    count = CSC_Count(attacks & board->all[1 - board->player]);
    printf("Num attacks (theirs): %d\n", count);
    mu_assert("Wrong number of attacks (theirs)", count == 1);

    /* Check the black pawn on b4 (this the second lowest pawn) */
    loc = CSC_LSB(board->pieces[CSC_PAWN][CSC_BLACK]);
    loc = CSC_LSB(board->pieces[CSC_PAWN][CSC_BLACK] & ~((CSC_Bitboard)1 << loc));
    attacks = CSC_GetAttacks(board, loc);

    /* We check the total number of attacked locations, how many are
       ours and how many are theirs. */
    count = CSC_Count(attacks);
    printf("Num attacks: %d\n", count);
    mu_assert("Wrong number of attacks", count == 2);
    count = CSC_Count(attacks & board->all[board->player]);
    printf("Num attacks (ours): %d\n", count);
    mu_assert("Wrong number of attacks (ours)", count == 1);
    count = CSC_Count(attacks & board->all[1 - board->player]);
    printf("Num attacks (theirs): %d\n", count);
    mu_assert("Wrong number of attacks (theirs)", count == 0);

    CSC_FreeBoard(board);

    return NULL;
}

char* AllAttackGenTests()
{
    printf("Running attack generation tests\n");
    mu_run_test(AttackGenTest1);
    return NULL;
}
