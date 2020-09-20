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

    bool debug;
    char* optionName;
    char* optionValue;
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

void dummyOnDebug(bool debug)
{
    fixture.onDebugCalled = true;
    fixture.debug = debug;
}

void dummyOnIsReady()
{
    fixture.onIsReadyCalled = true;
}

void dummyOnSetOptionName(const char* name)
{
    fixture.onSetOptionNameCalled = true;

    if (name != NULL)
    {
        fixture.optionName = malloc((strlen(name)+1)*sizeof(char));
        strcpy(fixture.optionName, name);
    }
}

void dummyOnSetOptionNameValue(const char* name, const char* value)
{
    fixture.onSetOptionNameValueCalled = true;

    if (name != NULL)
    {
        fixture.optionName = malloc((strlen(name)+1)*sizeof(char));
        strcpy(fixture.optionName, name);
    }

    if (value != NULL)
    {
        fixture.optionValue = malloc((strlen(value)+1)*sizeof(char));
        strcpy(fixture.optionValue, value);
    }
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

    return NULL;
}
