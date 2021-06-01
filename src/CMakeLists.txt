
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

target_include_directories(chessic
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include)
