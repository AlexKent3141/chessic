#include "../include/chessic.h"
#include "string.h"
#include "stdlib.h"

void ProcessUCICommand(
    struct CSC_UCICallbacks* callbacks)
{
    if (callbacks != NULL && callbacks->onUCI != NULL)
    {
        callbacks->onUCI();
    }
}

void ProcessDebugCommand(
    struct CSC_UCICallbacks* callbacks)
{
    char* arg = strtok(NULL, " ");
    bool debugOn;

    /* We expect an argument specifying whether debug should be turned on
       or off. */
    if (arg == NULL) return;

    debugOn = strcmp(arg, "on") == 0;

    if (callbacks != NULL && callbacks->onDebug != NULL)
    {
        callbacks->onDebug(debugOn);
    }
}

void ProcessIsReadyCommand(
    struct CSC_UCICallbacks* callbacks)
{
    if (callbacks != NULL && callbacks->onIsReady!= NULL)
    {
        callbacks->onIsReady();
    }
}

/* The option can be specified just by name or, if applicable, a value can
   also be specified. */
void ProcessSetOptionCommand(
    struct CSC_UCICallbacks* callbacks)
{
    char* nameCmd, *valueCmd;
    char* name = NULL, *value = NULL;
    
    /* The first argument must be 'name'. */
    nameCmd = strtok(NULL, " ");
    if (nameCmd == NULL || strcmp(nameCmd, "name") != 0) return;

    /* The next argument must be the name of the option. */
    name = strtok(NULL, " ");
    if (name == NULL) return;

    /* The value may not be specified. */
    valueCmd = strtok(NULL, " ");
    if (valueCmd == NULL || strcmp(valueCmd, "value") != 0)
    {
        if (callbacks != NULL && callbacks->onSetOptionName != NULL)
        {
            callbacks->onSetOptionName(name);
        }
    }
    else
    {
        value = strtok(NULL, " ");
        if (value == NULL) return;

        if (callbacks != NULL && callbacks->onSetOptionNameValue != NULL)
        {
            callbacks->onSetOptionNameValue(name, value);
        }
    }
}

void ProcessRegisterCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

void ProcessNewGameCommand(
    struct CSC_UCICallbacks* callbacks)
{
} 
void ProcessPositionCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

void ProcessGoCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

void ProcessStopCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

void ProcessPonderHitCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

void ProcessQuitCommand(
    struct CSC_UCICallbacks* callbacks)
{
}

/* Process the given command and call the corresponding callbacks. */
void CSC_UCIProcess(
    const char* cmd,
    struct CSC_UCICallbacks* callbacks)
{
    int len = strlen(cmd);
    char* copy = malloc((len+1)*sizeof(char));
    char* token;

    strcpy(copy, cmd);
    copy[len] = '\0';

    token = strtok(copy, " ");
    if (token == NULL)
    {
        free(copy);
        return;
    }

    /* The first token tells us what type of command this is. */
    if (strcmp(token, "uci") == 0)
    {
        ProcessUCICommand(callbacks);
    }
    else if (strcmp(token, "debug") == 0)
    {
        ProcessDebugCommand(callbacks);
    }
    else if (strcmp(token, "isready") == 0)
    {
        ProcessIsReadyCommand(callbacks);
    }
    else if (strcmp(token, "setoption") == 0)
    {
        ProcessSetOptionCommand(callbacks);
    }
    else if (strcmp(token, "register") == 0)
    {
        ProcessRegisterCommand(callbacks);
    }
    else if (strcmp(token, "ucinewgame") == 0)
    {
        ProcessNewGameCommand(callbacks);
    }
    else if (strcmp(token, "position") == 0)
    {
        ProcessPositionCommand(callbacks);
    }
    else if (strcmp(token, "go") == 0)
    {
        ProcessGoCommand(callbacks);
    }
    else if (strcmp(token, "stop") == 0)
    {
        ProcessStopCommand(callbacks);
    }
    else if (strcmp(token, "ponderhit") == 0)
    {
        ProcessPonderHitCommand(callbacks);
    }
    else if (strcmp(token, "quit") == 0)
    {
        ProcessQuitCommand(callbacks);
    }

    free(copy);
}

void CSC_UCISendId(
    const char* name,
    const char* author)
{
}

void CSC_UCISendOK()
{
}

void CSC_UCIBestMove(
    CSC_Move move,
    bool ponder)
{
}

void CSC_UCIInfo(
    struct CSC_UCIInfo* info)
{
}

void CSC_UCISupportedOptions(
    struct CSC_UCIOption* options,
    int numOptions)
{
}
