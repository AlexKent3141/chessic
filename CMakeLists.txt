cmake_minimum_required (VERSION 3.16.3)
project(chessic LANGUAGES C)

# Setup output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Setup compiler flags.
# Currently leaving defaults for everything except Unix.
if (UNIX)
  add_compile_options(-Wall -Wextra -Wshadow -Wpedantic -fno-common -ansi)
endif (UNIX)

include_directories("./include" "./src")

add_subdirectory("./src")
add_subdirectory("./tests")
add_subdirectory("./test_engine")
