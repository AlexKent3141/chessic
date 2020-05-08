#include "perft_tests.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int numTestCases, currentTest;
struct TestCase* testCases;

// Load the test cases from the epd file in the test directory.
void LoadTestCases()
{
    const int MAX_TEST_CASES = 1000;
    testCases = malloc(MAX_TEST_CASES*sizeof(struct TestCase));
    numTestCases = 0;

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
                struct TestCase test = { 0 };
                test.depth = token[1] - '0';
                test.expected = atoi(&token[3]);
                memcpy(test.fen, fen, MAX_FEN_LENGTH*sizeof(char));

                testCases[numTestCases++] = test;
            }
        }
    }
    else
    {
        printf("Could not find perft test file\n");
    }
}

int Perft(struct Board* b, int depth)
{
    if (depth == 0) return 1;

    int nodes = 0;
    struct MoveList* l = GetMoves(b, ALL);
    for (int i = 0; i < l->n; i++)
    {
        Move m = l->moves[i];
        if (MakeMove(b, m))
        {
            nodes += Perft(b, depth-1);
            UndoMove(b);
        }
    }

    FreeMoveList(l);

    return nodes;
}

char* PerftTest()
{
    struct TestCase test = testCases[currentTest];
    printf("Position %s, Depth %d, Expected %d\n",
        test.fen,
        test.depth,
        test.expected);

    struct Board* b = BoardFromFEN(test.fen);
    int nodes = Perft(b, test.depth);
    printf("Got: %d\n", nodes);
    mu_assert("Incorrect perft value", nodes == test.expected);
    FreeBoard(b);
    return NULL;
}

char* AllPerftTests()
{
    LoadTestCases();

    for (currentTest = 0; currentTest < numTestCases; currentTest++)
    {
        mu_run_test(PerftTest);
    }

    free(testCases);
    return NULL;
}
