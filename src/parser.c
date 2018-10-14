#include "parser.h"
#include "utils.h"
#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

board* board_from_fen(const char* fen)
{
    board* b = create_board_empty();
    state* bs = b->bs;

    char* fen_dup = malloc(strlen(fen)*sizeof(char) + 1);
    strcpy(fen_dup, fen);

    // Get the piece definitions.
    char c;
    char* token = strtok(fen_dup, " ");
    int f = 0; int r = 7;
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if (isalpha(c))
        {
            set_piece_from_fen(b, 8*r + f, c);
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
    b->player = token[0] == 'w' ? WHITE : BLACK;

    // Get the castling rights.
    token = strtok(NULL, " ");
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if      (c == 'K') bs->crs[WHITE].ks = true;
        else if (c == 'Q') bs->crs[WHITE].qs = true;
        else if (c == 'k') bs->crs[BLACK].ks = true;
        else if (c == 'q') bs->crs[BLACK].qs = true;
    }

    // Get the en-passent square.
    token = strtok(NULL, " ");
    if (token[0] != '-') bs->ep = loc_from_fen(token);

    // Get the half-move count.
    token = strtok(NULL, " ");
    bs->plies_50_move = atoi(token);

    // Get the full-move count.
    token = strtok(NULL, " ");
    b->turn = atoi(token);

    free(fen_dup);

    return b;
}

char* fen_from_board(board* b)
{
    char* fen = malloc(MAX_FEN_LENGTH*sizeof(char));
    memset(fen, 0, MAX_FEN_LENGTH*sizeof(char));

    state* bs = b->bs;

    // Set the piece definitions.
    int i = 0, e;
    int col, type;
    for (int r = 7; r >= 0; r--)
    {
        e = 0;
        for (int f = 0; f < 8; f++)
        {
            loc_details(b, 8*r+f, &col, &type);
            if (type != NONE)
            {
                if (e) fen[i++] = e + '0';
                e = 0;
                fen[i++] = fen_from_piece(col, type);
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
    fen[i++] = b->player == WHITE ? 'w' : 'b';

    fen[i++] = ' ';
    int start = i;
    if (bs->crs[WHITE].ks) fen[i++] = 'K';
    if (bs->crs[WHITE].qs) fen[i++] = 'Q';
    if (bs->crs[BLACK].ks) fen[i++] = 'k';
    if (bs->crs[BLACK].qs) fen[i++] = 'q';
    if (i == start) fen[i++] = '-';

    fen[i++] = ' ';
    if (bs->ep != BAD_LOC)
    {
        fen[i++] = (bs->ep % 8) + 'a';
        fen[i++] = (bs->ep / 8) + '1';
    }
    else
    {
        fen[i++] = '-';
    }

    fen[i++] = ' ';

    // Serialise the half-move count.
    const int MAX_TURN_LENGTH = 10;
    char* buf = malloc(MAX_TURN_LENGTH*sizeof(char));
    memset(buf, 0, MAX_TURN_LENGTH*sizeof(char));
    snprintf(buf, 10, "%d", bs->plies_50_move);
    for (size_t j = 0; j < strlen(buf); j++) fen[i++] = buf[j];

    fen[i++] = ' ';

    // Serialise the number of full turns.
    memset(buf, 0, MAX_TURN_LENGTH*sizeof(char));
    snprintf(buf, 10, "%d", b->turn);
    for (size_t j = 0; j < strlen(buf); j++) fen[i++] = buf[j];

    fen[i] = '\0';

    return fen;
}

void set_piece_from_fen(board* b, int loc, char c)
{
    int col = isupper(c) ? WHITE : BLACK;
    char l = tolower(c);
    int type;
    switch (l)
    {
        case 'p':
            type = PAWN;
            break;
        case 'n':
            type = KNIGHT;
            break;
        case 'b':
            type = BISHOP;
            break;
        case 'r':
            type = ROOK;
            break;
        case 'q':
            type = QUEEN;
            break;
        case 'k':
            type = KING;
            break;
    }

    bb bit = (bb)1 << loc;
    b->all[col] |= bit;
    b->pieces[type][col] |= bit;
    b->squares[loc] = create_piece(col, type);
}

char fen_from_piece(int col, int type)
{
    char c;
    switch (type)
    {
        case PAWN:
            c = 'p';
            break;
        case KNIGHT:
            c = 'n';
            break;
        case BISHOP:
            c = 'b';
            break;
        case ROOK:
            c = 'r';
            break;
        case QUEEN:
            c = 'q';
            break;
        case KING:
            c = 'k';
            break;
    }

    if (col == WHITE) c = toupper(c);

    return c;
}

int loc_from_fen(const char* loc)
{
    int f = loc[0] - 'a';
    int r = loc[1] - '1';
    return 8*r + f;
}
