#include "../include/chessic.h"
#include "minunit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct TestFixture
{
    bool onUCICalled;
    bool onDebugCalled;
    bool onIsReadyCalled;
    bool onSetOptionNameCalled;
    bool onSetOptionNameValueCalled;
    bool onPositionCalled;
    bool onGoCalled;

    bool debug;
    char* optionName;
    char* optionValue;
    struct CSC_Board* position;

    int depth;
};

struct TestFixture fixture;
struct CSC_UCICallbacks callbacks;

void ResetFixture()
{
    fixture.onUCICalled = false;
    fixture.onDebugCalled = false;
    fixture.onIsReadyCalled = false;
    fixture.onSetOptionNameCalled = false;
    fixture.onSetOptionNameValueCalled = false;
    fixture.onPositionCalled = false;
    fixture.onGoCalled = false;

    fixture.debug = false;

    if (fixture.optionName != NULL)
    {
        free(fixture.optionName);
        fixture.optionName = NULL;
    }

    if (fixture.optionValue != NULL)
    {
        free(fixture.optionValue);
        fixture.optionValue = NULL;
    }

    if (fixture.position != NULL)
    {
        CSC_FreeBoard(fixture.position);
        fixture.position = NULL;
    }

    fixture.depth = -1;

    callbacks.onUCI = NULL;
    callbacks.onDebug = NULL;
    callbacks.onIsReady = NULL;
    callbacks.onSetOptionName = NULL;
    callbacks.onSetOptionNameValue = NULL;
    callbacks.onNewGame = NULL;
    callbacks.onPosition = NULL;
    callbacks.onGo = NULL;
    callbacks.onStop = NULL;
    callbacks.onPonderHit = NULL;
    callbacks.onQuit = NULL;
}

void dummyOnUCI()
{
    fixture.onUCICalled = true;
}

char* ProcessUCITest()
{
    printf("UCI test\n");
    ResetFixture();

    callbacks.onUCI = &dummyOnUCI;

    CSC_UCIProcess("uci", &callbacks); 
    mu_assert(
        "We should have received a 'UCI' command.",
        fixture.onUCICalled);

    return NULL;
}

void dummyOnDebug(bool debug)
{
    fixture.onDebugCalled = true;
    fixture.debug = debug;
}

char* ProcessDebugTest_On()
{
    printf("Debug on test\n");
    ResetFixture();

    callbacks.onDebug = &dummyOnDebug;

    CSC_UCIProcess("debug on", &callbacks);

    mu_assert(
        "Should have received a 'UCI' command.",
        fixture.onDebugCalled);

    mu_assert(
        "Debug should have been turned on.",
        fixture.debug);

    return NULL;
}

char* ProcessDebugTest_Off()
{
    printf("Debug off test\n");
    ResetFixture();

    callbacks.onDebug = &dummyOnDebug;

    CSC_UCIProcess("debug off", &callbacks);

    mu_assert(
        "Should have received a 'UCI' command.",
        fixture.onDebugCalled);

    mu_assert(
        "Debug should have been turned off.",
        !fixture.debug);

    return NULL;
}

void dummyOnIsReady()
{
    fixture.onIsReadyCalled = true;
}

char* ProcessIsReadyTest()
{
    printf("IsReady test\n");
    ResetFixture();

    callbacks.onIsReady = &dummyOnIsReady;

    CSC_UCIProcess("isready", &callbacks);

    mu_assert(
        "We should have received a 'isready' command.",
        fixture.onIsReadyCalled);

    return NULL;
}

void dummyOnSetOptionName(const char* name)
{
    fixture.onSetOptionNameCalled = true;
    fixture.optionName = (char*)name;
}

