
add_library(chessic
  STATIC
    bits.c
    board.c
    board_state.c
    move.c
    movegen.c
    parser.c
    uci.c
    token.c
    zobrist.c)
