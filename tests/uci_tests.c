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

    bool debug;
    char* optionName;
    char* optionValue;
    struct CSC_Board* position;
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

    /* Final call to free any allocated memory. */
    ResetFixture();

    return NULL;
}
