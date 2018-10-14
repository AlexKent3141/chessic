#include "perft_tests.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int num_test_cases, current_test;
test_case* test_cases;

// Load the test cases from the epd file in the test directory.
void load_test_cases()
{
    const int MAX_TEST_CASES = 1000;
    test_cases = malloc(MAX_TEST_CASES*sizeof(test_case));
    num_test_cases = 0;

    const size_t MAX_LINE_LENGTH = 255;
    char line[MAX_LINE_LENGTH];
    char fen[MAX_FEN_LENGTH];

    FILE* f = fopen("src/test/perftsuite.epd", "r");
    if (f != NULL)
    {
        while (fgets(line, MAX_LINE_LENGTH, f))
        {
            char* token = strtok(line, ";");
            memset(fen, 0, MAX_FEN_LENGTH*sizeof(char));
            memcpy(fen, token, strlen(token)*sizeof(char));

            while ((token = strtok(NULL, ";")) != NULL)
            {
                test_case test = {};
                test.depth = token[1] - '0';
                test.expected = atoi(&token[3]);
                memcpy(test.fen, fen, MAX_FEN_LENGTH*sizeof(char));

                test_cases[num_test_cases++] = test;
            }
        }
    }
    else
    {
        printf("Could not find perft test file\n");
    }
}

int perft(board* b, int depth)
{
    if (depth == 0) return 1;

    int nodes = 0;
    move_list* l = get_moves(b, ALL);
    for (int i = 0; i < l->n; i++)
    {
        move m = l->moves[i];
        if (make_move(b, m))
        {
            nodes += perft(b, depth-1);
            undo_move(b);
        }
    }

    free_move_list(l);

    return nodes;
}

char* perft_test()
{
    test_case test = test_cases[current_test];
    printf("Position %s, Depth %d, Expected %d\n", test.fen, test.depth, test.expected);

    board* b = board_from_fen(test.fen);
    int nodes = perft(b, test.depth);
    printf("Got: %d\n", nodes);
    mu_assert("Incorrect perft value", nodes == test.expected);
    free_board(b);
    return NULL;
}

char* all_perft_tests()
{
    load_test_cases();

    for (current_test = 0; current_test < num_test_cases; current_test++)
    {
        mu_run_test(perft_test);
    }

    free(test_cases);
    return NULL;
}
