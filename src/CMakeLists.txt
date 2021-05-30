
add_library(chessic
  SHARED
    bits.c
    board.c
    board_state.c
    move.c
    movegen.c
    parser.c
    piece.c
    uci.c
    token.c
    zobrist.c)

set_target_properties(chessic
  PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden)
