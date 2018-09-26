#include "parser.h"
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
    int file = 0; int rank = 7;
    for (size_t i = 0; i < strlen(token); i++)
    {
        c = token[i];
        if (isalpha(c))
        {
            int loc = file_rank_to_board(file, rank);
            b->squares[loc] = square_from_fen(c);
            ++file;
        }
        else if (c == '/')
        {
            --rank;
            file = 0;
        }
        else
        {
            file += c - '0';
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
        if      (c == 'K') bs->wc.ks = true;
        else if (c == 'Q') bs->wc.qs = true;
        else if (c == 'k') bs->bc.ks = true;
        else if (c == 'q') bs->bc.qs = true;
    }

    // Get the en-passent square.
    token = strtok(NULL, " ");
    if (token[0] != '-') bs->ep = coord_to_board(coord_from_fen(token));

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
    const int MAX_FEN_LENGTH = 100;
    char* fen = malloc(MAX_FEN_LENGTH*sizeof(char));
    memset(fen, 0, MAX_FEN_LENGTH*sizeof(char));

    state* bs = b->bs;

    // Set the piece definitions.
    int i = 0, e, loc;
    char s;
    for (int r = 7; r >= 0; r--)
    {
        e = 0;
        for (int f = 0; f < 8; f++)
        {
            loc = file_rank_to_board(f, r);
            s = b->squares[loc];
            if (s)
            {
                if (e)
                {
                    fen[i++] = e + '0';
                    e = 0;
                }

                fen[i++] = fen_from_square(s);
            }
            else
            {
                ++e;
            }
        }

        if (e)
        {
            fen[i++] = e + '0';
            e = 0;
        }

        if (r) fen[i++] = '/';
    }

    fen[i++] = ' ';
    fen[i++] = b->player == WHITE ? 'w' : 'b';

    fen[i++] = ' ';
    int start = i;
    if (bs->wc.ks) fen[i++] = 'K';
    if (bs->wc.qs) fen[i++] = 'Q';
    if (bs->bc.ks) fen[i++] = 'k';
    if (bs->bc.qs) fen[i++] = 'q';
    if (i == start) fen[i++] = '-';

    fen[i++] = ' ';
    if (bs->ep != BAD_LOC)
    {
        int coord = board_to_coord(bs->ep);
        fen[i++] = (coord % 8) + 'a';
        fen[i++] = (coord / 8) + '1';
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

char square_from_fen(char c)
{
    int col = isupper(c) ? WHITE : BLACK;
    char l = tolower(c);
    int p = NONE;
    switch (l)
    {
        case 'p':
            p = PAWN;
            break;
        case 'n':
            p = KNIGHT;
            break;
        case 'b':
            p = BISHOP;
            break;
        case 'r':
            p = ROOK;
            break;
        case 'q':
            p = QUEEN;
            break;
        case 'k':
            p = KING;
            break;
    }

    return create_square(col, p);
}

char fen_from_square(char s)
{
    int p = piece_from_square(s);
    int col = col_from_square(s);

    assert(p);

    char c;
    switch (p)
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

int coord_from_fen(const char* loc)
{
    int file = loc[0] - 'a';
    int rank = loc[1] - '1';
    return 8*rank + file;
}
