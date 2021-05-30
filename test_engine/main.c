#include "chessic.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

int quit = 0;
struct CSC_Board* latestPosition = NULL;

void onUCI()
{
    CSC_UCISendId("Chessic test engine", "AlexKent3141");
}

void onIsReady()
{
    CSC_UCISendReadyOK();
}

void onPosition(struct CSC_Board* board)
{
    if (latestPosition != NULL)
    {
        CSC_FreeBoard(latestPosition);
    }

    latestPosition = board;
}

/* This is where we would execute the search algorithm.
   For this test engine just report a random best move. */
void onGo()
{
    struct CSC_MoveList* list = CSC_MakeMoveList();
    CSC_Move bestMove;

    if (latestPosition == NULL)
    {
        return;
    }

    CSC_GetMoves(latestPosition, list, CSC_ALL);
    if (list->n > 0)
    {
        bestMove = list->moves[rand() % list->n];
        CSC_UCIBestMove(bestMove, NULL);
    }

    CSC_FreeMoveList(list);
}

void onQuit()
{
    quit = 1;
}

int main()
{
    struct CSC_UCICallbacks callbacks;
    char buf[CSC_MAX_UCI_COMMAND_LENGTH];
    FILE* log = fopen("log.txt", "a");

    CSC_InitBits();
    CSC_InitZobrist();

    srand(time(NULL));

    memset(&callbacks, 0, sizeof(struct CSC_UCICallbacks));

    callbacks.onUCI = &onUCI;
    callbacks.onIsReady = &onIsReady;
    callbacks.onPosition = &onPosition;
    callbacks.onGo = &onGo;
    callbacks.onQuit = &onQuit;

    while (!quit)
    {
        memset(buf, 0, CSC_MAX_UCI_COMMAND_LENGTH*sizeof(char));
        if (fgets(buf, CSC_MAX_UCI_COMMAND_LENGTH, stdin) != NULL)
        {
            fwrite(buf, sizeof(char), strlen(buf), log);
            fflush(log);

            /* Trim off the ending newline character. */
            buf[strlen(buf)-1] = '\0';
            CSC_UCIProcess(buf, &callbacks);
        }
    }

    if (latestPosition != NULL)
    {
        CSC_FreeBoard(latestPosition);
    }

    fclose(log);

    return 0;
}
