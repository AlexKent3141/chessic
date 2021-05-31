# Chessic
Chessic is a chess utility library written in ANSI C that aims to provide the functionality that is common to all chess engines. I decided to create this library because historically chess programs each seem to re-implement a lot of functions which could potentially be reused.

## <ins>Components</ins>

### Initialisation
Before using any of Chessic's routines you must initialise it by calling `CSC_InitBits` and `CSC_InitZobrist`.

### Board structure/functions
The `CSC_Board` structure represents the current board state and its history, it is normally created from a FEN string using the `CSC_BoardFromFEN` function. There are a number of functions to query this structure, for example `CSC_GetEnPassentIndex`.

Making and undoing a move is done using the `CSC_MakeMove` and `CSC_UndoMove` functions.

### Move generation
The `CSC_GetMoves` function uses bitboards to quickly generate legal moves of a specific type. The raw bitboards are exposed to the user (e.g. `CSC_Ranks`) so they can be used for evaluation etc.

### UCI protocol support
A large subset of the UCI protocol commands are supported. This part of the API works using a callback pattern where clients register callbacks for the messages they're interested in by passing a `CSC_UCICallbacks` object to the `CSC_UCIProcess` function.

## <ins>Tests and examples</ins>
There are a number of tests and examples, including a test chess engine that reports random moves without searching. The test engine is the recommended starting point if you want to start using Chessic.
* The `tests` target builds the unit test executable which also runs perft.
* The `test_engine` target builds a small example engine (see `test_engine\main.c` for an example of how to use Chessic).
