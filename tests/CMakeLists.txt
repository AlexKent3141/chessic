
add_executable(test
  make_undo_tests.c
  movegen_tests.c
  parser_tests.c
  perft_tests.c
  token_tests.c
  uci_tests.c
  test.c)

target_link_libraries(test
  chessic)

add_custom_command(TARGET test POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/perftsuite.epd
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/perftsuite.epd)
