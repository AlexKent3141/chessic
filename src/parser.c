#include "../include/chessic.h"
#include "board.h"
#include "zobrist.h"
#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

enum CSC_PieceType PieceTypeFromFEN(char c)
{
    int type = CSC_PAWN;
    switch (c)
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

    return type;
}

void SetPieceFromFEN(struct CSC_Board* b, int loc, char c)
{
    int col = isupper(c) ? CSC_WHITE : CSC_BLACK;
    enum CSC_PieceType type = PieceTypeFromFEN(tolower(c));

    CSC_Bitboard bit = (CSC_Bitboard)1 << loc;
    b->all[col] |= bit;
    b->pieces[type][col] |= bit;
    b->squares[loc] = CSC_CreatePiece(col, type);
    b->bs->hash ^= keys.pieceSquare[col][type][loc];
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

    if (b->player == CSC_WHITE) bs->hash ^= keys.side;

    // Get the castling rights.
    token = strtok(NULL, " ");
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if (c == 'K')
        {
            bs->castlingRights[CSC_WHITE].kingSide = true;
            bs->hash ^= keys.castling[CSC_WHITE][0];
        }
        else if (c == 'Q')
        {
            bs->castlingRights[CSC_WHITE].queenSide = true;
            bs->hash ^= keys.castling[CSC_WHITE][1];
        }
        else if (c == 'k')
        {
            bs->castlingRights[CSC_BLACK].kingSide = true;
            bs->hash ^= keys.castling[CSC_BLACK][0];
        }
        else if (c == 'q')
        {
            bs->castlingRights[CSC_BLACK].queenSide = true;
            bs->hash ^= keys.castling[CSC_BLACK][1];
        }
    }

    // Get the en-passent square.
    token = strtok(NULL, " ");
    if (token[0] != '-')
    {
        bs->enPassentIndex = LocFromFEN(token);
        bs->hash ^= keys.enpassentFile[bs->enPassentIndex % CSC_FILE_NB];
    }

    // Get the half-move count.
    token = strtok(NULL, " ");
    bs->plies50Move = atoi(token);

    // Get the full-move count.
    token = strtok(NULL, " ");
    b->turnNumber = atoi(token);

    free(fenDup);

    return b;
}

void CSC_FENFromBoard(struct CSC_Board* b, char* buf, int* len)
{
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
                if (e) buf[i++] = e + '0';
                e = 0;
                buf[i++] = FENFromPiece(col, type);
            }
            else
            {
                ++e;
            }
        }

        if (e) buf[i++] = e + '0';
        if (r) buf[i++] = '/';
    }

    buf[i++] = ' ';
    buf[i++] = b->player == CSC_WHITE ? 'w' : 'b';

    buf[i++] = ' ';
    int start = i;
    if (bs->castlingRights[CSC_WHITE].kingSide) buf[i++] = 'K';
    if (bs->castlingRights[CSC_WHITE].queenSide) buf[i++] = 'Q';
    if (bs->castlingRights[CSC_BLACK].kingSide) buf[i++] = 'k';
    if (bs->castlingRights[CSC_BLACK].queenSide) buf[i++] = 'q';
    if (i == start) buf[i++] = '-';

    buf[i++] = ' ';
    if (bs->enPassentIndex != CSC_BAD_LOC)
    {
        buf[i++] = (bs->enPassentIndex % 8) + 'a';
        buf[i++] = (bs->enPassentIndex / 8) + '1';
    }
    else
    {
        buf[i++] = '-';
    }

    buf[i++] = ' ';

    // Serialise the half-move count.
    const int MaxTurnLength = 10;
    char* numBuf = malloc(MaxTurnLength*sizeof(char));
    memset(numBuf, 0, MaxTurnLength*sizeof(char));
    snprintf(numBuf, 10, "%d", bs->plies50Move);
    for (size_t j = 0; j < strlen(numBuf); j++) buf[i++] = numBuf[j];

    buf[i++] = ' ';

    // Serialise the number of full turns.
    memset(numBuf, 0, MaxTurnLength*sizeof(char));
    snprintf(numBuf, 10, "%d", b->turnNumber);
    for (size_t j = 0; j < strlen(numBuf); j++) buf[i++] = numBuf[j];

    buf[i] = '\0';

    if (len != NULL) *len = i;

    free(numBuf);
}

void CSC_MoveToUCIString(CSC_Move move, char* buf, int* len)
{
    char start = CSC_GetMoveStart(move);
    char end = CSC_GetMoveEnd(move);
    buf[0] = (start % CSC_FILE_NB) + 'a';
    buf[1] = (start / CSC_RANK_NB) + '1';
    buf[2] = (end % CSC_FILE_NB) + 'a';
    buf[3] = (end / CSC_RANK_NB) + '1';

    int numChars = 4;
    if (CSC_GetMoveType(move) & CSC_PROMOTION)
    {
        /* Specified black here so we get a lowercase character. */
        buf[4] = FENFromPiece(CSC_BLACK, CSC_GetMovePromotion(move));
        numChars = 5;
    }

    buf[numChars] = '\0';

    if (len != NULL) *len = numChars;
}

CSC_Move CSC_MoveFromUCIString(struct CSC_Board* b, const char* buf)
{
    char fileStart = buf[0] - 'a';
    char rankStart = buf[1] - '1';
    char fileEnd = buf[2] - 'a';
    char rankEnd = buf[3] - '1';

    int start = CSC_FILE_NB*rankStart + fileStart;
    int end = CSC_FILE_NB*rankEnd + fileEnd;

    /* Next work out the move type (this is why we need the board). */
    CSC_Piece p = b->squares[start];
    enum CSC_PieceType promotionType = CSC_NONE;
    enum CSC_MoveType mt = CSC_NORMAL;

    if (CSC_GetPieceType(p) == CSC_KING && abs(fileStart - fileEnd) == 2)
    {
        /* Castling move. */
        mt = fileEnd == 6 ? CSC_KINGCASTLE : CSC_QUEENCASTLE;
    }
    else if (CSC_GetPieceType(p) == CSC_PAWN
         && (rankEnd == 0 || rankEnd == CSC_RANK_NB - 1))
    {
        /* Pawn promotion - read the next character for the type. */
        promotionType = PieceTypeFromFEN(buf[4]);
        mt = CSC_PROMOTION;
    }
    else if (CSC_GetPieceType(p) == CSC_PAWN
        && abs(rankStart - rankEnd) == 2)
    {
        mt = CSC_TWOSPACE;
    }
    else if (CSC_GetPieceType(p) == CSC_PAWN
        && fileStart != fileEnd
        && !b->squares[end])
    {
        /* A pawn is performing a capture but there is no target piece.
           This must be an en-passent capture. */
        mt = CSC_ENPASSENT;
    }

    return CSC_CreateMove(start, end, promotionType, mt);
}