char* ProcessSetOptionNameTest_Valid()
{
    printf("SetOptionName valid test\n");
    ResetFixture();

    callbacks.onSetOptionName = &dummyOnSetOptionName;

    CSC_UCIProcess("setoption name pokemon", &callbacks);

    mu_assert(
        "We should have received a 'setoption' command.",
        fixture.onSetOptionNameCalled);

    mu_assert(
        "The option name should have been pokemon.",
        strcmp(fixture.optionName, "pokemon") == 0);

    return NULL;
}

char* ProcessSetOptionNameTest_NoNameSpecified()
{
    printf("SetOptionName no name specified test\n");
    ResetFixture();

    callbacks.onSetOptionName = &dummyOnSetOptionName;

    CSC_UCIProcess("setoption name", &callbacks);

    mu_assert(
        "We should not have received a 'setoption' command.",
        !fixture.onSetOptionNameCalled);

    return NULL;
}

void dummyOnSetOptionNameValue(const char* name, const char* value)
{
    fixture.onSetOptionNameValueCalled = true;
    fixture.optionName = (char*)name;
    fixture.optionValue = (char*)value;
}

char* ProcessSetOptionNameValueTest_Valid()
{
    printf("SetOptionNameValue valid test\n");
    ResetFixture();

    callbacks.onSetOptionNameValue = &dummyOnSetOptionNameValue;

    CSC_UCIProcess("setoption name pokemon value pikachu", &callbacks);

    mu_assert(
        "We should have received a 'setoption' command.",
        fixture.onSetOptionNameValueCalled);

    mu_assert(
        "The option name should have been pokemon.",
        strcmp(fixture.optionName, "pokemon") == 0);

    mu_assert(
        "The option value should have been pikachu.",
        strcmp(fixture.optionValue, "pikachu") == 0);

    return NULL;
}

char* ProcessSetOptionNameValueTest_NoValueSpecified()
{
    printf("SetOptionNameValue no value specified test\n");
    ResetFixture();

    callbacks.onSetOptionNameValue = &dummyOnSetOptionNameValue;

    CSC_UCIProcess("setoption name pokemon value", &callbacks);

    mu_assert(
        "We should not have received a 'setoption' command.",
        !fixture.onSetOptionNameCalled);

    mu_assert(
        "We should not have received a 'setoption' command.",
        !fixture.onSetOptionNameValueCalled);

    return NULL;
}

void dummyOnPosition(struct CSC_Board* position)
{
    fixture.onPositionCalled = true;
    fixture.position = position;
}

char* ProcessPositionTest_ValidStartPosNoMoves()
{
    const char* startFen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen;

    printf("Position valid startpos no moves test\n");
    ResetFixture();

    callbacks.onPosition = &dummyOnPosition;

    CSC_UCIProcess("position startpos", &callbacks);

    mu_assert(
        "We should have received a 'position' command.",
        fixture.onPositionCalled);

    mu_assert(
        "We should have received a position.",
        fixture.position != NULL);

    /* Check that the position is actually the start position by converting
       to a FEN string. */
    fen = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    CSC_FENFromBoard(fixture.position, fen, NULL);
    mu_assert(
        "The received position should be the start position.",
        strcmp(fen, startFen) == 0);

    free(fen);

    return NULL;
}

char* ProcessPositionTest_ValidFENNoMoves()
{
    const char* initialFEN =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    char* fen;
    char cmdBuf[100] = "position ";

    printf("Position valid FEN no moves test\n");
    ResetFixture();

    callbacks.onPosition = &dummyOnPosition;

    strcat(cmdBuf, initialFEN);
    CSC_UCIProcess(cmdBuf, &callbacks);

    mu_assert(
        "We should have received a 'position' command.",
        fixture.onPositionCalled);

    mu_assert(
        "We should have received a position.",
        fixture.position != NULL);

    /* Check that the position is actually the start position by converting
       to a FEN string. */
    fen = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    CSC_FENFromBoard(fixture.position, fen, NULL);
    mu_assert(
        "The received position should be the start position.",
        strcmp(fen, initialFEN) == 0);

    free(fen);

    return NULL;
}

