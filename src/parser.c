#include "../include/chessic.h"
#include "board.h"
#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void SetPieceFromFEN(struct CSC_Board* b, int loc, char c)
{
    int col = isupper(c) ? CSC_WHITE : CSC_BLACK;
    char l = tolower(c);
    int type = CSC_PAWN;
    switch (l)
    {
        case 'n':
            type = CSC_KNIGHT;
            break;
        case 'b':
            type = CSC_BISHOP;
            break;
        case 'r':
            type = CSC_ROOK;
            break;
        case 'q':
            type = CSC_QUEEN;
            break;
        case 'k':
            type = CSC_KING;
            break;
    }

    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    b->all[col] |= bit;
    b->pieces[type][col] |= bit;
    b->squares[loc] = CSC_CreatePiece(col, type);
}

char FENFromPiece(int col, int type)
{
    char c = 'p';
    switch (type)
    {
        case CSC_KNIGHT:
            c = 'n';
            break;
        case CSC_BISHOP:
            c = 'b';
            break;
        case CSC_ROOK:
            c = 'r';
            break;
        case CSC_QUEEN:
            c = 'q';
            break;
        case CSC_KING:
            c = 'k';
            break;
    }

    if (col == CSC_WHITE) c = toupper(c);

    return c;
}

int LocFromFEN(const char* loc)
{
    int f = loc[0] - 'a';
    int r = loc[1] - '1';
    return 8*r + f;
}

struct CSC_Board* CSC_BoardFromFEN(const char* fen)
{
    struct CSC_Board* b = CreateBoardEmpty();
    struct CSC_BoardState* bs = b->bs;

    char* fenDup = malloc(strlen(fen)*sizeof(char) + 1);
    strcpy(fenDup, fen);

    // Get the piece definitions.
    char c;
    char* token = strtok(fenDup, " ");
    int f = 0; int r = 7;
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if (isalpha(c))
        {
            SetPieceFromFEN(b, 8*r + f, c);
            ++f;
        }
        else if (c == '/')
        {
            --r;
            f = 0;
        }
        else
        {
            f += c - '0';
        }
    }

    // Get the colour to move.
    token = strtok(NULL, " ");
    b->player = token[0] == 'w' ? CSC_WHITE : CSC_BLACK;

    // Get the castling rights.
    token = strtok(NULL, " ");
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if      (c == 'K') bs->castlingRights[CSC_WHITE].kingSide = true;
        else if (c == 'Q') bs->castlingRights[CSC_WHITE].queenSide = true;
        else if (c == 'k') bs->castlingRights[CSC_BLACK].kingSide = true;
        else if (c == 'q') bs->castlingRights[CSC_BLACK].queenSide = true;
    }

    // Get the en-passent square.
    token = strtok(NULL, " ");
    if (token[0] != '-') bs->enPassentIndex = LocFromFEN(token);

    // Get the half-move count.
    token = strtok(NULL, " ");
    bs->plies50Move = atoi(token);

    // Get the full-move count.
    token = strtok(NULL, " ");
    b->turnNumber = atoi(token);

    free(fenDup);

    return b;
}

char* CSC_FENFromBoard(struct CSC_Board* b)
{
    char* fen = malloc(CSC_MAX_FEN_LENGTH*sizeof(char));
    memset(fen, 0, CSC_MAX_FEN_LENGTH*sizeof(char));

    struct CSC_BoardState* bs = b->bs;

    // Set the piece definitions.
    int i = 0, e;
    int col, type;
    for (int r = 7; r >= 0; r--)
    {
        e = 0;
        for (int f = 0; f < 8; f++)
        {
            LocDetails(b, 8*r+f, &col, &type);
            if (type != CSC_NONE)
            {
                if (e) fen[i++] = e + '0';
                e = 0;
                fen[i++] = FENFromPiece(col, type);
            }
            else
            {
                ++e;
            }
        }

        if (e) fen[i++] = e + '0';
        if (r) fen[i++] = '/';
    }

    fen[i++] = ' ';
    fen[i++] = b->player == CSC_WHITE ? 'w' : 'b';

    fen[i++] = ' ';
    int start = i;
    if (bs->castlingRights[CSC_WHITE].kingSide) fen[i++] = 'K';
    if (bs->castlingRights[CSC_WHITE].queenSide) fen[i++] = 'Q';
    if (bs->castlingRights[CSC_BLACK].kingSide) fen[i++] = 'k';
    if (bs->castlingRights[CSC_BLACK].queenSide) fen[i++] = 'q';
    if (i == start) fen[i++] = '-';

    fen[i++] = ' ';
    if (bs->enPassentIndex != CSC_BAD_LOC)
    {
        fen[i++] = (bs->enPassentIndex % 8) + 'a';
        fen[i++] = (bs->enPassentIndex / 8) + '1';
    }
    else
    {
        fen[i++] = '-';
    }

    fen[i++] = ' ';

    // Serialise the half-move count.
    const int MaxTurnLength = 10;
    char* buf = malloc(MaxTurnLength*sizeof(char));
    memset(buf, 0, MaxTurnLength*sizeof(char));
    snprintf(buf, 10, "%d", bs->plies50Move);
    for (size_t j = 0; j < strlen(buf); j++) fen[i++] = buf[j];

    fen[i++] = ' ';

    // Serialise the number of full turns.
    memset(buf, 0, MaxTurnLength*sizeof(char));
    snprintf(buf, 10, "%d", b->turnNumber);
    for (size_t j = 0; j < strlen(buf); j++) fen[i++] = buf[j];

    fen[i] = '\0';

    free(buf);

    return fen;
}
