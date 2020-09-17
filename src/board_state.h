#ifndef __CHESSIC_BOARD_STATE_H__
#define __CHESSIC_BOARD_STATE_H__

#include "../include/chessic.h"
#include "stdlib.h"

struct BoardState
{
    /* The move that was applied to reach this state. */
    CSC_Move lastMove;

    /* The piece that was captured on the last move. */
    CSC_Piece lastMoveCapture;

    /* The type of piece that moved on the last move. */
    enum CSC_PieceType lastMovePieceType;

    /* The index of the square where an en-passent capture is possible. */
    int enPassentIndex;

    /* The number of plies since the last move that reset the 50 move rule. */
    int plies50Move;

    /* The castling rights for each player. */
    struct CSC_CastlingRights castlingRights[2];

    /* The current board hash. */
    CSC_Hash hash;

    /* The previous game state. */
    struct BoardState* previousState;
};

struct StateStack
{
    struct BoardState* data;
    size_t dataSize;
    size_t head;
};

struct StateStack* CreateStack();

void FreeStack(struct StateStack*);

struct StateStack* CopyStack(struct StateStack*);

/* Increase the size of the stack by 1 and get a pointer to the new element. */
struct BoardState* Push(struct StateStack*);

/* Reduce the size of the stack by 1. */
struct BoardState* Pop(struct StateStack*);

/* Access the top element of the stack. */
struct BoardState* Top(struct StateStack*);

#endif /* __CHESSIC_BOARD_STATE_H__ */