char* ProcessPositionTest_ValidFENWithMovesNoMoves()
{
    const char* initialFEN =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    char* fen;
    char cmdBuf[100] = "position ";
    int len;

    printf("Position valid FEN with 'moves' but no moves test\n");
    ResetFixture();

    callbacks.onPosition = &dummyOnPosition;

    strcat(cmdBuf, initialFEN);
    len = strlen(cmdBuf);
    cmdBuf[len] = ' ';
    cmdBuf[len+1] = '\0';
    strcat(cmdBuf, "moves");
    CSC_UCIProcess(cmdBuf, &callbacks);

    mu_assert(
        "We should have received a 'position' command.",
        fixture.onPositionCalled);

    mu_assert(
        "We should have received a position.",
        fixture.position != NULL);

    /* Check that the position is actually the start position by converting
       to a FEN string. */
    fen = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    CSC_FENFromBoard(fixture.position, fen, NULL);
    mu_assert(
        "The received position should be the start position.",
        strcmp(fen, initialFEN) == 0);

    free(fen);

    return NULL;
}

char* ProcessPositionTest_ValidFENWithMoves()
{
    const char* initialFEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const char* finalFEN =
        "rnbqkb1r/pppppppp/5n2/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1";
    char* fen;
    char cmdBuf[100] = "position ";
    int len;

    printf("Position valid FEN with moves test\n");
    ResetFixture();

    callbacks.onPosition = &dummyOnPosition;

    strcat(cmdBuf, initialFEN);
    len = strlen(cmdBuf);
    cmdBuf[len] = ' ';
    cmdBuf[len+1] = '\0';
    strcat(cmdBuf, "moves e2e4 g8f6 e4e5");
    CSC_UCIProcess(cmdBuf, &callbacks);

    mu_assert(
        "We should have received a 'position' command.",
        fixture.onPositionCalled);

    mu_assert(
        "We should have received a position.",
        fixture.position != NULL);

    /* Check that the position is actually the start position by converting
       to a FEN string. */
    fen = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    CSC_FENFromBoard(fixture.position, fen, NULL);
    mu_assert(
        "The received position should have the moves applied.",
        strcmp(fen, finalFEN) == 0);

    free(fen);

    return NULL;
}

void dummyOnGo(
    struct CSC_SearchConstraints* search,
    struct CSC_TimeConstraints* time)
{
    fixture.onGoCalled = true;

    if (search != NULL && search->depth != NULL)
    {
        fixture.depth = *search->depth;
    }
}

char* ProcessGoTest_Depth()
{
    printf("Go with depth test\n");
    ResetFixture();

    callbacks.onGo = &dummyOnGo;

    CSC_UCIProcess("go depth 10", &callbacks);

    mu_assert(
        "We should have received a 'go' command.",
        fixture.onGoCalled);

    mu_assert(
        "Depth should have been set to 10.",
        fixture.depth == 10);

    return NULL;
}

char* AllUCITests()
{
    printf("Running UCI command tests...\n");
    mu_run_test(ProcessUCITest);
    mu_run_test(ProcessDebugTest_On);
    mu_run_test(ProcessDebugTest_Off);
    mu_run_test(ProcessIsReadyTest);
    mu_run_test(ProcessSetOptionNameTest_Valid);
    mu_run_test(ProcessSetOptionNameTest_NoNameSpecified);
    mu_run_test(ProcessSetOptionNameValueTest_Valid);
    mu_run_test(ProcessSetOptionNameValueTest_NoValueSpecified);
    mu_run_test(ProcessPositionTest_ValidStartPosNoMoves);
    mu_run_test(ProcessPositionTest_ValidFENNoMoves);
    mu_run_test(ProcessPositionTest_ValidFENWithMovesNoMoves);
    mu_run_test(ProcessPositionTest_ValidFENWithMoves);
    mu_run_test(ProcessGoTest_Depth);

    /* Final call to free any allocated memory. */
    ResetFixture();

    return NULL;
}
