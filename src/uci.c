#include "../include/chessic.h"
#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "utils.h"

void ProcessUCICommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    if (callbacks != NULL && callbacks->onUCI != NULL)
    {
        callbacks->onUCI();
    }
}

void ProcessDebugCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    char* arg = Token(NULL, ' ', state);
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
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    if (callbacks != NULL && callbacks->onIsReady!= NULL)
    {
        callbacks->onIsReady();
    }
}

/* The option can be specified just by name or, if applicable, a value can
   also be specified.
   Note: this function allocates new memory for the character arrays it feeds
   into the callback. */
void ProcessSetOptionCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    char* nameCmd, *valueCmd;
    char* nameToken = NULL, *valueToken = NULL;
    char* name, *value;
    
    /* The first argument must be 'name'. */
    nameCmd = Token(NULL, ' ', state);
    if (nameCmd == NULL || strcmp(nameCmd, "name") != 0) return;

    /* The next argument must be the name of the option. */
    nameToken = Token(NULL, ' ', state);
    if (nameToken == NULL) return;

    name = malloc((strlen(nameToken)+1)*sizeof(char));
    memcpy(name, nameToken, strlen(nameToken)*sizeof(char));
    name[strlen(nameToken)] = '\0';

    /* The value may not be specified. */
    valueCmd = Token(NULL, ' ', state);
    if (valueCmd == NULL || strcmp(valueCmd, "value") != 0)
    {
        if (callbacks != NULL && callbacks->onSetOptionName != NULL)
        {
            callbacks->onSetOptionName(name);
        }
        else
        {
            free(name);
        }
    }
    else
    {
        valueToken = Token(NULL, ' ', state);
        if (valueToken != NULL &&
            callbacks != NULL &&
            callbacks->onSetOptionNameValue != NULL)
        {
            value = malloc((strlen(valueToken)+1)*sizeof(char));
            memcpy(value, valueToken, strlen(valueToken)*sizeof(char));
            value[strlen(valueToken)] = '\0';

            callbacks->onSetOptionNameValue(name, value);
        }
        else
        {
            free(name);
        }
    }
}

void ProcessRegisterCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    /* TODO: Not so interested in this so leaving for now. */
}

void ProcessNewGameCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    if (callbacks != NULL && callbacks->onNewGame != NULL)
    {
        callbacks->onNewGame();
    }
} 

void ProcessPositionCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    const char* startFen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* moveBuf, *token;
    char fen[CSC_MAX_FEN_LENGTH];
    struct CSC_Board* position;
    CSC_Move move;

    /* The first argument(s) should be either a fen or 'startpos'. */
    token = Token(NULL, ' ', state);
    if (token == NULL) return;

    if (strcmp(token, "startpos") == 0)
    {
        strcpy(fen, startFen);
    }
    else
    {
        /* Until we find the 'moves' string, or we reach a NULL token, keep
           appending to the FEN string. */
        strcpy(fen, token);
        token = Token(NULL, ' ', state);
        while (token != NULL && strcmp(token, "moves") != 0)
        {
            /* Insert a space. */
            int len = strlen(fen);
            fen[len] = ' ';
            fen[len+1] = '\0';

            strcat(fen, token);
            token = Token(NULL, ' ', state);
        }
    }

    position = CSC_BoardFromFEN(fen);
    assert(position != NULL);

    if (token != NULL && strcmp(token, "moves") == 0)
    {
        /* Next can follow a series of moves. Each move should be applied to the
           position specified in the FEN string. */
        moveBuf = Token(NULL, ' ', state);
        while (moveBuf != NULL)
        {
            move = CSC_MoveFromUCIString(position, moveBuf);
            CSC_MakeMove(position, move);
            moveBuf = Token(NULL, ' ', state);
        }
    }

    if (callbacks != NULL && callbacks->onPosition != NULL)
    {
        callbacks->onPosition(position);
    }
}

void ProcessGoCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    struct CSC_SearchConstraints search;
    struct CSC_TimeConstraints time;
    char* token, *value;

    /* Can take the address of these variables to fill in the constraints. */
    int depth, numNodes, mate;
    int wTime, bTime, wInc, bInc, movesToGo, moveTime;
    bool ponder, infinite;

    /* This sets pointer members to NULL. */
    memset(&search, 0, sizeof(struct CSC_SearchConstraints));
    memset(&time, 0, sizeof(struct CSC_TimeConstraints));

    while ((token = Token(NULL, ' ', state)) != NULL)
    {
        if (strcmp(token, "searchmoves") == 0)
        {
            /* TODO: This is awkward because in order to parse the moves we need
               to know the board state. Possible solution would be to just pass
               back the string representations of the moves so that the user
               actually does the parsing. */
        }
        else if (strcmp(token, "ponder") == 0)
        {
            ponder = true;
            search.ponder = &ponder;
        }
        else if (strcmp(token, "wtime") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                wTime = atoi(value);
                time.wTime = &wTime;
            }
        }
        else if (strcmp(token, "btime") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                bTime = atoi(value);
                time.bTime = &bTime;
            }
        }
        else if (strcmp(token, "winc") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                wInc = atoi(value);
                time.wInc = &wInc;
            }
        }
        else if (strcmp(token, "binc") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                bInc = atoi(value);
                time.bInc = &bInc;
            }
        }
        else if (strcmp(token, "movestogo") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                movesToGo = atoi(value);
                time.movesToGo = &movesToGo;
            }
        }
        else if (strcmp(token, "depth") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                depth = atoi(value);
                search.depth = &depth;
            }
        }
        else if (strcmp(token, "nodes") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                numNodes = atoi(value);
                search.numNodes = &numNodes;
            }
        }
        else if (strcmp(token, "mate") == 0)
        {
            value = Token(NULL, ' ', state);
            if (value != NULL)
            {
                mate = atoi(value);
                search.mate = &mate;
            }
        }
        else if (strcmp(token, "infinite") == 0)
        {
            infinite = true;
            time.infinite = &infinite;
        }

        /* Tokens that we don't recognise are ignored. */
    }

    if (callbacks != NULL && callbacks->onGo != NULL)
    {
        callbacks->onGo(&search, &time);
    }
}

void ProcessStopCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    /* TODO */
}

void ProcessPonderHitCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    /* TODO */
}

void ProcessQuitCommand(
    struct CSC_UCICallbacks* callbacks,
    struct TokenState* state)
{
    /* TODO */
}

/* Process the given command and call the corresponding callbacks. */
void CSC_UCIProcess(
    const char* cmd,
    struct CSC_UCICallbacks* callbacks)
{
    struct TokenState state;
    int len = strlen(cmd);
    char* copy = malloc((len+1)*sizeof(char));
    char* token;

    strcpy(copy, cmd);
    copy[len] = '\0';

    token = Token(copy, ' ', &state);
    if (token == NULL)
    {
        free(copy);
        return;
    }

    /* The first token tells us what type of command this is. */
    if (strcmp(token, "uci") == 0)
    {
        ProcessUCICommand(callbacks, &state);
    }
    else if (strcmp(token, "debug") == 0)
    {
        ProcessDebugCommand(callbacks, &state);
    }
    else if (strcmp(token, "isready") == 0)
    {
        ProcessIsReadyCommand(callbacks, &state);
    }
    else if (strcmp(token, "setoption") == 0)
    {
        ProcessSetOptionCommand(callbacks, &state);
    }
    else if (strcmp(token, "register") == 0)
    {
        ProcessRegisterCommand(callbacks, &state);
    }
    else if (strcmp(token, "ucinewgame") == 0)
    {
        ProcessNewGameCommand(callbacks, &state);
    }
    else if (strcmp(token, "position") == 0)
    {
        ProcessPositionCommand(callbacks, &state);
    }
    else if (strcmp(token, "go") == 0)
    {
        ProcessGoCommand(callbacks, &state);
    }
    else if (strcmp(token, "stop") == 0)
    {
        ProcessStopCommand(callbacks, &state);
    }
    else if (strcmp(token, "ponderhit") == 0)
    {
        ProcessPonderHitCommand(callbacks, &state);
    }
    else if (strcmp(token, "quit") == 0)
    {
        ProcessQuitCommand(callbacks, &state);
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
