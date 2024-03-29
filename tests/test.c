#include "chessic.h"
#include "parser_tests.h"
#include "movegen_tests.h"
#include "make_undo_tests.h"
#include "perft_tests.h"
#include "uci_tests.h"
#include "token_tests.h"
#include "stdio.h"

/* This corresponds to the variable in min_unit. */
int tests_run = 0;

bool RunTests(char* (*tests)())
{
    char* res = tests();
    if (res) printf("%s\n", res);
    return res == NULL;
}

int main()
{
    bool pass;

    CSC_InitBits();
    CSC_InitZobrist();

    pass = RunTests(AllTokenTests)
        && RunTests(AllParserTests)
        && RunTests(AllMoveGenTests)
        && RunTests(AllMakeUndoTests)
        && RunTests(AllUCITests)
        && RunTests(AllPerftTests);

    if (pass) printf("ALL TESTS PASSED\n");
    printf("Tests run: %d\n", tests_run);

    return !pass;
}
