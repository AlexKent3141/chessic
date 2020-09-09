#include "../../include/chessic.h"
#include "perft_tests.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct TestCase
{
    char fen[CSC_MAX_FEN_LENGTH];
    int depth;
    int expected;
};

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
    char fen[CSC_MAX_FEN_LENGTH];

    FILE* f = fopen("src/test/perftsuite.epd", "r");
    if (f != NULL)
    {
        while (fgets(line, MAX_LINE_LENGTH, f))
        {
            char* token = strtok(line, ";");
            memset(fen, 0, CSC_MAX_FEN_LENGTH*sizeof(char));
            memcpy(fen, token, strlen(token)*sizeof(char));

            while ((token = strtok(NULL, ";")) != NULL)
            {
                struct TestCase test = { 0 };
                test.depth = token[1] - '0';
                test.expected = atoi(&token[3]);
                memcpy(test.fen, fen, CSC_MAX_FEN_LENGTH*sizeof(char));

                testCases[numTestCases++] = test;
            }
        }

        fclose(f);
    }
    else
    {
        printf("Could not find perft test file\n");
    }
}

int Perft(struct CSC_Board* b, int depth)
{
    if (depth == 0) return 1;

    int nodes = 0;
    struct CSC_MoveList* l = CSC_GetMoves(b, CSC_ALL);
    for (int i = 0; i < l->n; i++)
    {
        CSC_Move m = l->moves[i];
        if (CSC_MakeMove(b, m))
        {
            nodes += Perft(b, depth-1);
            CSC_UndoMove(b);
        }
    }

    CSC_FreeMoveList(l);

    return nodes;
}

char* PerftTest()
{
    struct TestCase test = testCases[currentTest];
    printf("Position %s, Depth %d, Expected %d\n",
        test.fen,
        test.depth,
        test.expected);

    struct CSC_Board* b = CSC_BoardFromFEN(test.fen);
    int nodes = Perft(b, test.depth);
    printf("Got: %d\n", nodes);
    mu_assert("Incorrect perft value", nodes == test.expected);
    CSC_FreeBoard(b);
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
