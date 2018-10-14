#include "parser_tests.h"
#include "movegen_tests.h"
#include "make_undo_tests.h"
#include "perft_tests.h"
#include "stdio.h"

int tests_run = 0;

bool run_tests(char* (*tests)())
{
    char* res = tests();
    if (res) printf("%s\n", res);
    return res == NULL;
}

int main()
{
    init_bits();

    bool pass = run_tests(all_parser_tests)
             && run_tests(all_movegen_tests)
             && run_tests(all_make_undo_tests)
             && run_tests(all_perft_tests);

    if (pass) printf("ALL TESTS PASSED\n");
    printf("Tests run: %d\n", tests_run);

    return !pass;
}
