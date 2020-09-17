#ifndef __CHESSIC_ZOBRIST_H__
#define __CHESSIC_ZOBRIST_H__

/* Generate the Zobrist keys to use for repetition detection. */
struct ZobristKeys
{
    /* Indexed by: player, piece type, board location. */
    uint64_t pieceSquare[2][7][CSC_SQUARE_NB];

    /* Indexed by the file on which enpassent is available. */
    uint64_t enpassentFile[CSC_FILE_NB];

    /* Indexed by: player, king/queenside. */
    uint64_t castling[2][2];

    /* Alternate each turn. */
    uint64_t side;
};

extern struct ZobristKeys keys;

#endif /* __CHESSIC_ZOBRIST_H__ */
